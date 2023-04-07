#include "pch.h"
#include "resource.h"
#include "BaseNppExplorerCommandHandler.h"

#include "PathHelper.h"

using namespace NppShell::CommandHandlers;
using namespace NppShell::Helpers;

extern HMODULE thisModule;

BaseNppExplorerCommandHandler::BaseNppExplorerCommandHandler()
{
    counter = make_unique<SharedCounter>();
}

const wstring BaseNppExplorerCommandHandler::GetNppExecutableFullPath()
{
    const wstring path = GetApplicationPath();
    const wstring fileName = L"\\notepad++.exe";

    return path + fileName;
}

const wstring BaseNppExplorerCommandHandler::Title()
{
    // A buffer size of 1024 should be enough to hold the for all languages.
    constexpr int bufferSize = 1024;

    // Buffer to store the string resource into.
    WCHAR buffer[bufferSize];

    // Load the string from the resource matching the current language.
    LoadStringW(thisModule, IDS_EDIT_WITH_NOTEPADPLUSPLUS, buffer, bufferSize);

    // Finally we convert the buffer into a wstring that we can return.
    return wstring(buffer);
}

const wstring BaseNppExplorerCommandHandler::Icon()
{
    const wstring fileName = GetNppExecutableFullPath();

    return fileName;
}

const wstring BaseNppExplorerCommandHandler::GetCommandLine(const wstring& itemName)
{
    const wstring fileName = GetNppExecutableFullPath();
    const wstring parameters = L"\"" + itemName + L"\"";

    return L"\"" + fileName + L"\" " + parameters;
}

IFACEMETHODIMP BaseNppExplorerCommandHandler::Invoke(IShellItemArray* psiItemArray, IBindCtx* pbc) noexcept try
{
    UNREFERENCED_PARAMETER(pbc);

    if (!psiItemArray)
    {
        return S_OK;
    }

    DWORD count;
    RETURN_IF_FAILED(psiItemArray->GetCount(&count));

    IShellItem* psi = nullptr;
    LPWSTR itemName;

    for (DWORD i = 0; i < count; ++i)
    {
        psiItemArray->GetItemAt(i, &psi);
        RETURN_IF_FAILED(psi->GetDisplayName(SIGDN_FILESYSPATH, &itemName));

        const wstring applicationName = GetNppExecutableFullPath();
        const wstring commandLine = GetCommandLine(itemName);

        // Cleanup itemName, since we are done with it.
        if (itemName)
        {
            CoTaskMemFree(itemName);
        }

        // Release the IShellItem pointer, since we are done with it as well.
        psi->Release();

        STARTUPINFO si;
        PROCESS_INFORMATION pi;

        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        ZeroMemory(&pi, sizeof(pi));

        wchar_t* application = (LPWSTR)applicationName.c_str();
        wchar_t* command = (LPWSTR)commandLine.c_str();

        if (!CreateProcessW(application, command, nullptr, nullptr, false, CREATE_NEW_PROCESS_GROUP, nullptr, nullptr, &si, &pi))
        {
            return S_OK;
        }

        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }

    return S_OK;
}
CATCH_RETURN();

const EXPCMDSTATE BaseNppExplorerCommandHandler::State(IShellItemArray* psiItemArray)
{
    UNREFERENCED_PARAMETER(psiItemArray);

    throw L"State must be overridden in all implementations";
}