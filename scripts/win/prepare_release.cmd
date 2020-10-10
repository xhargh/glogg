set "QTDIR=%Qt5_Dir%"
set "Qt5_DIR=%QTDIR:/=\%"
echo %Qt5_DIR%

md %KLOGG_WORKSPACE%\release

xcopy %KLOGG_WORKSPACE%\build\output\klogg_io_portable.exe %KLOGG_WORKSPACE%\release\ /y
xcopy %KLOGG_WORKSPACE%\build\output\klogg_io_portable.pdb %KLOGG_WORKSPACE%\release\ /y
xcopy %KLOGG_WORKSPACE%\build\output\klogg-io.exe %KLOGG_WORKSPACE%\release\ /y
xcopy %KLOGG_WORKSPACE%\build\output\klogg-io.pdb %KLOGG_WORKSPACE%\release\ /y
xcopy %KLOGG_WORKSPACE%\build\generated\documentation.html %KLOGG_WORKSPACE%\release\ /y

xcopy %KLOGG_WORKSPACE%\build\output\klogg_tbbmalloc.dll %KLOGG_WORKSPACE%\release\ /y
xcopy %KLOGG_WORKSPACE%\build\output\klogg_tbbmalloc.pdb %KLOGG_WORKSPACE%\release\ /y
xcopy %KLOGG_WORKSPACE%\build\output\klogg_tbbmalloc_proxy.dll %KLOGG_WORKSPACE%\release\ /y
xcopy %KLOGG_WORKSPACE%\build\output\klogg_tbbmalloc_proxy.pdb %KLOGG_WORKSPACE%\release\ /y

xcopy %KLOGG_WORKSPACE%\COPYING %KLOGG_WORKSPACE%\release\ /y
xcopy %KLOGG_WORKSPACE%\NOTICE %KLOGG_WORKSPACE%\release\ /y
xcopy %KLOGG_WORKSPACE%\README.md %KLOGG_WORKSPACE%\release\ /y
xcopy %KLOGG_WORKSPACE%\DOCUMENTATION.md %KLOGG_WORKSPACE%\release\ /y

xcopy "%VCToolsRedistDir%%platform%\Microsoft.VC142.CRT\msvcp140.dll" %KLOGG_WORKSPACE%\release\ /y
xcopy "%VCToolsRedistDir%%platform%\Microsoft.VC142.CRT\vcruntime140.dll" %KLOGG_WORKSPACE%\release\ /y
xcopy "%VCToolsRedistDir%%platform%\Microsoft.VC142.CRT\vcruntime140_1.dll" %KLOGG_WORKSPACE%\release\ /y

md %KLOGG_WORKSPACE%\release\platforms
md %KLOGG_WORKSPACE%\release\styles

xcopy %Qt5_DIR%\bin\Qt5Core.dll %KLOGG_WORKSPACE%\release\ /y
xcopy %Qt5_DIR%\bin\Qt5Gui.dll %KLOGG_WORKSPACE%\release\ /y
xcopy %Qt5_DIR%\bin\Qt5Network.dll %KLOGG_WORKSPACE%\release\ /y
xcopy %Qt5_DIR%\bin\Qt5SerialPort.dll %KLOGG_WORKSPACE%\release\ /y
xcopy %Qt5_DIR%\bin\Qt5Widgets.dll %KLOGG_WORKSPACE%\release\ /y
xcopy %Qt5_DIR%\bin\Qt5Concurrent.dll %KLOGG_WORKSPACE%\release\ /y
xcopy %Qt5_DIR%\bin\Qt5Xml.dll %KLOGG_WORKSPACE%\release\ /y
xcopy %Qt5_DIR%\plugins\platforms\qwindows.dll %KLOGG_WORKSPACE%\release\platforms\ /y
xcopy %Qt5_DIR%\plugins\styles\qwindowsvistastyle.dll %KLOGG_WORKSPACE%\release\styles /y

xcopy %SSL_DIR%\libcrypto-1_1%SSL_ARCH%.dll %KLOGG_WORKSPACE%\release\ /y
xcopy %SSL_DIR%\libssl-1_1%SSL_ARCH%.dll %KLOGG_WORKSPACE%\release\ /y

md %KLOGG_WORKSPACE%\chocolately
md %KLOGG_WORKSPACE%\chocolately\tools

xcopy %KLOGG_WORKSPACE%\packaging\windows\klogg-io.nuspec chocolately /y
xcopy %KLOGG_WORKSPACE%\packaging\windows\chocolatelyInstall.ps1 chocolately\tools\ /y

xcopy %KLOGG_WORKSPACE%\packaging\windows\klogg-io.nsi  /y
xcopy %KLOGG_WORKSPACE%\packaging\windows\FileAssociation.nsh  /y

7z a %KLOGG_WORKSPACE%\klogg-%KLOGG_VERSION%-%KLOGG_ARCH%-portable.7z @%KLOGG_WORKSPACE%\scripts\win\7z_klogg_io_listfile.txt
7z a %KLOGG_WORKSPACE%\klogg-%KLOGG_VERSION%-%KLOGG_ARCH%-pdb.7z @%KLOGG_WORKSPACE%\scripts\win\7z_pdb_listfile.txt
