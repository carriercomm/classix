//
// CXObjcDisassemblyWriter.h
// Classix
//
// Copyright (C) 2012 Félix Cloutier
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

#ifndef __Classix__CXObjcDisassemblyWriter__
#define __Classix__CXObjcDisassemblyWriter__

#include "FancyDisassembler.h"
#import <Cocoa/Cocoa.h>

class CXObjCDisassemblyWriter : public PPCVM::Disassembly::DisassemblyWriter
{
	NSMutableArray* result;
	NSMutableArray* currentArray;
	
	uint32_t desiredSection;
	bool inSection;
	
public:
	CXObjCDisassemblyWriter(uint32_t desiredSection);
	
	virtual void EnterSection(const PEF::InstantiableSection& section, uint32_t sectionIndex) override;
	virtual void EnterLabel(const PPCVM::Disassembly::InstructionRange& label, intptr_t labelAddress) override;
	virtual void VisitOpcode(const PPCVM::Disassembly::DisassembledOpcode& opcode, intptr_t opcodeAddress, const std::string* metadata) override;
	
	NSArray* GetDisassembly();
	
	virtual ~CXObjCDisassemblyWriter() override;
};

#endif /* defined(__Classix__CXObjcDisassemblyWriter__) */
