#include "pch.h"

#include "Installer.h"
#include "SimpleFactory.h"
#include "ClassicEditWithNppExplorerCommandHandler.h"
#include "ModernEditWithNppExplorerCommandHandler.h"
#include "LoggingHelper.h"

using namespace NppShell::CommandHandlers;
using namespace NppShell::Factories;
using namespace NppShell::Helpers;

HMODULE g_module;
LoggingHelper g_loggingHelper;

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    UNREFERENCED_PARAMETER(lpReserved);

    switch (ul_reason_for_call)
    {
        case DLL_PROCESS_ATTACH:
            g_module = hModule;
            try
            {
                NppShell::Installer::EnsureRegistrationOnCurrentUser();
            }
            catch (...)
            {
                // Never crash process attach due to optional registration checks.
                g_loggingHelper.LogMessage(L"DllMain", L"Unknown exception during EnsureRegistrationOnCurrentUser.");
            }
            break;
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
        case DLL_PROCESS_DETACH:
            break;
    }

    return TRUE;
}

STDAPI DllRegisterServer()
{
    try
    {
        return NppShell::Installer::Install();
    }
    catch (const winrt::hresult_error& ex)
    {
        g_loggingHelper.LogMessage(L"DllRegisterServer", L"WinRT exception: " + LoggingHelper::FormatHResult(ex.code()));
        return ex.code();
    }
    catch (const exception& ex)
    {
        wstring whatMessage;
        if (ex.what() != nullptr)
        {
            const string message(ex.what());
            whatMessage.assign(message.begin(), message.end());
        }

        g_loggingHelper.LogMessage(L"DllRegisterServer", L"std::exception: " + whatMessage);
        return E_FAIL;
    }
    catch (...)
    {
        g_loggingHelper.LogMessage(L"DllRegisterServer", L"Unknown exception.");
        return E_FAIL;
    }
}

STDAPI DllUnregisterServer()
{
    try
    {
        return NppShell::Installer::Uninstall();
    }
    catch (const winrt::hresult_error& ex)
    {
        g_loggingHelper.LogMessage(L"DllUnregisterServer", L"WinRT exception: " + LoggingHelper::FormatHResult(ex.code()));
        return ex.code();
    }
    catch (const exception& ex)
    {
        wstring whatMessage;
        if (ex.what() != nullptr)
        {
            const string message(ex.what());
            whatMessage.assign(message.begin(), message.end());
        }

        g_loggingHelper.LogMessage(L"DllUnregisterServer", L"std::exception: " + whatMessage);
        return E_FAIL;
    }
    catch (...)
    {
        g_loggingHelper.LogMessage(L"DllUnregisterServer", L"Unknown exception.");
        return E_FAIL;
    }
}

__control_entrypoint(DllExport)
STDAPI DllCanUnloadNow(void)
{
    if (winrt::get_module_lock())
    {
        return S_FALSE;
    }
    else
    {
        return S_OK;
    }
}

_Use_decl_annotations_ STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv) try
{
    *ppv = nullptr;

    if (rclsid == __uuidof(ClassicEditWithNppExplorerCommandHandler))
    {
        return winrt::make<SimpleFactory<ClassicEditWithNppExplorerCommandHandler>>().as(riid, ppv);
    }
    else if (rclsid == __uuidof(ModernEditWithNppExplorerCommandHandler))
    {
        return winrt::make<SimpleFactory<ModernEditWithNppExplorerCommandHandler>>().as(riid, ppv);
    }
    else
    {
        return CLASS_E_CLASSNOTAVAILABLE;
    }
}
catch (...)
{
    return winrt::to_hresult();
}