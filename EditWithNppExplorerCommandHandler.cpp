#include "pch.h"
#include "EditWithNppExplorerCommandHandler.h"

#include "Helpers.h"

using namespace NppShell::CommandHandlers;
using namespace NppShell::Helpers;

const wstring EditWithNppExplorerCommandHandler::GetNppExecutableFullPath()
{
    const wstring path = GetApplicationPath();
    const wstring fileName = L"\\notepad++.exe";

    return path + fileName;
}

const wstring EditWithNppExplorerCommandHandler::Title()
{
    return L"Edit with Notepad++";
}

const wstring EditWithNppExplorerCommandHandler::Icon()
{
    const wstring fileName = GetNppExecutableFullPath();

    return fileName;
}

const wstring EditWithNppExplorerCommandHandler::GetCommandLine(const wstring& itemName)
{
    const wstring fileName = GetNppExecutableFullPath();
    const wstring parameters = L"\"" + itemName + L"\"";

    return L"\"" + fileName + L"\" " + parameters;
}

IFACEMETHODIMP EditWithNppExplorerCommandHandler::Invoke(IShellItemArray* psiItemArray, IBindCtx* pbc) noexcept try
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