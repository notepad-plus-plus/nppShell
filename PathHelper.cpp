#include "pch.h"
#include "PathHelper.h"

using namespace NppShell::Helpers;
using namespace std::filesystem;

extern HMODULE g_module;

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

const wstring NppShell::Helpers::GetContextMenuFullName()
{
    path modulePath = GetModulePath();
    return modulePath.wstring();
}