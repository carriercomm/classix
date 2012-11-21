//
//  FileMapper.h
//  pefdump
//
//  Created by Félix on 2012-10-20.
//  Copyright (c) 2012 Félix. All rights reserved.
//

#ifndef __pefdump__FileMapper__
#define __pefdump__FileMapper__

#include <string>

namespace Common
{
	class FileMapping
	{
		long long fileSize;
		void* address;
		
	public:
		FileMapping(const std::string& filePath);
		FileMapping(const FileMapping& that) = delete;
		FileMapping(FileMapping&& that);
		FileMapping(int fd);
		
		long long size() const;
		
		void* begin();
		void* end();
		
		const void* begin() const;
		const void* end() const;
		
		~FileMapping();
	};
}

#endif /* defined(__pefdump__FileMapper__) */
