# NSIS script creating the Windows installer for klogg-io

# Is passed to the script using -DVERSION=$(git describe) on the command line
!ifndef VERSION
    !define VERSION 'dev-build'
!endif

!ifndef PLATFORM
    !define PLATFORM 'unknown'
!endif

# Headers
!include "MUI2.nsh"
!include "FileAssociation.nsh"

# General
OutFile "klogg-io-${VERSION}-${PLATFORM}-setup.exe"

XpStyle on

SetCompressor /SOLID lzma

; Registry key to keep track of the directory we are installed in
!ifdef ARCH32
  InstallDir "$PROGRAMFILES\klogg-io"
!else
  InstallDir "$PROGRAMFILES64\klogg-io"
!endif
InstallDirRegKey HKLM Software\klogg-io ""

; klogg icon
; !define MUI_ICON klogg.ico

RequestExecutionLevel admin

Name "klogg-io"
Caption "klogg-io ${VERSION} Setup"

# Pages
!define MUI_WELCOMEPAGE_TITLE "Welcome to the klogg-io ${VERSION} Setup Wizard"
!define MUI_WELCOMEPAGE_TEXT "This wizard will guide you through the installation of klogg-io\
, a fast, advanced log explorer with serial support.$\r$\n$\r$\n\
klogg-io and the Qt libraries are released under the GPL, see \
the COPYING and NOTICE files.$\r$\n$\r$\n$_CLICK"
; MUI_FINISHPAGE_LINK_LOCATION "http://nsis.sf.net/"

!insertmacro MUI_PAGE_WELCOME
;!insertmacro MUI_PAGE_LICENSE "COPYING"
# !ifdef VER_MAJOR & VER_MINOR & VER_REVISION & VER_BUILD...
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_WELCOME
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES
!insertmacro MUI_UNPAGE_FINISH

# Languages
!insertmacro MUI_LANGUAGE "English"

# Installer sections
Section "klogg-io" klogg-io
    ; Prevent this section from being unselected
    SectionIn RO

    SetOutPath $INSTDIR
    File release\klogg-io.exe
    ;File release\klogg_crashpad_handler.exe
    ;File release\klogg_minidump_dump.exe
    File release\klogg_tbbmalloc.dll
    File release\klogg_tbbmalloc_proxy.dll

    File COPYING
    File NOTICE
    File README.md
    File DOCUMENTATION.md
    File release\documentation.html

    ; Create the 'sendto' link
    CreateShortCut "$SENDTO\klogg-io.lnk" "$INSTDIR\klogg-io,exe" "" "$INSTDIR\klogg-io.exe" 0

    ; Register as an otion (but not main handler) for some files (.txt, .Log, .cap)
    WriteRegStr HKCR "Applications\klogg-io.exe" "" ""
    WriteRegStr HKCR "Applications\klogg-io.exe\shell" "" "open"
    WriteRegStr HKCR "Applications\klogg-io.exe\shell\open" "Klogg-io log viewer" "klogg"
    WriteRegStr HKCR "Applications\klogg-io.exe\shell\open\command" "" '"$INSTDIR\klogg-io.exe" "%1"'
    WriteRegStr HKCR "*\OpenWithList\klogg-io.exe" "" ""
    WriteRegStr HKCR ".txt\OpenWithList\klogg-io.exe" "" ""
    WriteRegStr HKCR ".Log\OpenWithList\klogg-io.exe" "" ""
    WriteRegStr HKCR ".cap\OpenWithList\klogg-io.exe" "" ""

    ; Register uninstaller
    WriteRegExpandStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\klogg-io"\
"UninstallString" '"$INSTDIR\Uninstall.exe"'
    WriteRegExpandStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\klogg-io"\
"InstallLocation" "$INSTDIR"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\klogg-io" "DisplayName" "klogg-io"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\klogg-io" "DisplayVersion" "${VERSION}"
    WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\klogg-io" "NoModify" "1"
    WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\klogg-io" "NoRepair" "1"

    ; Create uninstaller
    WriteUninstaller "$INSTDIR\Uninstall.exe"
SectionEnd

Section "Qt5 Runtime libraries" qtlibs
    SetOutPath $INSTDIR
    File release\Qt5Core.dll
    File release\Qt5Gui.dll
    File release\Qt5Network.dll
    File release\Qt5SerialPort.dll
    File release\Qt5Widgets.dll
    File release\Qt5Concurrent.dll
    File release\Qt5Xml.dll
    SetOutPath $INSTDIR\platforms
    File release\platforms\qwindows.dll
    SetOutPath $INSTDIR\styles
    File release\styles\qwindowsvistastyle.dll
SectionEnd

Section "MSVC Runtime libraries" vcruntime
    SetOutPath $INSTDIR
    File release\msvcp140.dll
    File release\vcruntime140.dll
    
!if ${PLATFORM} == "x64"
    File release\vcruntime140_1.dll
!endif

!if ${PLATFORM} == "x64"
    File release\libcrypto-1_1-x64.dll
    File release\libssl-1_1-x64.dll
!else
    File release\libcrypto-1_1.dll
    File release\libssl-1_1.dll
!endif

SectionEnd

Section "Create Start menu shortcut" shortcut
    SetShellVarContext all
    CreateShortCut "$SMPROGRAMS\klogg-io.lnk" "$INSTDIR\klogg-io.exe" "" "$INSTDIR\klogg-io.exe" 0
SectionEnd

Section /o "Associate with .log files" associate
    ${registerExtension} "$INSTDIR\klogg-io.exe" ".log" "Log file"
SectionEnd

# Descriptions
!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
    !insertmacro MUI_DESCRIPTION_TEXT ${klogg-io} "The core files required to use klogg-io."
    !insertmacro MUI_DESCRIPTION_TEXT ${qtlibs} "Needed by klogg-io, you have to install these unless \
you already have the Qt5 development kit installed."
    !insertmacro MUI_DESCRIPTION_TEXT ${vcruntime} "Needed by klogg-io, you have to install these unless \
you already have the Microsoft Visual C++ 2017 Redistributable installed."
    !insertmacro MUI_DESCRIPTION_TEXT ${shortcut} "Create a shortcut in the Start menu for klogg-io."
    !insertmacro MUI_DESCRIPTION_TEXT ${associate} "Make klogg-io the default viewer for .log files."
!insertmacro MUI_FUNCTION_DESCRIPTION_END

# Uninstaller
Section "Uninstall"
    Delete "$INSTDIR\Uninstall.exe"

    Delete "$INSTDIR\klogg-io.exe"
    ;Delete "$INSTDIR\klogg_crashpad_handler.exe"
    ;Delete "$INSTDIR\klogg_minidump_dump.exe"
    Delete "$INSTDIR\README.md"
    Delete "$INSTDIR\COPYING"
    Delete "$INSTDIR\NOTICE"
    Delete "$INSTDIR\readme.html"
    Delete "$INSTDIR\documentation.md"
    Delete "$INSTDIR\documentation.html"
    Delete "$INSTDIR\libstdc++-6.dll"
    Delete "$INSTDIR\libgcc_s_seh-1.dll"
    Delete "$INSTDIR\libgcc_s_dw2-1.dll"
    Delete "$INSTDIR\Qt5Widgets.dll"
    Delete "$INSTDIR\Qt5Core.dll"
    Delete "$INSTDIR\Qt5Gui.dll"
    Delete "$INSTDIR\Qt5Network.dll"
    Delete "$INSTDIR\Qt5SerialPort.dll"
    Delete "$INSTDIR\Qt5Concurrent.dll"
    Delete "$INSTDIR\Qt5Xml.dll"
    Delete "$INSTDIR\platforms\qwindows.dll"
    Delete "$INSTDIR\platforms\qminimal.dll"
    Delete "$INSTDIR\styles\qwindowsvistastyle.dll"
    Delete "$INSTDIR\msvcp140.dll"
    Delete "$INSTDIR\vcruntime140.dll"
    Delete "$INSTDIR\vcruntime140_1.dll"
    Delete "$INSTDIR\tbbmalloc.dll"
    Delete "$INSTDIR\tbbmalloc_proxy.dll"
    Delete "$INSTDIR\klogg_tbbmalloc.dll"
    Delete "$INSTDIR\klogg_tbbmalloc_proxy.dll"
    Delete "$INSTDIR\libcrypto-1_1-x64.dll"
    Delete "$INSTDIR\libssl-1_1-x64.dll"
    Delete "$INSTDIR\libcrypto-1_1.dll"
    Delete "$INSTDIR\libssl-1_1.dll"
    RMDir "$INSTDIR"

    ; Remove settings in %appdata%
    Delete "$APPDATA\klogg-io\klogg-io.ini"
    RMDir "$APPDATA\klogg-io"

    DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\klogg-io"

    ; Remove the file associations
    ${unregisterExtension} ".log" "Log file"

    DeleteRegKey HKCR "*\OpenWithList\klogg-io.exe"
    DeleteRegKey HKCR ".txt\OpenWithList\klogg-io.exe"
    DeleteRegKey HKCR ".Log\OpenWithList\klogg-io.exe"
    DeleteRegKey HKCR ".cap\OpenWithList\klogg-io.exe"
    DeleteRegKey HKCR "Applications\klogg-io.exe\shell\open\command"
    DeleteRegKey HKCR "Applications\klogg-io.exe\shell\open"
    DeleteRegKey HKCR "Applications\klogg-io.exe\shell"
    DeleteRegKey HKCR "Applications\klogg-io.exe"

    ; Remove the shortcut, if any
    SetShellVarContext all
    Delete "$SMPROGRAMS\klogg-io.lnk"
SectionEnd
