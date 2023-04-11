#pragma once
#include "pch.h"

namespace NppShell::Installer
{
    HRESULT RegisterOldContextMenu(const wstring& dllName);
    HRESULT UnregisterOldContextMenu();

    HRESULT RegisterSparsePackage();
    HRESULT UnregisterSparsePackage();

    HRESULT Install();
    HRESULT Uninstall();

    void EnsureRegistrationOnCurrentUser();
    void EnsureCorrectFileName(const wstring& targetFileName);
}

STDAPI CleanupDll();