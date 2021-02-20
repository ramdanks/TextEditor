; HM NIS Edit Wizard helper defines
!define PRODUCT_NAME "Memoriser"
!define PRODUCT_VERSION "1.0 Beta"
!define PRODUCT_PUBLISHER "Ramadhan Kalih Sewu, Yogie Wisesa, Suryanegara"
!define PRODUCT_WEB_SITE "https://github.com/ramdanks/TextEditor"
!define PRODUCT_DIR_REGKEY "Software\Microsoft\Windows\CurrentVersion\App Paths\memoriser.exe"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"

; MUI 1.67 compatible ------
!include "MUI.nsh"
!include "FileAssociation.nsh"

; MUI Settings
!define MUI_ABORTWARNING
!define MUI_ICON "${NSISDIR}\Contrib\Graphics\Icons\modern-install.ico"
!define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\modern-uninstall.ico"

; Language Selection Dialog Settings
!define MUI_LANGDLL_REGISTRY_ROOT "${PRODUCT_UNINST_ROOT_KEY}"
!define MUI_LANGDLL_REGISTRY_KEY "${PRODUCT_UNINST_KEY}"
!define MUI_LANGDLL_REGISTRY_VALUENAME "NSIS:Language"

; Welcome page
!insertmacro MUI_PAGE_WELCOME
; License page
!define MUI_LICENSEPAGE_RADIOBUTTONS
!insertmacro MUI_PAGE_LICENSE "..\LICENSE.txt"
; Directory page
!insertmacro MUI_PAGE_DIRECTORY
; Instfiles page
!insertmacro MUI_PAGE_INSTFILES
; Finish page
!define MUI_FINISHPAGE_RUN "$INSTDIR\memoriser.exe"
!insertmacro MUI_PAGE_FINISH
; Uninstaller pages
!insertmacro MUI_UNPAGE_INSTFILES

; Language files
!insertmacro MUI_LANGUAGE "English"
!insertmacro MUI_LANGUAGE "Indonesian"
!insertmacro MUI_LANGUAGE "Spanish"
!insertmacro MUI_LANGUAGE "Portuguese"
!insertmacro MUI_LANGUAGE "German"
!insertmacro MUI_LANGUAGE "Italian"
!insertmacro MUI_LANGUAGE "French"
!insertmacro MUI_LANGUAGE "Dutch"

; MUI end ------

Name "${PRODUCT_NAME} ${PRODUCT_VERSION}"
OutFile "Memoriser[1.0 Beta].exe"
InstallDir "$PROGRAMFILES64\Memoriser"
InstallDirRegKey HKLM "${PRODUCT_DIR_REGKEY}" ""
ShowInstDetails show
ShowUnInstDetails show

Function .onInit
  !insertmacro MUI_LANGDLL_DISPLAY
FunctionEnd

Section "MainSection" SEC01
  SetOutPath "$INSTDIR"
  SetOverwrite try
  File "..\Build\x64\Dist\Compressor.exe"
  File "..\Build\x64\Dist\memoriser.exe"
  File "..\LICENSE.txt"
  CreateDirectory "$SMPROGRAMS\Memoriser"
  CreateShortCut "$SMPROGRAMS\Memoriser\Memoriser.lnk" "$INSTDIR\memoriser.exe"
  CreateShortCut "$DESKTOP\Memoriser.lnk" "$INSTDIR\memoriser.exe"
  SetOutPath "$INSTDIR\dictionary"
  File "..\Resource\dictionary\Fruit.csv"
  File "..\Resource\dictionary\Nama.csv"
  SetOutPath "$INSTDIR\resource\img"
  File "..\Resource\img\gallery.bmp"
  File "..\Resource\img\save.bmp"
  File "..\Resource\img\unsave.bmp"
  SetOutPath "$INSTDIR\resource\lang"
  File "..\Resource\lang\bahasa.txt"
  File "..\Resource\lang\english.txt"
  File "..\Resource\lang\info.txt"
  SetOutPath "$INSTDIR\resource"
  File "..\Resource\splash.bmp"
SectionEnd

Section -AdditionalIcons
  SetOutPath $INSTDIR
  WriteIniStr "$INSTDIR\${PRODUCT_NAME}.url" "InternetShortcut" "URL" "${PRODUCT_WEB_SITE}"
  CreateShortCut "$SMPROGRAMS\Memoriser\Website.lnk" "$INSTDIR\${PRODUCT_NAME}.url"
  CreateShortCut "$SMPROGRAMS\Memoriser\Uninstall.lnk" "$INSTDIR\uninst.exe"
SectionEnd

Section -Post
  WriteUninstaller "$INSTDIR\uninst.exe"
  WriteRegStr HKLM "${PRODUCT_DIR_REGKEY}" "" "$INSTDIR\memoriser.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" "$(^Name)"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\uninst.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayIcon" "$INSTDIR\memoriser.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "URLInfoAbout" "${PRODUCT_WEB_SITE}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "Publisher" "${PRODUCT_PUBLISHER}"
  
  EnVar::SetHKLM
  EnVar::AddValue "path" "$INSTDIR"
  EnVar::SetHKCU
  EnVar::AddValue "path" "$INSTDIR"
 
  ${registerExtension} "$INSTDIR\memoriser.exe" ".mtx" "Memoriser Text"
  ${registerExtension} "$INSTDIR\memoriser.exe" ".txt" "Text Document"
SectionEnd


Function un.onUninstSuccess
  HideWindow
  MessageBox MB_ICONINFORMATION|MB_OK "$(^Name) was successfully removed from your computer."
FunctionEnd

Function un.onInit
!insertmacro MUI_UNGETLANGUAGE
  MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "Are you sure you want to completely remove $(^Name) and all of its components?" IDYES +2
  Abort
FunctionEnd

Section Uninstall
  Delete "$INSTDIR\${PRODUCT_NAME}.url"
  Delete "$INSTDIR\uninst.exe"
  Delete "$INSTDIR\resource\splash.bmp"
  Delete "$INSTDIR\resource\lang\info.txt"
  Delete "$INSTDIR\resource\lang\english.txt"
  Delete "$INSTDIR\resource\lang\bahasa.txt"
  Delete "$INSTDIR\resource\img\unsave.bmp"
  Delete "$INSTDIR\resource\img\save.bmp"
  Delete "$INSTDIR\resource\img\gallery.bmp"
  Delete "$INSTDIR\license\wxWidgets.txt"
  Delete "$INSTDIR\license\icon author.txt"
  Delete "$INSTDIR\license\app.txt"
  Delete "$INSTDIR\dictionary\Nama.csv"
  Delete "$INSTDIR\dictionary\Fruit.csv"
  Delete "$INSTDIR\memoriser.exe"
  Delete "$INSTDIR\Compressor.exe"
  Delete "$INSTDIR\config.cfg"

  Delete "$SMPROGRAMS\Memoriser\Uninstall.lnk"
  Delete "$SMPROGRAMS\Memoriser\Website.lnk"
  Delete "$DESKTOP\Memoriser.lnk"
  Delete "$SMPROGRAMS\Memoriser\Memoriser.lnk"

  RMDir "$SMPROGRAMS\Memoriser"
  RMDir "$INSTDIR\resource\lang"
  RMDir "$INSTDIR\resource\img"
  RMDir "$INSTDIR\resource"
  RMDir "$INSTDIR\license"
  RMDir "$INSTDIR\dictionary"
  RMDir "$INSTDIR\temp"
  RMDir "$INSTDIR\log"
  RMDir "$INSTDIR"

  DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
  DeleteRegKey HKLM "${PRODUCT_DIR_REGKEY}"
  SetAutoClose true
  
  EnVar::SetHKLM
  EnVar::DeleteValue "path" "$INSTDIR"
  EnVar::SetHKCU
  EnVar::DeleteValue "path" "$INSTDIR"
  
  ${unregisterExtension} ".mtx" "Memoriser Text"
  ${unregisterExtension} ".txt" "Text Document"
SectionEnd