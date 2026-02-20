#pragma once
#include <Windows.h>
#include <TlHelp32.h>
#include <iostream>
#include "Values.h"
#include "skStr.h"

typedef struct _NULL_MEMORY
{
	void*		buff_address;
	ULONGLONG	size;
	UINT_PTR	address;
	int			check;
	BOOLEAN		write, read, req_base, antidebug;
	ULONG		pid;
	void*		output;
	const char* module_name;
	ULONG64		base_address;
}NULL_MEMORY;

template<typename ... Arg>
uint64_t call_hook(const Arg ... args)
{
	//void* hooked_func = GetProcAddress(val.hmodule, skCrypt("NtQueryCompositionSurfaceStatistics")); //NtQueryCompositionSurfaceStatistics  NtSetCompositionSurfaceStatistics  NtGdiDdCanCreateSurface

	auto func = static_cast<uint64_t(_fastcall*)(Arg...)>(val.Hooked_function_PTR);

	return func(args ...);
}




struct HandleDisposer
{
	using pointer = HANDLE;
	void operator()(HANDLE handle) const
	{
		if (handle != NULL || handle != INVALID_HANDLE_VALUE)
		{
			CloseHandle(handle);
		}
	}
};

using unique_handle = std::unique_ptr<HANDLE, HandleDisposer>;

std::uint32_t get_process_id(std::string_view process_name)
{
	PROCESSENTRY32 processentry;
	const unique_handle snapshot_handle(CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL));

	if (snapshot_handle.get() == INVALID_HANDLE_VALUE)
		return NULL;

	processentry.dwSize = sizeof(MODULEENTRY32);

	while (Process32Next(snapshot_handle.get(), &processentry) == TRUE)
	{
		if (process_name.compare(processentry.szExeFile) == NULL)
		{
			return processentry.th32ProcessID;
		}
	}
	return processentry.th32ProcessID;
}


static ULONG64 get_module_base_address(const char* module_name)
{
	NULL_MEMORY instructions = { 0 };
	instructions.check = 76542454;
	instructions.pid = val.Process;
	instructions.req_base = TRUE;
	instructions.read = FALSE;
	instructions.write = FALSE;
	instructions.module_name = module_name;

	//call_hook(1900, &instructions);
	call_hook(40521, 176, &instructions);

	ULONG64 base = NULL;
	base = instructions.base_address;
	return base;

}

static ULONG64 get_module_size(const char* module_name)
{
	NULL_MEMORY instructions = { 0 };
	instructions.check = 76542454;
	instructions.pid = val.Process;
	instructions.req_base = TRUE;
	instructions.read = FALSE;
	instructions.write = FALSE;
	instructions.module_name = module_name;

	//call_hook(1900, &instructions);
	call_hook(40521, 176, &instructions);

	ULONG64 size = NULL;
	size = instructions.size;
	return size;
}

template <class T>
T ReadPM(UINT_PTR read_address)
{
	T response{};
	NULL_MEMORY instructions;
	instructions.check = 76542454;
	instructions.pid = val.Process;
	instructions.size = sizeof(T);
	instructions.address = read_address;
	instructions.read = TRUE;
	instructions.write = FALSE;
	instructions.req_base = FALSE;
	instructions.output = &response;
	//call_hook(&instructions);
	call_hook(40521, 176, &instructions);

	return response;
}
bool write_memory(UINT_PTR write_address, UINT_PTR source_address, SIZE_T write_size)
{
	NULL_MEMORY instructions;
	instructions.check = 76542454;
	instructions.address = write_address;
	instructions.pid = val.Process;
	instructions.write = TRUE;
	instructions.read = FALSE;
	instructions.req_base = FALSE;
	instructions.buff_address = (void*)source_address;
	instructions.size = write_size;
	//call_hook(&instructions);
	//call_hook(40521, &instructions);
	call_hook(40521, 176, &instructions);

	return true;

}

template<typename S>
bool write(UINT_PTR write_address, const S& value)
{
	return write_memory(write_address, (UINT_PTR)&value, sizeof(S));
}

bool being_debugged_kernel()
{
	NULL_MEMORY instructions;
	instructions.check = 76542454;
	instructions.address = 1;
	instructions.pid = GetCurrentProcessId();
	instructions.write = FALSE;
	instructions.read = FALSE;
	instructions.req_base = FALSE;
	instructions.antidebug = TRUE;
	instructions.buff_address = 0;
	instructions.size = 0;

	call_hook(40521, 176, &instructions);

	if (instructions.address == 101)
		return true;
	else
		return false;
}

bool being_debugged_kernel_thread()
{
	NULL_MEMORY instructions;
	instructions.check = 76542454;
	instructions.address = 1;
	instructions.pid = GetCurrentThreadId();
	instructions.write = FALSE;
	instructions.read = FALSE;
	instructions.req_base = FALSE;
	instructions.antidebug = TRUE;
	instructions.buff_address = 0;
	instructions.size = 0;

	call_hook(40521, 176, &instructions);

	if (instructions.address == 101)
		return true;
	else
		return false;
}