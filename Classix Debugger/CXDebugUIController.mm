//
// CXDebugUIController.mm
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

#import "CXDebugUIController.h"
#import "CXDocument.h"
#import "CXStackFrame.h"
#import "CXJSONEncode.h"
#import "CXDocumentController.h"
#import "CXRegister.h"
#import "CXHexFormatter.h"
#import "CXUnmangle.h"

#include "SymbolResolver.h"
#include "Allocator.h"
#include "FragmentManager.h"
#include "PEFSymbolResolver.h"
#include "FancyDisassembler.h"
#include "CXObjcDisassemblyWriter.h"

static NSImage* exportImage;
static NSImage* labelImage;
static NSImage* functionImage;
static NSNib* uiNib;

@interface CXDebugUIController (Private)

-(void)buildSymbolMenu;
-(void)highlightPC;
-(void)jumpToPC;
-(void)showDisassembly:(NSString *)labelUniqueName;
-(void)showDisassembly:(NSString *)labelUniqueName address:(uint32_t)address;
-(void)reloadStackTrace;
-(NSImage*)fileIcon16x16:(NSString*)path;
-(NSMenu*)exportMenuForResolver:(const CFM::SymbolResolver*)resolver;
-(uint32_t)addressOfEntryPoint:(int)entryPointIndex;

@end

@implementation CXDebugUIController

@synthesize disassemblyView;
@synthesize navBar;
@synthesize backForward;
@synthesize outline;
@synthesize stackTraceTable;
@synthesize topLevelObjects;
@synthesize windowController;

+(void)initialize
{
	NSBundle* bundle = [NSBundle bundleForClass:self];
	NSString* exportPath = [bundle pathForResource:@"export" ofType:@"png"];
	NSString* labelPath = [bundle pathForResource:@"label" ofType:@"png"];
	NSString* functionPath = [bundle pathForResource:@"function" ofType:@"png"];
	exportImage = [[NSImage alloc] initWithContentsOfFile:exportPath];
	labelImage = [[NSImage alloc] initWithContentsOfFile:labelPath];
	functionImage = [[NSImage alloc] initWithContentsOfFile:functionPath];
	
	uiNib = [[NSNib alloc] initWithNibNamed:@"CXDebugUI" bundle:bundle];
}

-(id)initWithDocument:(CXDocument*)document
{
	if (!(self = [super init]))
		return nil;
	
	parent = document;
	js = [[CXJSAdapter alloc] initWithDocument:parent];
	
	return self;
}

-(BOOL)isWindowAlive
{
	return [windowController.window isVisible];
}

-(void)awakeFromNib
{
	[self buildSymbolMenu];
	
	[backForward setEnabled:NO forSegment:0];
	[backForward setEnabled:NO forSegment:1];
	[disassemblyView addObserver:self forKeyPath:@"canGoBack" options:NSKeyValueObservingOptionNew context:nullptr];
	[disassemblyView addObserver:self forKeyPath:@"canGoForward" options:NSKeyValueObservingOptionNew context:nullptr];
	
	stackTrace = [[CXStackTrace alloc] initWithDisassembly:parent.disassembly];
	stackTrace.onFrameSelected = ^(CXStackTrace*, CXStackFrame* frame)
	{
		[self showDisassembly:frame.functionLabel.uniqueName address:frame.absoluteAddress];
	};
	
	stackTraceTable.dataSource = stackTrace;
	stackTraceTable.delegate = stackTrace;
	[self reloadStackTrace];
	
	CXHexFormatter* formatter = [[[CXHexFormatter alloc] init] autorelease];
	NSTableColumn* column = [outline tableColumnWithIdentifier:@"Value"];
	[column.dataCell setFormatter:formatter];
	
	outline.delegate = parent.vm;
	outline.dataSource = parent.vm;
	[self jumpToPC];
}

-(void)instantiate
{
	[uiNib instantiateWithOwner:self topLevelObjects:&topLevelObjects];
	[topLevelObjects retain];
	[self orderFront];
	
	NSURL* executable = parent.executableURL;
	windowController.window.representedURL = executable;
	windowController.window.title = executable.lastPathComponent;;
}

-(void)orderFront
{
	[windowController showWindow:self];
}

-(void)webView:(WebView*)sender didFinishLoadForFrame:(WebFrame *)frame
{
	[[sender windowScriptObject] setValue:js forKey:@"cxdb"];
	[self highlightPC];
}

-(void)dealloc
{
	[topLevelObjects release];
	[js release];
	[super dealloc];
}

#pragma mark -
#pragma mark UI Methods

-(IBAction)goBack:(id)sender
{
	[disassemblyView goBack];
}

-(IBAction)goForward:(id)sender
{
	[disassemblyView goForward];
}

-(IBAction)run:(id)sender
{
	[parent.vm run:sender];
	[outline reloadData];
	[self reloadStackTrace];
	[self jumpToPC];
}

-(IBAction)stepInto:(id)sender
{
	[parent.vm stepInto:sender];
	[outline reloadData];
	[self reloadStackTrace];
	[self jumpToPC];
}

-(IBAction)stepOver:(id)sender
{
	[parent.vm stepOver:sender];
	[outline reloadData];
	[self reloadStackTrace];
	[self jumpToPC];
}

-(IBAction)controlFlow:(id)sender
{
	NSInteger segment = [sender selectedSegment];
	switch (segment)
	{
		case 0: return [self run:sender];
		case 1: return [self stepOver:sender];
		case 2: return [self stepInto:sender];
	}
}

-(IBAction)navigate:(id)sender
{
	NSInteger segment = [sender selectedSegment];
	if (segment == 0)
		[self goBack:sender];
	else
		[self goForward:sender];
}

#pragma mark -

-(void)observeValueForKeyPath:(NSString *)keyPath ofObject:(id)object change:(NSDictionary *)change context:(void *)context
{
	if (object == disassemblyView)
	{
		if ([keyPath isEqualToString:@"canGoBack"])
		{
			BOOL enable = [[change objectForKey:NSKeyValueChangeNewKey] boolValue];
			[backForward setEnabled:enable forSegment:0];
		}
		else if ([keyPath isEqualToString:@"canGoForward"])
		{
			BOOL enable = [[change objectForKey:NSKeyValueChangeNewKey] boolValue];
			[backForward setEnabled:enable forSegment:1];
		}
	}
}

-(NSImage*)fileIcon16x16:(NSString *)path
{
	const NSSize targetSize = NSMakeSize(16, 16);
	NSImage* icon = [[NSWorkspace sharedWorkspace] iconForFile:path];
	NSImage* smallIcon = [[NSImage alloc] initWithSize:targetSize];
	
	NSSize actualSize = icon.size;
	[smallIcon lockFocus];
	[[NSGraphicsContext currentContext] setImageInterpolation:NSImageInterpolationHigh];
	[icon drawInRect:NSMakeRect(0, 0, targetSize.width, targetSize.height) fromRect:NSMakeRect(0, 0, actualSize.width, actualSize.height) operation:NSCompositeCopy fraction:1];
	[smallIcon unlockFocus];
	
	return [smallIcon autorelease];
}

-(void)jumpToPC
{
	CXCodeLabel* label = [[parent.disassembly functionDisassemblyForAddress:parent.vm.pc] objectAtIndex:0];
	[self showDisassembly:label.uniqueName address:parent.vm.pc];
}

-(void)highlightPC
{
	NSArray* trace = stackTrace.stackTrace;
	NSMutableArray* addresses = [NSMutableArray arrayWithCapacity:trace.count];
	for (CXStackFrame* frame in trace)
		[addresses addObject:@(frame.absoluteAddress)];
	
	NSString* scriptArguments = CXJSONEncode(addresses);
	if (NSString* error = parent.vm.lastError)
		scriptArguments = [scriptArguments stringByAppendingFormat:@", %@", CXJSONEncode(error)];
	
	NSString* script = [NSString stringWithFormat:@"HighlightPC(%@)", scriptArguments];
	[[disassemblyView windowScriptObject] evaluateWebScript:script];
}

-(void)showDisassembly:(NSString *)labelUniqueName
{
	NSUInteger documentId = [[CXDocumentController documentController] idOfDocument:parent];
	CXCodeLabel* function = [[parent.disassembly functionDisassemblyForUniqueName:labelUniqueName] objectAtIndex:0];
	NSString* cxdbUrl = [NSString stringWithFormat:@"cxdb://disassembly/%@/%@#%@", @(documentId), function.uniqueName, labelUniqueName];
	NSURL* url = [NSURL URLWithString:cxdbUrl];
	NSURLRequest* request = [NSURLRequest requestWithURL:url];
	[[disassemblyView mainFrame] loadRequest:request];
	[self highlightPC];
}

-(void)showDisassembly:(NSString *)labelUniqueName address:(uint32_t)address
{
	NSUInteger documentId = [[CXDocumentController documentController] idOfDocument:parent];
	CXCodeLabel* function = [[parent.disassembly functionDisassemblyForUniqueName:labelUniqueName] objectAtIndex:0];
	NSString* anchor = [NSString stringWithFormat:@"i%08x", address];
	NSString* cxdbUrl = [NSString stringWithFormat:@"cxdb://disassembly/%@/%@#%@", @(documentId), function.uniqueName, anchor];
	NSURL* url = [NSURL URLWithString:cxdbUrl];
	NSURLRequest* request = [NSURLRequest requestWithURL:url];
	[[disassemblyView mainFrame] loadRequest:request];
	[self highlightPC];
}

-(void)reloadStackTrace
{
	CXRegister* spReg = [parent.vm.gpr objectAtIndex:1];
	CXRegister* lrReg = [parent.vm.spr objectAtIndex:CXVirtualMachineSPRLRIndex];
	uint32_t currentSp = spReg.value.unsignedIntValue;
	uint32_t currentLr = lrReg.value.unsignedIntValue;
	if (currentSp != sp || currentLr != lr)
	{
		sp = currentSp;
		lr = currentLr;
		[stackTrace feedNumericTrace:parent.vm.stackTrace];
	}
	else
	{
		[stackTrace setTopAddress:parent.vm.pc];
	}
	[stackTraceTable reloadData];
}

-(NSMenu*)exportMenuForResolver:(const CFM::SymbolResolver *)resolver
{
	std::vector<std::string> symbols = resolver->CodeSymbolList();
	if (symbols.size() == 0)
		return nil;
	
	NSMenu* exportMenu = [[NSMenu alloc] initWithTitle:@"Exports"];
	for (const std::string& symbol : symbols)
	{
		std::string unmangled = CXUnmangle(symbol);
		NSString* title = [NSString stringWithCString:unmangled.c_str() encoding:NSUTF8StringEncoding];
		if (title == nil)
			title = @"(invalid symbol name)";
		
		NSMenuItem* item = [[[NSMenuItem alloc] initWithTitle:title action:NULL keyEquivalent:@""] autorelease];
		item.image = exportImage;
		[item setEnabled:NO];
		[exportMenu addItem:item];
	}
	
	return [exportMenu autorelease];
}

-(void)buildSymbolMenu
{
	using namespace CFM;
	
	// build the menus with the current resolvers
	NSMenu* resolverMenu = [[[NSMenu alloc] initWithTitle:@"Debugger"] autorelease];
	
	CFM::FragmentManager* cfm;
	Common::Allocator* allocator;
	[[parent.vm fragmentManager] getValue:&cfm];
	[[parent.vm allocator] getValue:&allocator];
	
	for (auto iter = cfm->begin(); iter != cfm->end(); iter++)
	{
		const SymbolResolver* resolver = iter->second;
		std::string name;
		if (const std::string* fullPath = resolver->FilePath())
		{
			std::string::size_type lastSlash = fullPath->find_last_of('/');
			name = fullPath->substr(lastSlash + 1);
		}
		else
		{
			std::string::size_type lastSlash = iter->first.find_last_of('/');
			name = iter->first.substr(lastSlash + 1);
		}
		
		NSString* title = [NSString stringWithCString:name.c_str() encoding:NSUTF8StringEncoding];
		NSMenuItem* resolverItem = [[[NSMenuItem alloc] initWithTitle:title action:NULL keyEquivalent:@""] autorelease];
		
		if (const std::string* path = resolver->FilePath())
		{
			NSString* libraryPath = [NSString stringWithCString:path->c_str() encoding:NSUTF8StringEncoding];
			resolverItem.image = [self fileIcon16x16:libraryPath];
		}
		[resolverMenu addItem:resolverItem];
		
		NSMenu* submenu = [[[NSMenu alloc] initWithTitle:title] autorelease];
		resolverItem.submenu = submenu;
		
		if (NSMenu* exportsMenu = [self exportMenuForResolver:resolver])
		{
			NSMenuItem* exports = [[[NSMenuItem alloc] initWithTitle:@"Exports" action:NULL keyEquivalent:@""] autorelease];
			exports.submenu = exportsMenu;
			[submenu addItem:exports];
		}
		
		// unfortunately we have to do typecasts from this point...
		if (const PEFSymbolResolver* pef = dynamic_cast<const PEFSymbolResolver*>(resolver))
		{
			PPCVM::Disassembly::FancyDisassembler disasm(*allocator);
			const PEF::Container& container = pef->GetContainer();
			NSMutableDictionary* labelToArray = [NSMutableDictionary dictionary];
			NSMutableDictionary* addressToLabel = [NSMutableDictionary dictionary];
			for (size_t i = 0; i < container.size(); i++)
			{
				const PEF::InstantiableSection& section = container.GetSection(i);
				PEF::SectionType type = section.GetSectionType();
				if (type == PEF::SectionType::Code || type == PEF::SectionType::ExecutableData)
				{
					CXObjCDisassemblyWriter writer(i);
					disasm.Disassemble(container, writer);
					NSArray* result = writer.GetDisassembly();
					if (result.count == 0)
						continue;
					
					NSString* menuTitle = [NSString stringWithCString:section.Name.c_str() encoding:NSUTF8StringEncoding];
					NSMenuItem* sectionItem = [[[NSMenuItem alloc] initWithTitle:menuTitle action:NULL keyEquivalent:@""] autorelease];
					NSMutableArray* currentArray = [NSMutableArray array];
					NSMenu* labelMenu = [[[NSMenu alloc] initWithTitle:menuTitle] autorelease];
					
					for (CXCodeLabel* codeLabel in result)
					{
						NSArray* instructions = codeLabel.instructions;
						if (instructions.count == 0)
							continue;
						
						uint32_t address = codeLabel.address;
						if (codeLabel.isFunction)
							currentArray = [NSMutableArray array];
						
						[currentArray addObject:codeLabel];
						[labelToArray setObject:currentArray forKey:codeLabel.uniqueName];
						[addressToLabel setObject:codeLabel.uniqueName forKey:@(address)];
						
						NSString* uniqueName = codeLabel.uniqueName;
						NSString* title = [parent.disassembly displayNameForUniqueName:uniqueName];
						NSMenuItem* labelMenuItem = [[NSMenuItem alloc] initWithTitle:title action:NULL keyEquivalent:@""];
						labelMenuItem.representedObject = codeLabel;
						
						labelMenuItem.image = codeLabel.isFunction ? functionImage : labelImage;
						[labelMenu addItem:labelMenuItem];
					}
					sectionItem.submenu = labelMenu;
					[submenu addItem:sectionItem];
				}
			}
		}
	}
	
	navBar.selectionChanged = ^(CXNavBar* bar, NSMenuItem* selection) {
		CXCodeLabel* label = [selection representedObject];
		[self showDisassembly:label.uniqueName];
	};
	navBar.menu = resolverMenu;
}

@end
