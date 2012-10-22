//
//  ImportedSymbol.h
//  pefdump
//
//  Created by Félix on 2012-10-21.
//  Copyright (c) 2012 Félix. All rights reserved.
//

#ifndef __pefdump__ImportedSymbol__
#define __pefdump__ImportedSymbol__

#include <string>
#include "Structures.h"

namespace PEF
{
	class ImportedSymbol
	{
	public:
		ImportedSymbol(const std::string& name, bool isStronglyLinked, SymbolClasses::Enum symbolClass);
		
		std::string Name;
		intptr_t Address;
		bool IsStronglyLinked;
		SymbolClasses::Enum Class;
	};
}

#endif /* defined(__pefdump__ImportedSymbol__) */
