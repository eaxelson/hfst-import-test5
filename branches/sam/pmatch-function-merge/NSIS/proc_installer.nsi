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
	
	messageBox MB_OK "This program will install hfst-proc locally to a chosen directory. After installation, you can use the functionalities by opening Command Prompt and moving to that directory. There you will also find a README.txt file that gives more information on the functionalities and some simple examples."


	## Define the installation directory
	## ---------------------------------

	nsDialogs::SelectFolderDialog "Select a directory where hfst-proc will be installed" "C:\HFST-PROC"
	Pop $0
	messageBox MB_OK "Installing hfst-proc to directory: $0."
	setOutPath $0


!include CheckLibraries.nsi


	## Install the README file
	## -----------------------

	File README.proc.txt
        !insertmacro MoveFile README.proc.txt README.txt

	## Install libhfst dll and hfst-proc
	## ---------------------------------

!include AddHfstLibrary.nsi

        File hfst-proc.exe

	# Install hfst command line script

	File hfst.bat

        !insertmacro _ReplaceInFile hfst.bat HFST_INSTALLATION_DIRECTORY $0
        !insertmacro _ReplaceInFile hfst.bat HFST_WELCOME_MESSAGE "Welcome to the hfst-proc directory!"

        !insertmacro MoveFile hfst.bat hfst-proc-window.bat

	messageBox MB_OK "Installation complete. The tool hfst-proc is in directory $0."

        messageBox MB_YESNO "Do you want to create a shortcut to hfst-proc in Start Menu?" IDYES true
        true:
		CreateDirectory "$SMPROGRAMS\HFST-PROC"
                CreateShortCut "$SMPROGRAMS\HFST-PROC\hfst-proc.lnk" "$0\hfst-proc-window.bat";
	
sectionEnd
