#pragma once
#include "pch.h"

namespace NppShell::Installer
{
    HRESULT RegisterOldContextMenu();
    HRESULT UnregisterOldContextMenu();

    HRESULT RegisterSparsePackage();
    HRESULT UnregisterSparsePackage();

    HRESULT Install();
    HRESULT Uninstall();
}

STDAPI CleanupDll();