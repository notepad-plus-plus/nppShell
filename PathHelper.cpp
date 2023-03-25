#include "pch.h"
#include "PathHelper.h"

using namespace NppShell::Helpers;
using namespace std::filesystem;

extern HMODULE thisModule;

const path GetThisModulePath()
{
    wchar_t pathBuffer[MAX_PATH] = { 0 };
    GetModuleFileNameW(thisModule, pathBuffer, MAX_PATH);
    return path(pathBuffer);
}

const wstring NppShell::Helpers::GetApplicationPath()
{
    path modulePath = GetThisModulePath();
    return modulePath.parent_path().parent_path().wstring();
}

const wstring NppShell::Helpers::GetContextMenuPath()
{
    path modulePath = GetThisModulePath();
    return modulePath.parent_path().wstring();
}

const wstring NppShell::Helpers::GetContextMenuFullName()
{
    path modulePath = GetThisModulePath();
    return modulePath.wstring();
}