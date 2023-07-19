#include "pch.h"
#include "SharedCounter.h"
#include "LoggingHelper.h"

using namespace NppShell::Helpers;

extern LoggingHelper g_loggingHelper;

SharedCounter::SharedCounter()
{
    // Create or open the shared memory mapped file
    hFileMapping = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(int), L"Local\\BaseNppExplorerCommandHandlerSharedMemory");
    if (hFileMapping == NULL)
    {
        g_loggingHelper.LogMessage(L"SharedCounter::ctor", L"Failed to create or open shared memory mapped file");
        return;
    }

    // Create a mutex to synchronize access to the shared memory
    hMutex = CreateMutex(NULL, FALSE, L"Local\\BaseNppExplorerCommandHandlerSharedMutex");
    if (hMutex == NULL)
    {
        g_loggingHelper.LogMessage(L"SharedCounter::ctor", L"Failed to create mutex");
        CloseHandle(hFileMapping);
        return;
    }

    // Map the shared memory into the current process's address space
    pCounter = (int*)MapViewOfFile(hFileMapping, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(int));
    if (pCounter == NULL)
    {
        g_loggingHelper.LogMessage(L"SharedCounter::ctor", L"Failed to map shared memory");
        CloseHandle(hMutex);
        CloseHandle(hFileMapping);
        return;
    }

    // Increment the shared counter
    WaitForSingleObject(hMutex, INFINITE);
    *pCounter += 1;
    localValue = *pCounter;
    ReleaseMutex(hMutex);
}

SharedCounter::~SharedCounter()
{
    // Decrement the shared counter
    WaitForSingleObject(hMutex, INFINITE);
    *pCounter -= 1;
    ReleaseMutex(hMutex);

    // Unmap the shared memory from the current process's address space
    UnmapViewOfFile(pCounter);

    // Close the mutex and the shared memory mapped file
    CloseHandle(hMutex);
    CloseHandle(hFileMapping);
}

int SharedCounter::GetValue() const
{
    return localValue;
}