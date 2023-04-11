#include "pch.h"
#include "Installer.h"

#include "ClassicEditWithNppExplorerCommandHandler.h"
#include "PathHelper.h"
#include "AclHelper.h"
#include "RegistryKey.h"

#define GUID_STRING_SIZE 40

using namespace winrt::Windows::ApplicationModel;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Foundation::Collections;
using namespace winrt::Windows::Management::Deployment;

using namespace NppShell::Helpers;
using namespace NppShell::Installer;
using namespace NppShell::Registry;


extern HMODULE g_module;
extern thread ensureRegistrationThread;

const wstring SparsePackageName = L"NotepadPlusPlus";
constexpr int FirstWindows11BuildNumber = 22000;

#ifdef WIN64
const wstring ShellKey = L"Software\\Classes\\*\\shell\\ANotepad++64";
const wstring ShellExtensionKey = L"Software\\Classes\\*\\shellex\\ContextMenuHandlers\\ANotepad++64";
#else
const wstring ShellKey = L"Software\\Classes\\*\\shell\\ANotepad++";
const wstring ShellExtensionKey = L"Software\\Classes\\*\\shellex\\ContextMenuHandlers\\ANotepad++";
#endif

bool IsWindows11Installation()
{
    RegistryKey registryKey(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion");
    wstring buildNumberString = registryKey.GetStringValue(L"CurrentBuildNumber");

    const int buildNumber = stoi(buildNumberString);

    return buildNumber >= FirstWindows11BuildNumber;
}

wstring GetCLSIDString()
{
    const auto uuid = __uuidof(NppShell::CommandHandlers::ClassicEditWithNppExplorerCommandHandler);

    LPOLESTR guidString = 0;
    const HRESULT result = StringFromCLSID(uuid, &guidString);

    if (FAILED(result))
    {
        if (guidString)
        {
            CoTaskMemFree(guidString);
        }

        throw "Failed to parse GUID from command handler";
    }

    wstring guid(guidString);

    if (guidString)
    {
        CoTaskMemFree(guidString);
    }

    return guid;
}

void inline CleanupRegistry(const wstring& guid)
{
    // First we remove the shell key if it exists.
    if (RegistryKey::KeyExists(HKEY_LOCAL_MACHINE, ShellKey))
    {
        RegistryKey registryKey(HKEY_LOCAL_MACHINE, ShellKey, KEY_READ | KEY_WRITE);
        registryKey.DeleteKey();
    }

    // Then we remove the shell extension key if it exists.
    if (RegistryKey::KeyExists(HKEY_LOCAL_MACHINE, ShellExtensionKey))
    {
        RegistryKey registryKey(HKEY_LOCAL_MACHINE, ShellExtensionKey, KEY_READ | KEY_WRITE);
        registryKey.DeleteKey();
    }

    // Then we remove the Notepad++_file key if it exists.
    if (RegistryKey::KeyExists(HKEY_LOCAL_MACHINE, L"Notepad++_file\\shellex"))
    {
        RegistryKey registryKey(HKEY_LOCAL_MACHINE, L"Notepad++_file\\shellex", KEY_READ | KEY_WRITE);
        registryKey.DeleteKey();
    }

    // Finally we remove the CLSID key if it exists.
    if (RegistryKey::KeyExists(HKEY_LOCAL_MACHINE, L"Software\\Classes\\CLSID\\" + guid))
    {
        RegistryKey registryKey(HKEY_LOCAL_MACHINE, L"Software\\Classes\\CLSID\\" + guid, KEY_READ | KEY_WRITE);
        registryKey.DeleteKey();
    }
}

void inline CleanupHack()
{
    // First we test if the key even exists.
    if (!RegistryKey::KeyExists(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Classes\\*\\shell\\pintohome"))
    {
        return;
    }

    // If it does, we open it and check if the value exists.
    wstring valueName = L"MUIVerb";
    RegistryKey registryKey(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Classes\\*\\shell\\pintohome", KEY_READ | KEY_WRITE);

    if (!registryKey.ValueExists(valueName))
    {
        return;
    }

    // Then we get the value and see if it contains the text "Notepad++"
    wstring currentValue = registryKey.GetStringValue(valueName);
    bool found = currentValue.find(L"Notepad++") != wstring::npos;

    // If we found the text, we delete the entire key.
    if (found)
    {
        registryKey.DeleteKey();
    }
}

HRESULT MoveFileToTempAndScheduleDeletion(const wstring& filePath, bool moveToTempDirectory)
{
    wstring tempPath(MAX_PATH, L'\0');
    wstring tempFileName(MAX_PATH, L'\0');

    BOOL moveResult;

    if (moveToTempDirectory)
    {
        // First we get the path to the temporary directory.
        GetTempPath(MAX_PATH, &tempPath[0]);

        // Then we get a temporary filename in the temporary directory.
        GetTempFileName(tempPath.c_str(), L"tempFileName", 0, &tempFileName[0]);

        // Move the file into the temp directory - it can be moved even when it is loaded into memory and locked.
        moveResult = MoveFileEx(filePath.c_str(), tempFileName.c_str(), MOVEFILE_REPLACE_EXISTING);

        if (!moveResult)
        {
            return S_FALSE;
        }

        // Schedule it to be deleted from the temp directory on the next reboot.
        moveResult = MoveFileExW(tempFileName.c_str(), NULL, MOVEFILE_DELAY_UNTIL_REBOOT);
    }
    else
    {
        // Schedule it to be deleted on the next reboot, without moving it.
        moveResult = MoveFileExW(filePath.c_str(), NULL, MOVEFILE_DELAY_UNTIL_REBOOT);
    }

    if (!moveResult)
    {
        return S_FALSE;
    }

    return S_OK;
}

void ResetAclPermissionsOnApplicationFolder()
{
    // First we get the path where Notepad++ is installed.
    const wstring applicationPath = GetApplicationPath();

    // Create a new AclHelper
    AclHelper aclHelper;

    // Reset the ACL of the folder where Notepad++ is installed.
    aclHelper.ResetAcl(applicationPath);
}

Package GetSparsePackage()
{
    PackageManager packageManager;
    IIterable<Package> packages;

    try
    {
        packages = packageManager.FindPackagesForUser(L"");
    }
    catch (winrt::hresult_error)
    {
        return NULL;
    }

    for (const Package& package : packages)
    {
        if (package.Id().Name() != SparsePackageName)
        {
            continue;
        }

        return package;
    }

    return NULL;
}

HRESULT NppShell::Installer::RegisterSparsePackage()
{
    PackageManager packageManager;
    AddPackageOptions options;

    const wstring externalLocation = GetContextMenuPath();
    const wstring sparsePkgPath = externalLocation + L"\\NppShell.msix";

    Uri externalUri(externalLocation);
    Uri packageUri(sparsePkgPath);

    options.ExternalLocationUri(externalUri);

    auto deploymentOperation = packageManager.AddPackageByUriAsync(packageUri, options);
    auto deployResult = deploymentOperation.get();

    if (!SUCCEEDED(deployResult.ExtendedErrorCode()))
    {
        return deployResult.ExtendedErrorCode();
    }

    return S_OK;
}

HRESULT NppShell::Installer::UnregisterSparsePackage()
{
    PackageManager packageManager;
    IIterable<Package> packages;

    Package package = GetSparsePackage();
    
    if (package == NULL)
    {
        return S_FALSE;
    }

    winrt::hstring fullName = package.Id().FullName();
    auto deploymentOperation = packageManager.RemovePackageAsync(fullName, RemovalOptions::None);
    auto deployResult = deploymentOperation.get();

    if (!SUCCEEDED(deployResult.ExtendedErrorCode()))
    {
        return deployResult.ExtendedErrorCode();
    }

    // After unregistering the sparse package, we reset the folder permissions of the folder where we are installed.
    ResetAclPermissionsOnApplicationFolder();

    return S_OK;
}

HRESULT NppShell::Installer::RegisterOldContextMenu()
{
    const wstring contextMenuFullName = GetContextMenuFullName();
    const wstring guid = GetCLSIDString();

    // First we set the shell extension values.
    RegistryKey regKeyExtension(HKEY_LOCAL_MACHINE, ShellKey, KEY_READ | KEY_WRITE, true);
    regKeyExtension.SetStringValue(L"", L"Notepad++ Context menu");
    regKeyExtension.SetStringValue(L"ExplorerCommandHandler", guid);
    regKeyExtension.SetStringValue(L"NeverDefault", L"");

    // Then we create the CLSID for the handler with it's values.
    RegistryKey regKeyClsid(HKEY_LOCAL_MACHINE, L"Software\\Classes\\CLSID\\" + guid, KEY_READ | KEY_WRITE, true);
    regKeyClsid.SetStringValue(L"", L"notepad++");

    RegistryKey regKeyInProc = regKeyClsid.GetSubKey(L"InProcServer32", true);
    regKeyInProc.SetStringValue(L"", contextMenuFullName);
    regKeyInProc.SetStringValue(L"ThreadingModel", L"Apartment");

    return S_OK;
}

HRESULT NppShell::Installer::UnregisterOldContextMenu()
{
    const wstring guid = GetCLSIDString();

    // Clean up registry entries.
    CleanupRegistry(guid);

    return S_OK;
}

HRESULT NppShell::Installer::Install()
{
    const bool isWindows11 = IsWindows11Installation();

    HRESULT result;

    // Clean up the 8.5 Windows 11 hack if present.
    CleanupHack();

    if (isWindows11)
    {
        // We need to unregister the old menu on Windows 11 to prevent double entries in the old menu.
        UnregisterOldContextMenu();

        // Since we are on Windows 11, we unregister the sparse package as well.
        UnregisterSparsePackage();

        // And then we register it again.
        result = RegisterSparsePackage();
    }

    result = RegisterOldContextMenu();

    // Ensure we schedule old files for removal on next reboot.
    MoveFileToTempAndScheduleDeletion(GetApplicationPath() + L"\\NppShell_01.dll", false);
    MoveFileToTempAndScheduleDeletion(GetApplicationPath() + L"\\NppShell_02.dll", false);
    MoveFileToTempAndScheduleDeletion(GetApplicationPath() + L"\\NppShell_03.dll", false);
    MoveFileToTempAndScheduleDeletion(GetApplicationPath() + L"\\NppShell_04.dll", false);
    MoveFileToTempAndScheduleDeletion(GetApplicationPath() + L"\\NppShell_05.dll", false);
    MoveFileToTempAndScheduleDeletion(GetApplicationPath() + L"\\NppShell_06.dll", false);

    // This include the old NppModernShell and NppShell files from the main program directory.
    MoveFileToTempAndScheduleDeletion(GetApplicationPath() + L"\\NppShell.dll", false);
    MoveFileToTempAndScheduleDeletion(GetApplicationPath() + L"\\NppShell.msix", false);
    MoveFileToTempAndScheduleDeletion(GetApplicationPath() + L"\\NppModernShell.dll", false);
    MoveFileToTempAndScheduleDeletion(GetApplicationPath() + L"\\NppModernShell.msix", false);

    // Finally we notify the shell that we have made changes, so it refreshes the context menus.
    SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, 0, 0);

    return result;
}

HRESULT NppShell::Installer::Uninstall()
{
    const bool isWindows11 = IsWindows11Installation();

    HRESULT result;

    // We remove the old context menu in all cases, since both Windows 11 and older versions can have it setup if upgrading.
    result = UnregisterOldContextMenu();

    if (result != S_OK)
    {
        return result;
    }

    if (isWindows11)
    {
        // Since we are on Windows 11, we unregister the sparse package as well.
        result = UnregisterSparsePackage();

        if (result != S_OK)
        {
            return result;
        }
    }

    // Finally we notify the shell that we have made changes, so it refreshes the context menus.
    SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, 0, 0);

    return S_OK;
}

void EnsureRegistrationOnCurrentUserWorker()
{
    // Initialize the WinRT apartment.
    winrt::init_apartment();

    // Get the package to check if it is already installed for the current user.
    Package existingPackage = GetSparsePackage();

    if (existingPackage == NULL)
    {
        // The package is not installed for the current user - but we know that Notepad++ is.
        // If it wasn't, this code wouldn't be running, so it is safe to just register the package.
        RegisterSparsePackage();

        // Finally we notify the shell that we have made changes, so it reloads the right click menu items.
        SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, 0, 0);
    }
}

void NppShell::Installer::EnsureRegistrationOnCurrentUser()
{
    // First we find the name of the process the DLL is being loaded into.
    wstring moduleName = GetExecutingModuleName();

    if (moduleName == L"explorer.exe")
    {
        const bool isWindows11 = IsWindows11Installation();

        if (isWindows11)
        {
            // We are being loaded into explorer.exe, so we can continue.
            // Explorer.exe only loads the DLL on the first time a user right-clicks a file
            // after that it stays in memory for the rest of their session.
            // Since we are here, we spawn a thread and call the EnsureRegistrationOnCurrentUserWorker function.
            ensureRegistrationThread = thread(EnsureRegistrationOnCurrentUserWorker);
            ensureRegistrationThread.detach();
        }
    }
}

STDAPI CleanupDll()
{
    // First we get the full path to this DLL.
    wstring currentFilePath(MAX_PATH, L'\0');
    GetModuleFileName(g_module, &currentFilePath[0], MAX_PATH);

    // Then we get it moved out of the way and scheduled for deletion.
    return MoveFileToTempAndScheduleDeletion(currentFilePath, true);
}