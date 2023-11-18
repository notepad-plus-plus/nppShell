#pragma once
#include "pch.h"

namespace NppShell::Installer
{
    HRESULT RegisterOldContextMenu();
    HRESULT UnregisterOldContextMenu();

    HRESULT RegisterSparsePackageAllUsers();
    HRESULT RegisterSparsePackageCurrentUser();
    HRESULT UnregisterSparsePackage();

    HRESULT Install();
    HRESULT Uninstall();

    void EnsureRegistrationOnCurrentUser();
}

STDAPI CleanupDll();