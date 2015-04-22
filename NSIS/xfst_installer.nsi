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
	
	messageBox MB_OK "This program will install hfst-xfst locally to a chosen directory. After installation, you can use the functionalities by opening Command Prompt and moving to that directory. There you will also find a README.txt file that gives more information on the functionalities and some simple examples."


	## Define the installation directory
	## ---------------------------------

	nsDialogs::SelectFolderDialog "Select a directory where hfst-xfst will be installed" "C:\HFST-XFST"
	Pop $0
	messageBox MB_OK "Installing hfst-xfst to directory: $0."
	setOutPath $0


!include CheckLibraries.nsi


	## Install the README file
	## -----------------------

	File README.xfst.txt
        !insertmacro MoveFile README.xfst.txt README.txt

	## Install libhfst dll and hfst-xfst
	## ---------------------------------

!include AddHfstLibrary.nsi

        File hfst-xfst.exe

	# Install hfst command line script

	File hfst.bat

        !insertmacro _ReplaceInFile hfst.bat HFST_INSTALLATION_DIRECTORY $0
        !insertmacro _ReplaceInFile hfst.bat HFST_WELCOME_MESSAGE "Welcome to the hfst-xfst directory!"

        !insertmacro MoveFile hfst.bat hfst-xfst-window.bat

	messageBox MB_OK "Installation complete. The tool hfst-xfst is in directory $0."

        messageBox MB_YESNO "Do you want to create a shortcut to hfst-xfst in Start Menu?" IDYES true
        true:
		CreateDirectory "$SMPROGRAMS\HFST-XFST"
                CreateShortCut "$SMPROGRAMS\HFST-XFST\hfst-xfst.lnk" "$0\hfst-xfst-window.bat";
	
sectionEnd
