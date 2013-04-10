//
// CXILApplication.h
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

#import <Cocoa/Cocoa.h>

@interface CXILApplication : NSApplication

// Reacting to stuff
-(void)sendEvent:(NSEvent *)theEvent;
-(void)receiveNotification:(NSNotification*)notification;

// IPC messages implementation
-(void)processIPCMessage;
-(void)readInto:(void*)into size:(size_t)size;
-(void)writeFrom:(const void*)from size:(size_t)size;

// get a copy of the next EventRecord matching the EventMask without altering the queue
-(void)peekNextEvent;

// discard the next EventRecord matching the EventMask; does nothing if there's no such event
-(void)discardNextEvent;

// makes a beep
-(void)beep;

// creates a window
-(void)createWindow;

@end
