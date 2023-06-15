#include "pch.h"
#include "resource.h"
#include "BaseNppExplorerCommandHandler.h"

#include "PathHelper.h"
#include "ThreadUILanguageChanger.h"

using namespace NppShell::CommandHandlers;
using namespace NppShell::Helpers;

extern HMODULE g_module;

BaseNppExplorerCommandHandler::BaseNppExplorerCommandHandler()
{
    counter = make_unique<SharedCounter>();
    state = make_unique<SharedState>();
}

const wstring BaseNppExplorerCommandHandler::GetNppExecutableFullPath()
{
    const wstring path = GetApplicationPath();
    const wstring fileName = L"\\notepad++.exe";

    return path + fileName;
}

bool ReadLanguageOverrideFile(wstring& content)
{
    const wstring fileFullName = GetContextMenuPath() + L"\\overrideMenuEntryLanguage.txt";

    // Open the file for reading.
    HANDLE file = CreateFileW(fileFullName.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE)
    {
        // The file does not exist, so no override.
        return false;
    }

    // Allocate a buffer for the file content.
    // Since we only need 6 chars ("xx-XX" followed by a null byte, maybe double if unicode encoded), so we use 32 bytes, to be on the safe side.
    const DWORD bufferSize = 32;
    vector<BYTE> buffer(bufferSize);

    // Read the first 32 chars into the buffer.
    DWORD bytesRead;
    if (!ReadFile(file, buffer.data(), bufferSize - 1, &bytesRead, NULL))
    {
        CloseHandle(file);

        return false;
    }

    // Close the file handle
    CloseHandle(file);

    // Null-terminate the buffer
    buffer[bytesRead] = '\0';

    // Convert the buffer to a wstring
    content = wstring(buffer.begin(), buffer.begin() + bytesRead);

    if (content == L"")
    {
        // The file exists, but is empty, so we default to "en-US";
        content = L"en-US";
    }

    return true;
}

const wstring BaseNppExplorerCommandHandler::Title()
{
    // A buffer size of 1024 should be enough to hold the for all languages.
    constexpr int bufferSize = 1024;

    // Buffer to store the string resource into.
    WCHAR buffer[bufferSize];

    // Read the override file, and if success, we change the language we use.
    wstring overrideLanguage;
    if (ReadLanguageOverrideFile(overrideLanguage))
    {
        // Change the language
        ThreadUILanguageChanger languageChanger(overrideLanguage);

        // Load the string from the resource matching the override language.
        LoadStringW(g_module, IDS_EDIT_WITH_NOTEPADPLUSPLUS, buffer, bufferSize);
    }
    else
    {
        // Load the string from the resource matching the current language.
        LoadStringW(g_module, IDS_EDIT_WITH_NOTEPADPLUSPLUS, buffer, bufferSize);
    }

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
    LPWSTR file2OpenPath;
    
    wstring appPath = L"\"";
    appPath += GetNppExecutableFullPath().c_str();
    appPath += L"\"";

    wstring filePathsArg = appPath;
    filePathsArg += L" ";

    for (DWORD i = 0; i < count; ++i)
    {
        psiItemArray->GetItemAt(i, &psi);
        RETURN_IF_FAILED(psi->GetDisplayName(SIGDN_FILESYSPATH, &file2OpenPath));
        // Release the IShellItem pointer, since we are done with it as well.
        psi->Release();

        filePathsArg += L"\"";
        filePathsArg += file2OpenPath;
        filePathsArg += L"\" ";

        // Cleanup itemName, since we are done with it.
        if (file2OpenPath)
        {
            CoTaskMemFree(file2OpenPath);
        }
    }

    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    if (!CreateProcessW(GetNppExecutableFullPath().c_str(), (LPWSTR)filePathsArg.c_str(), nullptr, nullptr, false, CREATE_NEW_PROCESS_GROUP, nullptr, nullptr, &si, &pi))
    {
        return S_OK;
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return S_OK;
}
CATCH_RETURN();

const EXPCMDSTATE BaseNppExplorerCommandHandler::State(IShellItemArray* psiItemArray)
{
    UNREFERENCED_PARAMETER(psiItemArray);

    throw L"State must be overridden in all implementations";
}