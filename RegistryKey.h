#pragma once

namespace NppShell::Registry
{
    class RegistryKey
    {
    public:
        RegistryKey(HKEY hKey, const wstring& subKey = L"", REGSAM access = KEY_READ, bool createIfMissing = false);
        ~RegistryKey();

        static bool KeyExists(HKEY hKey, const wstring& subKey);
        bool ValueExists(const wstring& valueName) const;

        RegistryKey GetSubKey(const wstring& subKey, bool createIfMissing = false) const;

        DWORD GetDwordValue(const wstring& valueName);
        wstring GetStringValue(const wstring& valueName);

        void SetDwordValue(const wstring& valueName, DWORD value);
        void SetStringValue(const wstring& valueName, const wstring& value);

        void DeleteKey();

    private:
        HKEY m_hKey;
        REGSAM m_regsam;
        HKEY m_originalHKey;
        wstring m_originalSubKey;
    };
}