# Windows 11 Modern UI Context Menu integration

The purpose of this project is to allow Notepad++ to integrate into the new Windows 11 right-click context menu.
Doing this requires two new things.

* A dll library with some COM objects that the shell can communicate with.
* A Sparse Package containing the metadata for the COM server.

To build this, the following steps needs to be taken:

1. Build a Release dll file three times (Win32, x64 and ARM64) resulting in three dll files (NppShell.x86.dll, NppShell.x64.dll and NppShell.arm64.dll)
2. Generate a Sparse Package (NppShell.msix)
3. Sign both of these with signtool.exe
4. Make sure they are included in the installer, so they are deployed next to the notepad++.exe program.
5. The installer should, upon installation, install the package.
6. The installer should, upon uninstallation, uninstall the package.

## Prerequisites

To be able to build this project, the following is needed:

* [Visual Studio 2022](https://visualstudio.microsoft.com/vs)
* [Windows 11 SDK](https://developer.microsoft.com/en-us/windows/downloads/windows-sdk)

## Build a Release dll file (NppShell.dll)
Just open the NppShell.sln Visual Studio solution, select Release as the build type, and do a Rebuild of the solution.

## Generate a Sparse Package (NppShell.msix)
To generate a Sparse Package, you need to have the makeappx.exe tool in your PATH, the easiest way to do this is to run the `Developer Command Prompt for VS 2022` command prompt, since it sets up the path.
Once inside the NppShell folder, run the following command to generate the Sparse Package:
```
makeappx pack /d .\Packaging /p .\NppShell.msix /nv
```
This takes the content of the Packaging directory, and packages them up into the msix file.

## Sign both of these with signtool.exe
Now we have both the `NppShell.dll` and `NppShell.msix` files, we need to sign them with a valid certificate.
To do this, once again run the `Developer Command Prompt for VS 2022` command prompt and change to the NppShell folder.
The following command expects the following:
* The pfx certificate is called MyCert.pfx
* The password for the pfx certificate is: `Test1234`

Make the needed changes to match the real certificate.
```
SignTool.exe sign /fd SHA256 /tr http://timestamp.digicert.com /td sha256 /a /f .\MyCert.pfx /p Test1234 /d "Notepad++" /du https://notepad-plus-plus.org/ NppShell.msix
SignTool.exe sign /fd SHA256 /tr http://timestamp.digicert.com /td sha256 /a /f .\MyCert.pfx /p Test1234 /d "Notepad++" /du https://notepad-plus-plus.org/ (ARCH)\Release\NppShell.dll
```
Now both files has been signed, and can be used.

## Make sure they are included in the installer, so they are deployed next to the notepad++.exe program.
The installer needs to deploy the two files into the same directory as notepad++.exe .
They need to be there, since the DLL is looking for notepad++.exe in the same directory as it is located itself.

## The installer should, upon installation, install the package.
When the installer is running, after all the files has been copied into the program files directory, the follow command should be be run to run the register function to register the package:
```
regsvr32.exe /s .\contextmenu\NppShell.dll
```

Remember to wait for the regsvr32 process to exit before continuing.

## The installer should, upon uninstallation, uninstall the package.
When the uninstaller is running, it should run this command to unregister the package:
```
regsvr32.exe /s /u .\contextmenu\NppShell.dll
```

Here we need to wait for regsvr32 to finish, since if it isn't finished, the dll file will be locked by explorer.