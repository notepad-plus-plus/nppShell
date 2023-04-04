#pragma once

namespace NppShell::Helpers
{
    class LoggingHelper
    {
    public:
        LoggingHelper();
        void LogMessage(const wstring& source, const wstring& message);

    private:
        wstring GetRoamingAppDataFolderPath();
        wstring CreateAppDataFolder();
        wstring GetTimestamp();
        bool IsAppDataLoggingEnabled();

        bool appDataLoggingEnabled;
        wstring logFilePath;
    };
}