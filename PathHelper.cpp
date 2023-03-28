#include "pch.h"
#include "PathHelper.h"

using namespace NppShell::Helpers;
using namespace std::filesystem;

extern HMODULE thisModule;

const path GetThisModulePath()
{
    wchar_t pathBuffer[FILENAME_MAX] = { 0 };
    GetModuleFileName(thisModule, pathBuffer, FILENAME_MAX);
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