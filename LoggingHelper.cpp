#include "pch.h"
#include "LoggingHelper.h"

#include <fstream>

using namespace NppShell::Helpers;

LoggingHelper::LoggingHelper()
{
    appDataLoggingEnabled = IsAppDataLoggingEnabled();

    if (!appDataLoggingEnabled)
    {
        return;
    }

    const wstring logFileFolder = CreateAppDataFolder();
    logFilePath = logFileFolder + L"\\NppShell.txt";
}

void LoggingHelper::LogMessage(const wstring& source, const wstring& message)
{
    if (!appDataLoggingEnabled)
    {
        return;
    }

    wofstream file(logFilePath, ios_base::app);
    if (file.is_open())
    {
        file << GetTimestamp();
        file << L" - ";
        file << source;
        file << L": ";
        file << message;
        file << endl;
        file.close();
    }
}

wstring LoggingHelper::GetRoamingAppDataFolderPath()
{
    // Initialize COM
    CoInitialize(NULL);

    // Create an instance of the KnownFolderManager interface
    IKnownFolderManager* pManager;
    HRESULT hr = CoCreateInstance(CLSID_KnownFolderManager, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pManager));

    if (FAILED(hr))
    {
        // Handle error
        return L"";
    }

    // Get the Roaming App Data folder
    IKnownFolder* pFolder;
    hr = pManager->GetFolder(FOLDERID_RoamingAppData, &pFolder);
    if (FAILED(hr))
    {
        // Handle error
        pManager->Release();
        return L"";
    }

    // Get the path to the Roaming App Data folder
    PWSTR pszPath = nullptr;
    hr = pFolder->GetPath(KF_FLAG_DEFAULT, &pszPath);
    if (FAILED(hr))
    {
        // Handle error
        pFolder->Release();
        pManager->Release();

        return L"";
    }

    // Convert the path to a wstring
    wstring path(pszPath);

    // Free the allocated memory
    CoTaskMemFree(pszPath);

    // Release interfaces
    pFolder->Release();
    pManager->Release();

    // Uninitialize COM
    CoUninitialize();

    // Return the path to the Roaming App Data folder
    return path;
}

wstring LoggingHelper::CreateAppDataFolder()
{
    wstring folderPath = GetRoamingAppDataFolderPath() + L"\\NppShell";

    if (!CreateDirectoryW(folderPath.c_str(), NULL) && GetLastError() != ERROR_ALREADY_EXISTS)
    {
        throw runtime_error("Failed to create folder");
    }

    return folderPath;
}

wstring LoggingHelper::GetTimestamp()
{
    SYSTEMTIME st;
    GetLocalTime(&st);

    wchar_t buffer[25];
    swprintf_s(buffer, L"%04d-%02d-%02d %02d:%02d:%02d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

    return wstring(buffer);
}

bool LoggingHelper::IsAppDataLoggingEnabled()
{
    const wstring environmentVariableName = L"NPPSHELL_LOGGING_ENABLED";

    // Get the size of the buffer required to hold the environment variable value
    DWORD size = GetEnvironmentVariableW(environmentVariableName.c_str(), nullptr, 0);
    if (size == 0)
    {
        // The specified environment variable was not found
        return false;
    }

    // Allocate a buffer to hold the environment variable value
    wstring buffer(size, L'\0');

    // Retrieve the environment variable value
    DWORD result = GetEnvironmentVariableW(environmentVariableName.c_str(), buffer.data(), size);
    if (result == 0 || result > size)
    {
        // An error occurred while retrieving the environment variable value
        return false;
    }

    // Return if the value is set to true
    return wstring(buffer.data()) == L"true";
}