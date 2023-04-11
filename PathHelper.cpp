#include "pch.h"
#include "PathHelper.h"

using namespace NppShell::Helpers;
using namespace std::filesystem;

extern HMODULE g_module;

const wstring GetModuleName(HMODULE hModule)
{
    wchar_t pathBuffer[FILENAME_MAX] = { 0 };
    GetModuleFileNameW(hModule, pathBuffer, FILENAME_MAX);
    PathStripPathW(pathBuffer);

    return wstring(pathBuffer);
}

const path GetModulePath()
{
    wchar_t pathBuffer[MAX_PATH] = { 0 };
    GetModuleFileNameW(g_module, pathBuffer, MAX_PATH);
    return path(pathBuffer);
}

const wstring NppShell::Helpers::GetApplicationPath()
{
    path modulePath = GetModulePath();
    return modulePath.parent_path().parent_path().wstring();
}

const wstring NppShell::Helpers::GetContextMenuPath()
{
    path modulePath = GetModulePath();
    return modulePath.parent_path().wstring();
}

const wstring NppShell::Helpers::GetContextMenuName()
{
    return GetModuleName(g_module);
}

const wstring NppShell::Helpers::GetExecutingModuleName()
{
    wstring moduleName = GetModuleName(NULL);

    transform(moduleName.begin(), moduleName.end(), moduleName.begin(), towlower);

    return moduleName;
}