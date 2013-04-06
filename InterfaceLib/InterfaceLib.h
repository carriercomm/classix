//
// InterfaceLib.h
// Classix
//
// Copyright (C) 2013 Félix Cloutier
//
// This file is part of Classix.
//
// Classix is free software: you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the Free Software
// Foundation, either version 3 of the License, or (at your option) any later
// version.
//
// Classix is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with
// Classix. If not, see http://www.gnu.org/licenses/.
//

#include <type_traits>
#include <unistd.h>

#include "CommonDefinitions.h"
#include "IAllocator.h"
#include "MachineState.h"
#include "SymbolType.h"
#include "BigEndian.h"
#include "ResourceManager.h"

namespace InterfaceLib
{
	union Pipe
	{
		int fd[2];
		struct
		{
			int read;
			int write;
		};
	};
	
	class UIChannel
	{
		Pipe read;
		Pipe write;
		pid_t head;
		
		template<typename T>
		T ReturnNonVoid(const uint8_t* buffer)
		{
			return *reinterpret_cast<const T*>(buffer);
		}
		
	public:
		UIChannel();
		
		template<typename TReturnType, typename... TArgument>
		TReturnType PerformAction(IPCMessage message, TArgument&&... argument)
		{
			static_assert(std::is_void<TReturnType>::value || std::is_trivially_copy_constructible<TReturnType>::value,
			"Using DoMessage with a non-trivial type");
			static_assert(!std::is_pointer<TReturnType>::value, "Using DoMessage with a pointer type");
			
			char doneReference[4] = {'D', 'O', 'N', 'E'};
			
			::write(write.write, &message, sizeof message);
			PACK_EXPAND(::write(write.write, &argument, sizeof argument));
			::write(write.write, doneReference, sizeof doneReference);
			
			uint8_t response[sizeof(TReturnType)];
			if (!std::is_void<TReturnType>::value)
				::read(read.read, response, sizeof response);
			
			char done[4];
			::read(read.read, done, sizeof done);
			if (memcmp(done, doneReference, sizeof doneReference) != 0)
				throw std::logic_error("Wrong return type for action");
			
			return ReturnNonVoid<TReturnType>(response);
		}
		
		~UIChannel();
	};
	
	struct Globals
	{
		GrafPort port;
		
		uint8_t padding[0x1000];
		uint32_t systemFatalErrorHandler;
		
		Common::IAllocator& allocator;
		ResourceManager resources;
		UIChannel ipc;
		
		Globals(Common::IAllocator& allocator);
	};
}

extern "C"
{
	InterfaceLib::Globals* LibraryLoad(Common::IAllocator* allocator);
	SymbolType LibraryLookup(InterfaceLib::Globals* globals, const char* symbolName, void** symbol);
	void LibraryUnload(InterfaceLib::Globals* context);
	extern const char* LibrarySymbolNames[];
	
	void InterfaceLib___LibraryInit(InterfaceLib::Globals* globals, PPCVM::MachineState* state);
}

