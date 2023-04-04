#include "pch.h"
#include "RegistryKey.h"
#include "LoggingHelper.h"

using namespace NppShell::Registry;
using namespace NppShell::Helpers;

extern LoggingHelper g_loggingHelper;

RegistryKey::RegistryKey(HKEY hKey, const wstring& subKey, REGSAM access, bool createIfMissing)
    : m_hKey(nullptr), m_regsam(access), m_originalHKey(hKey), m_originalSubKey(subKey)
{
    if (RegOpenKeyExW(hKey, subKey.data(), 0, access, &m_hKey) == ERROR_SUCCESS)
    {
        g_loggingHelper.LogMessage(L"RegistryKey::ctor", L"Opened sub key: " + subKey);
        return;
    }

    if (!createIfMissing)
    {
        return;
    }

    DWORD disposition = 0;

    if (RegCreateKeyExW(hKey, subKey.data(), 0, nullptr, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, nullptr, &m_hKey, &disposition) != ERROR_SUCCESS)
    {
        g_loggingHelper.LogMessage(L"RegistryKey::ctor", L"Failed to create sub key: " + subKey);
        throw runtime_error("Failed to create registry key.");
    }
    else
    {
        g_loggingHelper.LogMessage(L"RegistryKey::ctor", L"Created sub key: " + subKey);
    }
}

RegistryKey::~RegistryKey()
{
    if (m_hKey != nullptr)
    {
        RegCloseKey(m_hKey);
    }
}

bool RegistryKey::KeyExists(HKEY hKey, const wstring& subKey)
{
    HKEY result;
    LONG status = RegOpenKeyExW(hKey, subKey.data(), 0, KEY_READ, &result);

    if (status == ERROR_SUCCESS)
    {
        RegCloseKey(result);

        return true;
    }
    else if (status == ERROR_FILE_NOT_FOUND)
    {
        return false;
    }
    else
    {
        throw runtime_error("Error opening registry key.");
    }
}

bool RegistryKey::ValueExists(const wstring& valueName) const
{
    DWORD type;
    DWORD size = 0;

    LONG status = RegQueryValueExW(m_hKey, valueName.empty() ? NULL : valueName.data(), nullptr, &type, nullptr, &size);

    if (status == ERROR_SUCCESS)
    {
        return true;
    }
    else if (status == ERROR_FILE_NOT_FOUND)
    {
        return false;
    }
    else
    {
        throw runtime_error("Error querying registry value.");
    }
}

RegistryKey RegistryKey::GetSubKey(const wstring& subKey, bool createIfMissing) const
{
    if (m_hKey == nullptr)
    {
        throw runtime_error("Registry key is not open.");
    }

    HKEY hNewKey;
    if (RegOpenKeyExW(m_hKey, NULL, 0, m_regsam, &hNewKey) != ERROR_SUCCESS)
    {
        throw runtime_error("Failed to open registry key.");
    }

    return RegistryKey(hNewKey, subKey, m_regsam, createIfMissing);
}

DWORD RegistryKey::GetDwordValue(const wstring& valueName)
{
    if (m_hKey == nullptr)
    {
        throw runtime_error("Registry key is not open.");
    }

    DWORD value = 0;
    DWORD dataSize = sizeof(DWORD);

    if (RegGetValueW(m_hKey, nullptr, valueName.empty() ? NULL : valueName.data(), RRF_RT_REG_DWORD, nullptr, &value, &dataSize) != ERROR_SUCCESS)
    {
        g_loggingHelper.LogMessage(L"RegistryKey::GetDwordValue", L"Failed to get DWORD value: " + valueName);
        throw runtime_error("Failed to get registry value.");
    }

    return value;
}

wstring RegistryKey::GetStringValue(const wstring& valueName)
{
    if (m_hKey == nullptr)
    {
        throw runtime_error("Registry key is not open.");
    }

    DWORD dataSize = 0;

    if (RegGetValueW(m_hKey, nullptr, valueName.empty() ? NULL : valueName.data(), RRF_RT_REG_SZ, nullptr, nullptr, &dataSize) != ERROR_SUCCESS)
    {
        g_loggingHelper.LogMessage(L"RegistryKey::GetStringValue", L"Failed to get REG_SZ value: " + valueName);
        throw runtime_error("Failed to get registry value size.");
    }

    wstring value(dataSize / sizeof(wchar_t), L'\0');

    if (RegGetValueW(m_hKey, nullptr, valueName.empty() ? NULL : valueName.data(), RRF_RT_REG_SZ, nullptr, bit_cast<BYTE*>(value.data()), &dataSize) != ERROR_SUCCESS)
    {
        g_loggingHelper.LogMessage(L"RegistryKey::GetStringValue", L"Failed to get REG_SZ value: " + valueName);
        throw runtime_error("Failed to get registry value.");
    }

    return value;
}

void RegistryKey::SetDwordValue(const wstring& valueName, DWORD value)
{
    if (m_hKey == nullptr)
    {
        throw runtime_error("Registry key is not open.");
    }

    if (RegSetValueExW(m_hKey, valueName.empty() ? NULL : valueName.data(), 0, REG_DWORD, bit_cast<const BYTE*>(&value), sizeof(DWORD)) != ERROR_SUCCESS)
    {
        g_loggingHelper.LogMessage(L"RegistryKey::SetDwordValue", L"Failed to set DWORD value: " + valueName);
        throw runtime_error("Failed to set registry value.");
    }

    wstring valueNameLog = valueName.empty() ? L"(default)" : valueName.data();
    g_loggingHelper.LogMessage(L"RegistryKey::SetDwordValue", L"Setting DWORD name: " + valueNameLog + L" to: " + to_wstring(value));
}

void RegistryKey::SetStringValue(const wstring& valueName, const wstring& value)
{
    if (m_hKey == nullptr)
    {
        throw runtime_error("Registry key is not open.");
    }

    if (RegSetValueExW(m_hKey, valueName.empty() ? NULL : valueName.data(), 0, REG_SZ, bit_cast<const BYTE*>(value.data()), static_cast<DWORD>((value.length() + 1) * sizeof(wchar_t))) != ERROR_SUCCESS)
    {
        g_loggingHelper.LogMessage(L"RegistryKey::SetStringValue", L"Failed to set REG_SZ value: " + valueName);
        throw runtime_error("Failed to set registry value.");
    }

    wstring valueNameLog = valueName.empty() ? L"(default)" : valueName.data();
    g_loggingHelper.LogMessage(L"RegistryKey::SetStringValue", L"Setting REG_SZ name: " + valueNameLog + L" to: \"" + value + L"\"");
}

void RegistryKey::DeleteKey()
{
    if (m_hKey == nullptr)
    {
        throw runtime_error("Registry key is not open.");
    }
    
    if (RegDeleteTreeW(m_originalHKey, m_originalSubKey.data()) != ERROR_SUCCESS)
    {
        g_loggingHelper.LogMessage(L"RegistryKey::DeleteKey", L"Failed to delete subkey: " + m_originalSubKey);
        throw runtime_error("Failed to delete registry key.");
    }

    g_loggingHelper.LogMessage(L"RegistryKey::DeleteKey", L"Deleted subkey: " + m_originalSubKey);

    m_hKey = nullptr;
    m_originalHKey = nullptr;
}