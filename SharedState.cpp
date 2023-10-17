#include "pch.h"
#include "SharedState.h"
#include "LoggingHelper.h"

using namespace NppShell::Helpers;

extern LoggingHelper g_loggingHelper;

SharedState::SharedState()
{
    // Create or open the shared memory mapped file
    hFileMapping = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(int), L"Local\\BaseNppExplorerCommandHandlerSharedStateMemory");
    if (hFileMapping == NULL)
    {
        g_loggingHelper.LogMessage(L"SharedState::ctor", L"Failed to create or open shared memory mapped file");
        return;
    }

    // Create a mutex to synchronize access to the shared memory
    hMutex = CreateMutex(NULL, FALSE, L"Local\\BaseNppExplorerCommandHandlerSharedStateMutex");
    if (hMutex == NULL)
    {
        g_loggingHelper.LogMessage(L"SharedState::ctor", L"Failed to create mutex");
        CloseHandle(hFileMapping);
        return;
    }

    // Map the shared memory into the current process's address space
    pState = (CounterState*)MapViewOfFile(hFileMapping, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(CounterState));
    if (pState == NULL)
    {
        MessageBox(NULL, L"Failed to map shared memory", L"SharedState", MB_OK | MB_ICONERROR);
        CloseHandle(hMutex);
        CloseHandle(hFileMapping);
        return;
    }
}

SharedState::~SharedState()
{
    // Unmap the shared memory from the current process's address space
    UnmapViewOfFile(pState);

    // Close the mutex and the shared memory mapped file
    CloseHandle(hMutex);
    CloseHandle(hFileMapping);
}

CounterState SharedState::GetState(const wstring caller) const
{
    g_loggingHelper.LogMessage(L"SharedState::GetState", L"Get shared state by caller: " + caller);
    return *pState;
}

void SharedState::SetState(const wstring caller, const CounterState state)
{
    WaitForSingleObject(hMutex, INFINITE);
    *pState = state;
    g_loggingHelper.LogMessage(L"SharedState::SetState", L"Set shared state to: " + to_wstring(state) + L" by caller: " + caller);
    ReleaseMutex(hMutex);
}