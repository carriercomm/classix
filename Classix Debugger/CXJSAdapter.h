//
// CXJSAdapter.h
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

#import <Foundation/Foundation.h>
#import <WebKit/WebKit.h>

@class CXDocument;

@interface CXJSAdapter : NSObject
{
	CXDocument* document;
}

+(BOOL)isSelectorExcludedFromWebScript:(SEL)selector;

-(id)initWithDocument:(CXDocument*)document;

-(void)setPC:(uint32_t)pc;

-(NSDictionary*)representationsOfGPR:(uint32_t)gpr;
-(NSDictionary*)representationsOfFPR:(uint32_t)fpr;
-(NSDictionary*)representationsOfSPR:(uint32_t)spr;
-(NSDictionary*)representationsOfCR:(uint32_t)cr;
-(NSDictionary*)representationsOfMemoryAddress:(uint32_t)memoryAddress;
-(NSString*)jsonize:(id<NSObject>)object;

-(NSArray*)breakpoints;
-(BOOL)toggleBreakpoint:(uint32_t)address;

-(void)setDisplayName:(NSString*)displayName ofLabel:(NSString*)labelUniqueID;

-(void)dealloc;

@end
