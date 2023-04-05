#pragma once
#include "pch.h"

#include <filesystem>

namespace NppShell::Helpers
{
    const wstring GetApplicationPath();
    const wstring GetContextMenuPath();
    const wstring GetContextMenuFullName();
    const wstring GetExecutingModuleName();
}