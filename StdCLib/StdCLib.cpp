//
//  StdCLib.c
//  Classix
//
//  Created by Félix on 2012-11-11.
//  Copyright (c) 2012 Félix. All rights reserved.
//

#include <cstdlib>
#include <cfloat>
#include <cassert>
#include <map>
#include <string>

#include <dlfcn.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include "BigEndian.h"
#include "StdCLib.h"
#include "StdCLibFunctions.h"

const int StdCLib_NFILE = 40;

static FILE* fdup(FILE* fp)
{
	static const char* modes[] = {
		[O_RDONLY] = "r",
		[O_WRONLY] = "a",
		[O_RDWR] = "r+"
	};
	
	int fd = fileno(fp);
	int accmode = fcntl(fd, F_GETFL) & O_ACCMODE;
	return fdopen(dup(fd), modes[accmode]);
}

union StdCLibPPCFILE
{
	struct
	{
		int32_t _cnt;
		uint32_t _ptr;
		uint32_t _base;
		uint32_t _end;
		uint16_t _size;
		uint16_t _flag;
		uint16_t _file;
	};
	
	struct
	{
		uint32_t : 32;
		FILE* fptr;
	};
};

struct StdCLibGlobals
{
#pragma mark Globals
	Common::UInt32 __C_phase;
	Common::UInt32 __loc;
	Common::UInt32 __NubAt3;
	Common::UInt32 __p_CType;
	Common::UInt32 __SigEnv;
	uint32_t __target_for_exit[64];
	Common::UInt32 __yd;
	Common::UInt32 _CategoryLoc;
	Common::Real64 _DBL_EPSILON;
	Common::Real64 _DBL_MAX;
	Common::Real64 _DBL_MIN;
	Common::UInt32 _exit_status;
	Common::Real64 _FLT_EPSILON;
	Common::Real64 _FLT_MAX;
	Common::Real64 _FLT_MIN;
	Common::UInt32 _IntEnv;
	StdCLibPPCFILE _iob[StdCLib_NFILE];
	Common::UInt32 _lastbuf;
	Common::Real64 _LDBL_EPSILON;
	Common::Real64 _LDBL_MIN;
	Common::Real64 _LDBL_MAX;
	Common::UInt32 _PublicTimeInfo;
	Common::UInt32 _StdDevs;
	Common::UInt32 errno_;
	Common::UInt32 MacOSErr;
	Common::UInt32 MoneyData;
	Common::UInt32 NoMoreDebugStr;
	Common::UInt32 NumericData;
	Common::UInt32 StandAlone;
	Common::UInt32 TimeData;
	
#pragma mark Housekeeping
	Common::IAllocator* allocator;
	
	static std::map<std::string, off_t> FieldOffsets;
	
	StdCLibGlobals(Common::IAllocator* allocator)
	: allocator(allocator)
	{
		memset(__target_for_exit, 0, sizeof __target_for_exit);
		
		_iob[0].fptr = fdup(stdin);
		_iob[1].fptr = fdup(stdout);
		_iob[2].fptr = fdup(stderr);
		for (int i = 3; i < StdCLib_NFILE; i++)
			_iob[i].fptr = nullptr;
		
		_DBL_EPSILON = DBL_EPSILON;
		_DBL_MIN = DBL_MIN;
		_DBL_MAX = DBL_MAX;
		
		_FLT_EPSILON = FLT_EPSILON;
		_FLT_MIN = FLT_MIN;
		_FLT_MAX = FLT_MAX;
		
		_LDBL_EPSILON = DBL_EPSILON;
		_DBL_MIN = DBL_MIN;
		_LDBL_MAX = DBL_MAX;
	}
};

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winvalid-offsetof"

std::map<std::string, off_t> StdCLibGlobals::FieldOffsets
{
	std::make_pair("__C_phase", offsetof(StdCLibGlobals, __C_phase)),
	std::make_pair("__loc", offsetof(StdCLibGlobals, __loc)),
	std::make_pair("__NubAt3", offsetof(StdCLibGlobals, __NubAt3)),
	std::make_pair("__p_CType", offsetof(StdCLibGlobals, __p_CType)),
	std::make_pair("__SigEnv", offsetof(StdCLibGlobals, __SigEnv)),
	std::make_pair("__target_for_exit", offsetof(StdCLibGlobals, __target_for_exit)),
	std::make_pair("__yd", offsetof(StdCLibGlobals, __yd)),
	std::make_pair("_CategoryLoc", offsetof(StdCLibGlobals, _CategoryLoc)),
	std::make_pair("_DBL_EPSILON", offsetof(StdCLibGlobals, _DBL_EPSILON)),
	std::make_pair("_DBL_MAX", offsetof(StdCLibGlobals, _DBL_MAX)),
	std::make_pair("_DBL_MIN", offsetof(StdCLibGlobals, _DBL_MIN)),
	std::make_pair("_exit_status", offsetof(StdCLibGlobals, _exit_status)),
	std::make_pair("_FLT_EPSILON", offsetof(StdCLibGlobals, _FLT_EPSILON)),
	std::make_pair("_FLT_MAX", offsetof(StdCLibGlobals, _FLT_MAX)),
	std::make_pair("_FLT_MIN", offsetof(StdCLibGlobals, _FLT_MIN)),
	std::make_pair("_IntEnv", offsetof(StdCLibGlobals, _IntEnv)),
	std::make_pair("_iob", offsetof(StdCLibGlobals, _iob)),
	std::make_pair("_lastbuf", offsetof(StdCLibGlobals, _lastbuf)),
	std::make_pair("_LDBL_EPSILON", offsetof(StdCLibGlobals, _LDBL_EPSILON)),
	std::make_pair("_LDBL_MIN", offsetof(StdCLibGlobals, _LDBL_MIN)),
	std::make_pair("_LDBL_MAX", offsetof(StdCLibGlobals, _LDBL_MAX)),
	std::make_pair("_PublicTimeInfo", offsetof(StdCLibGlobals, _PublicTimeInfo)),
	std::make_pair("_StdDevs", offsetof(StdCLibGlobals, _StdDevs)),
	std::make_pair("errno", offsetof(StdCLibGlobals, errno_)),
	std::make_pair("MacOSErr", offsetof(StdCLibGlobals, MacOSErr)),
	std::make_pair("MoneyData", offsetof(StdCLibGlobals, MoneyData)),
	std::make_pair("NoMoreDebugStr", offsetof(StdCLibGlobals, NoMoreDebugStr)),
	std::make_pair("NumericData", offsetof(StdCLibGlobals, NumericData)),
	std::make_pair("StandAlone", offsetof(StdCLibGlobals, StandAlone)),
	std::make_pair("TimeData", offsetof(StdCLibGlobals, TimeData)),
};

#pragma clang diagnostics pop

#pragma mark -
#pragma mark Lifecycle
StdCLibGlobals* LibraryInit(Common::IAllocator* allocator)
{
	return allocator->Allocate<StdCLibGlobals>("StdCLib Globals", allocator);
}

SymbolType LibraryLookup(StdCLibGlobals* globals, const char* name, void** result)
{
	char functionName[36] = "StdCLib_";
	char* end = stpncpy(functionName + 8, name, 27);
	assert(*end == '\0' && "symbol name is too long");
	
	if (void* symbol = dlsym(RTLD_SELF, functionName))
	{
		*result = symbol;
		return CodeSymbol;
	}
	
	auto iter = StdCLibGlobals::FieldOffsets.find(name);
	if (iter != StdCLibGlobals::FieldOffsets.end())
	{
		*result = reinterpret_cast<uint8_t*>(globals) + iter->second;
		return DataSymbol;
	}
	
	*result = nullptr;
	return SymbolNotFound;
}

void LibraryFinit(StdCLibGlobals* globals)
{
	for (int i = 0; i < StdCLib_NFILE; i++)
	{
		FILE* file = globals->_iob[i].fptr;
		if (file != nullptr)
			fclose(file);
	}
	globals->allocator->Deallocate(globals);
}

#pragma mark -
#pragma mark Implementation

#define ToPointer	globals->allocator->ToPointer
#define ToIntPtr	globals->allocator->ToIntPtr

static FILE* MakeFilePtr(StdCLibGlobals* globals, intptr_t ptr)
{
	for (int i = 0; i < StdCLib_NFILE; i++)
	{
		intptr_t thisIOB = ToIntPtr(&globals->_iob[i]);
		if (ptr == thisIOB)
			return globals->_iob[i].fptr;
	}
	return nullptr;
}

void StdCLib___abort(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib___assertprint(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib___DebugMallocHeap(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib___GetTrapType(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib___growFileTable(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib___NumToolboxTraps(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib___RestoreInitialCFragWorld(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib___RevertCFragWorld(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib___setjmp(StdCLibGlobals* globals, MachineState* state)
{
	// since the client is not really supposed to read from jmpBuf values, we
	// shouldn't have to worry about endianness
	uint32_t* jmpBuf = ToPointer<uint32_t>(state->r3);
	jmpBuf[0] = state->lr;
	jmpBuf[1] = MachineStateGetCR(state);
	jmpBuf[2] = state->r1;
	jmpBuf[3] = state->r2;
	jmpBuf[4] = 0;
	memcpy(jmpBuf + 5, state->gpr + 13, 19 * sizeof(uint32_t));
	memcpy(jmpBuf + 24, state->fpr + 14, 18 * sizeof(double));
	jmpBuf[61] = 0;
	jmpBuf[62] = 0;
	
	// ??? there are two fields left over
	jmpBuf[63] = 0;
	jmpBuf[64] = 0;
	
	state->r3 = 0;
	globals->errno_ = 0;
}

void StdCLib___vec_longjmp(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib___vec_setjmp(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib__addDevHandler(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib__badPtr(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib__Bogus(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib__BreakPoint(StdCLibGlobals* globals, MachineState* state)
{
	__asm__ ("int $3");
}

void StdCLib__bufsync(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib__c2pstrcpy(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib__coClose(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib__coExit(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib__coFAccess(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib__coIoctl(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib__coRead(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib__coreIOExit(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib__coWrite(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib__cvt(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib__DoExitProcs(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib__doprnt(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib__doscan(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib__exit(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib__faccess(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib__filbuf(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib__findiop(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib__flsbuf(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib__fsClose(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib__fsFAccess(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib__fsIoctl(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib__fsRead(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib__FSSpec2Path(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib__fsWrite(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib__GetAliasInfo(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib__getDevHandler(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib__getIOPort(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib__memchr(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib__memcpy(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib__ResolveFileAlias(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib__rmemcpy(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib__RTExit(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib__RTInit(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib__SA_DeletePtr(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib__SA_GetPID(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib__SA_SetPtrSize(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib__syClose(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib__syFAccess(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib__syIoctl(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib__syRead(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib__syWrite(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib__uerror(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib__wrtchk(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib__xflsbuf(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_abort(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_abs(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_access(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_asctime(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_atexit(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_atof(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_atoi(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_atol(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_atoll(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_binhex(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_bsearch(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_calloc(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_clearerr(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_clock(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_close(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_ConvertTheString(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_creat(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_ctime(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_difftime(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_div(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_dup(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_ecvt(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_exit(StdCLibGlobals* globals, MachineState* state)
{
	// TODO longjmp to __target_for_exit
	// for now we'll just kill the host (which is really, really bad)
	exit(state->r3);
}

void StdCLib_faccess(StdCLibGlobals* globals, MachineState* state)
{
	fprintf(stderr, "Using 'faccess' with mode %08x\n", state->r4);
	state->r3 = 0;
	globals->errno_ = 0;
}

void StdCLib_fclose(StdCLibGlobals* globals, MachineState* state)
{
	for (int i = 0; i < StdCLib_NFILE; i++)
	{
		auto& ioBuffer = globals->_iob[i];
		intptr_t ioBufferAddress = ToIntPtr(&ioBuffer);
		if (ioBufferAddress == state->r3)
		{
			state->r3 = fclose(ioBuffer.fptr);
			ioBuffer.fptr = nullptr;
			return;
		}
	}
	
	globals->errno_ = EBADF;
	state->r3 = EOF;
}

void StdCLib_fcntl(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_fcvt(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_fdopen(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_feof(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_ferror(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_fflush(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_fgetc(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_fgetpos(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_fgets(StdCLibGlobals* globals, MachineState* state)
{
	char* buffer = ToPointer<char>(state->r3);
	int32_t size = state->r4;
	FILE* fptr = MakeFilePtr(globals, state->r5);
	
	char* result = fgets(buffer, size, fptr);
	state->r3 = ToIntPtr(result);
	globals->errno_ = errno;
}

void StdCLib_fopen(StdCLibGlobals* globals, MachineState* state)
{
	const char* filename = ToPointer<const char>(state->r3);
	const char* mode = ToPointer<const char>(state->r4);
	
	for (int i = 0; i < StdCLib_NFILE; i++)
	{
		auto& ioBuffer = globals->_iob[i];
		if (ioBuffer.fptr == nullptr)
		{
			ioBuffer.fptr = fopen(filename, mode);
			state->r3 = ToIntPtr(&ioBuffer);
			globals->errno_ = errno;
			return;
		}
	}
	
	state->r3 = 0;
	globals->errno_ = EMFILE;
}

void StdCLib_fprintf(StdCLibGlobals* globals, MachineState* state)
{
	FILE* fptr = MakeFilePtr(globals, state->r3);
	const char* formatString = ToPointer<const char>(state->r4);
	// gah, let's just print the format string for now.
	state->r3 = fputs(formatString, fptr);
	globals->errno_ = errno;
}

void StdCLib_fputc(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_fputs(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_fread(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_free(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_freopen(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_fscanf(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_fseek(StdCLibGlobals* globals, MachineState* state)
{
	FILE* fptr = MakeFilePtr(globals, state->r3);
	int32_t offset = state->r4;
	int whence = state->r5;
	state->r3 = fseek(fptr, offset, whence);
	globals->errno_ = errno;
}

void StdCLib_fsetfileinfo(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_fsetpos(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_FSMakeFSSpec_Long(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_FSp_creat(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_FSp_faccess(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_FSp_fopen(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_FSp_freopen(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_FSp_fsetfileinfo(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_FSp_open(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_FSp_remove(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_FSp_rename(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_FSp_unlink(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_FSSpec2Path_Long(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_ftell(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_fwrite(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_getc(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_getchar(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_getenv(StdCLibGlobals* globals, MachineState* state)
{
#ifdef __LP64__
# error This will break in 64 bits because getenv() will return a value out of the address space
#endif
	
	const char* name = ToPointer<const char>(state->r3);
	const char* env = getenv(name);
	state->r3 = ToIntPtr(env);
	globals->errno_ = errno;
}

void StdCLib_getIDstring(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_getpid(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_gets(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_getw(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_gmtime(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_IEResolvePath(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_ioctl(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_isalnum(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_isalpha(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_isascii(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_iscntrl(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_isdigit(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_isgraph(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_islower(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_isprint(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_ispunct(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_isspace(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_isupper(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_isxdigit(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_labs(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_ldiv(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_llabs(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_lldiv(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_localeconv(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_localtime(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_longjmp(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_lseek(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_MakeResolvedFSSpec(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_MakeResolvedFSSpec_Long(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_MakeResolvedPath(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_MakeResolvedPath_Long(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_MakeTheLocaleString(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_malloc(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_mblen(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_mbstowcs(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_mbtowc(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_memccpy(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_memchr(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_memcmp(StdCLibGlobals* globals, MachineState* state)
{
	const void* s1 = ToPointer<const void>(state->r3);
	const void* s2 = ToPointer<const void>(state->r4);
	size_t size = state->r5;
	state->r3 = memcmp(s1, s2, size);
}

void StdCLib_memcpy(StdCLibGlobals* globals, MachineState* state)
{
	void* s1 = ToPointer<void>(state->r3);
	const void* s2 = ToPointer<const void>(state->r4);
	size_t size = state->r5;
	state->r3 = ToIntPtr(memcpy(s1, s2, size));
}

void StdCLib_memmove(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_memset(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_mktemp(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_mktime(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_open(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_ParseTheLocaleString(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_perror(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_PLpos(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_PLstrcat(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_PLstrchr(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_PLstrcmp(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_PLstrcpy(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_PLstrlen(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_PLstrncat(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_PLstrncmp(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_PLstrncpy(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_PLstrpbrk(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_PLstrrchr(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_PLstrspn(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_PLstrstr(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_printf(StdCLibGlobals* globals, MachineState* state)
{
	const char* formatString = ToPointer<const char>(state->r3);
	// gah, let's just print the format string for now.
	state->r3 = puts(formatString);
	globals->errno_ = errno;
}

void StdCLib_putc(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_putchar(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_puts(StdCLibGlobals* globals, MachineState* state)
{
	const char* address = ToPointer<const char>(state->r3);
	state->r3 = puts(address);
	globals->errno_ = errno;
}

void StdCLib_putw(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_qsort(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_raise(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_rand(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_read(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_realloc(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_remove(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_rename(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_ResolveFolderAliases(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_ResolveFolderAliases_Long(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_ResolvePath(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_ResolvePath_Long(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_rewind(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_scanf(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_setbuf(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_setenv(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_setlocale(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_setvbuf(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_signal(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_sprintf(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_srand(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_sscanf(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_strcat(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_strchr(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_strcmp(StdCLibGlobals* globals, MachineState* state)
{
	const char* s1 = ToPointer<const char>(state->r3);
	const char* s2 = ToPointer<const char>(state->r4);
	state->r3 = strcmp(s1, s2);
}

void StdCLib_strcoll(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_strcpy(StdCLibGlobals* globals, MachineState* state)
{
	char* s1 = ToPointer<char>(state->r3);
	const char* s2 = ToPointer<const char>(state->r4);
	state->r3 = ToIntPtr(strcpy(s1, s2));
}

void StdCLib_strcspn(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_strerror(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_strftime(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_strlen(StdCLibGlobals* globals, MachineState* state)
{
	char* s = ToPointer<char>(state->r3);
	state->r3 = strlen(s);
}

void StdCLib_strncat(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_strncmp(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_strncpy(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_strpbrk(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_strrchr(StdCLibGlobals* globals, MachineState* state)
{
	const char* s = ToPointer<const char>(state->r3);
	int c = state->r4;
	char* result = strrchr(s, c);
	state->r3 = ToIntPtr(result);
}

void StdCLib_strspn(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_strstr(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_strtod(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_strtok(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_strtol(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_strtoll(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_strtoul(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_strtoull(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_strxfrm(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_system(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_time(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_tmpfile(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_tmpnam(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_toascii(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_tolower(StdCLibGlobals* globals, MachineState* state)
{
	state->r3 = tolower(state->r3);
}

void StdCLib_toupper(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_TrapAvailable(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_ungetc(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_unlink(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_vec_calloc(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_vec_free(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_vec_malloc(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_vec_realloc(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_vfprintf(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_vprintf(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_vsprintf(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_wcstombs(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_wctomb(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}

void StdCLib_write(StdCLibGlobals* globals, MachineState* state)
{
	abort();
}