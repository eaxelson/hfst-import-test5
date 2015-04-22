!include LogicLib.nsh
!include StrRep.nsh
!include ReplaceInFile.nsh
!include 'FileFunc.nsh'
!insertmacro Locate

# Var /GLOBAL switch_overwrite
# StrCpy $switch_overwrite 0
!include 'MoveFileFolder.nsh'


outfile "install-FOO-XX-bit.exe"

section

	## Tell the user what is going to be installed 
	## -------------------------------------------
	
	messageBox MB_OK "This program will install hfst-twolc locally to a chosen directory. After installation, you can use the functionalities by opening Command Prompt and moving to that directory. There you will also find a README.txt file that gives more information on the functionalities and some simple examples."


	## Define the installation directory
	## ---------------------------------

	nsDialogs::SelectFolderDialog "Select a directory where hfst-twolc will be installed" "C:\HFST-TWOLC"
	Pop $0
	messageBox MB_OK "Installing hfst-twolc to directory: $0."
	setOutPath $0


!include CheckLibraries.nsi


	## Install the README file
	## -----------------------

	File README.twolc.txt
        !insertmacro MoveFile README.twolc.txt README.txt

	## Install libhfst dll and hfst-twolc
	## ---------------------------------

!include AddHfstLibrary.nsi

        # File hfst-twolc.exe

        File htwolcpre1.exe
        File htwolcpre2.exe
        File htwolcpre3.exe
        File hfst-twolc.bat

        # Use installation directory in scripts
        !insertmacro _ReplaceInFile hfst-twolc.bat HFST_INSTALLATION_DIRECTORY $0


	# Install hfst command line script

	File hfst.bat

        !insertmacro _ReplaceInFile hfst.bat HFST_INSTALLATION_DIRECTORY $0
        !insertmacro _ReplaceInFile hfst.bat HFST_WELCOME_MESSAGE "Welcome to the hfst-twolc directory!"

        !insertmacro MoveFile hfst.bat hfst-twolc-window.bat

	messageBox MB_OK "Installation complete. The tool hfst-twolc is in directory $0."

        messageBox MB_YESNO "Do you want to create a shortcut to hfst-twolc in Start Menu?" IDYES true
        true:
		CreateDirectory "$SMPROGRAMS\HFST-TWOLC"
                CreateShortCut "$SMPROGRAMS\HFST-TWOLC\hfst-twolc.lnk" "$0\hfst-twolc-window.bat";
	
sectionEnd
