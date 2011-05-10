!include LogicLib.nsh
!include nsDialogs.nsh

!define PRODUCT_NAME "QtQuick3D"
!define PRODUCT_VERSION "1.0-tp1"
!define PRODUCT_PUBLISHER "Nokia"
!define PRODUCT_WEB_SITE "http://doc.qt.nokia.com/qt-quick3d-snapshot/"
!define PRODUCT_DIR_REGKEY "Software\Microsoft\Windows\CurrentVersion\App Paths\qglinfo.exe"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"

; MUI 1.67 compatible ------
!include "MUI.nsh"

; MUI Settings
!define MUI_ABORTWARNING
!define MUI_ICON "src\scripts\qtquick3d.ico"
!define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\modern-uninstall.ico"
!define MUI_WELCOMEFINISHPAGE_BITMAP "src\scripts\win_installer_background.bmp"
!define MUI_WELCOMEPAGE_TITLE "${PRODUCT_NAME} Setup"
!define MUI_WELCOMEPAGE_TEXT "QtQuick3D will be installed into Qt ${QT_VERSION}, found in $INSTDIR.  If you do not want ${PRODUCT_NAME} installed there, click cancel below."

; Welcome page
!insertmacro MUI_PAGE_WELCOME
; License page
!insertmacro MUI_PAGE_LICENSE "src\scripts\LICENSE.LGPL.txt"
; Directory page
; Disable this since we only install where we detect the right Qt
; !insertmacro MUI_PAGE_DIRECTORY
; Instfiles page
!insertmacro MUI_PAGE_INSTFILES
!define MUI_FINISHPAGE_NOAUTOCLOSE
; Finish page
Page custom DisplayWelcomeMessage
!define MUI_FINISHPAGE_RUN $INSTDIR\quick3d\bin\qglinfo.exe
!define MUI_FINISHPAGE_RUN_TEXT "Run QtQuick3D QGLInfo program"
!define MUI_FINISHPAGE_LINK "QtQuick3D documentation"
!define MUI_FINISHPAGE_LINK_LOCATION $INSTDIR\quick3d\doc\html\index.html

!insertmacro MUI_PAGE_FINISH

; Uninstaller pages
!insertmacro MUI_UNPAGE_INSTFILES

; Language files
!insertmacro MUI_LANGUAGE "English"

; MUI end ------

; Where make install puts all the files
!define MK_INST_ROOT tmp

Name "${PRODUCT_NAME} ${PRODUCT_VERSION}"
OutFile "${PRODUCT_NAME}-${PRODUCT_VERSION}.exe"
InstallDir "$PROGRAMFILES\QtQuick3D"
InstallDirRegKey HKLM "${PRODUCT_DIR_REGKEY}" ""
ShowInstDetails show
ShowUnInstDetails show

; HKCU\Software\Trolltech\Versions\DefaultQtVersion REG_SZ "${QT_VERSION}"
; HKCU\Software\Trolltech\Versions\${QT_VERSION}\InstallDir REG_SZ "C:\Qt\${QT_VERSION}"

Function .onInit
  ReadRegStr $0 HKCU "Software\Trolltech\Versions" "DefaultQtVersion"
  ; MessageBox MB_OK "Qt version installed: $0"
  ${If} $0 != ${QT_VERSION}
    MessageBox MB_ICONEXCLAMATION "Please install Qt v${QT_VERSION} from http:\\qt.nokia.com\downloads before installing ${PRODUCT_NAME} ${PRODUCT_VERSION}"
    Abort
  ${EndIf}
  ReadRegStr $1 HKCU "Software\Trolltech\Versions\$0" "InstallDir"
  StrCpy $INSTDIR $1
  LogSet on
FunctionEnd

Function DisplayWelcomeMessage
        ; Create the temporary plugins directory
        InitPluginsDir

        ; Put the welcome message in the temporary plugins directory.
        ; The plugins directory is deleted when the installer quits.
        File "/oname=$PLUGINSDIR\windows-install.txt" "src\scripts\windows-install.txt"

        nsDialogs::Create 1018
        Pop $0

        nsDialogs::CreateControl /NOUNLOAD ${__NSD_Text_CLASS} ${DEFAULT_STYLES}|${WS_TABSTOP}|${ES_MULTILINE} ${__NSD_Text_EXSTYLE} 20 45 325 200 ''
        Pop $1

        CustomLicense::LoadFile "$PLUGINSDIR\windows-install.txt" $1
        nsDialogs::Show

FunctionEnd

Section "MainSection" SEC01
  SetOutPath "$INSTDIR\bin"
  SetOverwrite try
  File "${MK_INST_ROOT}\Qt\${QT_VERSION}\bin\qglinfo.exe"
  CreateDirectory "$SMPROGRAMS\QtQuick3D"
  CreateShortCut "$SMPROGRAMS\QtQuick3D\QtQuick3D.lnk" "$INSTDIR\quick3d\bin\qglinfo.exe"
  CreateShortCut "$DESKTOP\QtQuick3D.lnk" "$INSTDIR\quick3d\bin\qglinfo.exe"
  ; bin imports include lib mkspecs plugins quick3d
  SetOutPath "$INSTDIR\quick3d\bin"
  File /r "${MK_INST_ROOT}\Qt\${QT_VERSION}\bin\*.*"
  SetOutPath "$INSTDIR\quick3d\doc"
  File /r "doc\html"
  SetOutPath "$INSTDIR"
  File /r "${MK_INST_ROOT}\Qt\${QT_VERSION}\imports"
  File /r "${MK_INST_ROOT}\Qt\${QT_VERSION}\include"
  File /r "${MK_INST_ROOT}\Qt\${QT_VERSION}\lib"
  File /r "${MK_INST_ROOT}\Qt\${QT_VERSION}\mkspecs"
  File /r "${MK_INST_ROOT}\Qt\${QT_VERSION}\plugins"
  File /r "${MK_INST_ROOT}\Qt\${QT_VERSION}\quick3d"
SectionEnd

Section -AdditionalIcons
  SetOutPath $INSTDIR
  WriteIniStr "$INSTDIR\${PRODUCT_NAME}.url" "InternetShortcut" "URL" "${PRODUCT_WEB_SITE}"
  CreateShortCut "$SMPROGRAMS\QtQuick3D\Website.lnk" "$INSTDIR\${PRODUCT_NAME}.url"
  CreateShortCut "$SMPROGRAMS\QtQuick3D\Uninstall.lnk" "$INSTDIR\uninst.exe"
SectionEnd

Section -Post
  WriteUninstaller "$INSTDIR\uninst.exe"
  WriteRegStr HKLM "${PRODUCT_DIR_REGKEY}" "" "$INSTDIR\Qt\${QT_VERSION}\bin\qglinfo.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" "$(^Name)"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\uninst.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayIcon" "$INSTDIR\Qt\${QT_VERSION}\bin\qglinfo.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "URLInfoAbout" "${PRODUCT_WEB_SITE}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "Publisher" "${PRODUCT_PUBLISHER}"
SectionEnd


Function un.onUninstSuccess
  HideWindow
  MessageBox MB_ICONINFORMATION|MB_OK "$(^Name) was successfully removed from your computer."
FunctionEnd

Function un.onInit
  MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "Are you sure you want to completely remove $(^Name) and all of its components?" IDYES +2
  Abort
FunctionEnd

Section Uninstall
  Delete "$INSTDIR\${PRODUCT_NAME}.url"
  Delete "$INSTDIR\uninst.exe"
  RMDir /r "$INSTDIR\plugins\sceneformats"
  RMDir /r "$INSTDIR\plugins\imageformats"
  Delete "$INSTDIR\mkspecs\features\qt3dquick.prf"
  Delete "$INSTDIR\mkspecs\features\qt3d.prf"
  Delete "$INSTDIR\lib\Qt3DQuick.*"
  Delete "$INSTDIR\lib\Qt3DQuickd.*"
  Delete "$INSTDIR\lib\Qt3D.*"
  Delete "$INSTDIR\lib\Qt3Dd.*"
  RMDir /r "$INSTDIR\include\Qt3D"
  RMDir /r "$INSTDIR\include\Qt3DQuick"
  Delete "$INSTDIR\Qt\${QT_VERSION}\bin\qglinfo.exe"

  Delete "$SMPROGRAMS\QtQuick3D\Uninstall.lnk"
  Delete "$SMPROGRAMS\QtQuick3D\Website.lnk"
  Delete "$DESKTOP\QtQuick3D.lnk"
  Delete "$SMPROGRAMS\QtQuick3D\QtQuick3D.lnk"

  RMDir "$SMPROGRAMS\QtQuick3D"
  RMDir /r "$INSTDIR\Qt\${QT_VERSION}\plugins\sceneformats"
  RMDir /r "$INSTDIR\Qt\${QT_VERSION}\plugins\imageformats"
  RMDir /r "$INSTDIR\Qt\${QT_VERSION}\include\Qt3DQuick"
  RMDir /r "$INSTDIR\Qt\${QT_VERSION}\include\Qt3D"
  RMDir /r "$INSTDIR\Qt\${QT_VERSION}\imports\Qt3D"

  DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
  DeleteRegKey HKLM "${PRODUCT_DIR_REGKEY}"
  SetAutoClose true
SectionEnd
