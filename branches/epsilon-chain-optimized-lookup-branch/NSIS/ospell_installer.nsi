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
	
	messageBox MB_OK "This program will install hfst-ospell locally to a chosen directory. After installation, you can use the functionalities by opening Command Prompt and moving to that directory. There you will also find a README.txt file that gives more information on the functionalities and some simple examples."


	## Define the installation directory
	## ---------------------------------

	nsDialogs::SelectFolderDialog "Select a directory where hfst-ospell will be installed" "C:\HFST-OSPELL"
	Pop $0
	messageBox MB_OK "Installing hfst-ospell to directory: $0."
	setOutPath $0


!include CheckLibraries.nsi

        ## Check for dlls
        ## --------------

        SearchPath $R0 "libbz2-2.dll"
        ${If} $R0 == ""
              File libbz2-2.dll
        ${EndIf}

        SearchPath $R0 "libiconv-2.dll"
        ${If} $R0 == ""
              File libiconv-2.dll
        ${EndIf}

        SearchPath $R0 "liblzma-5.dll"
        ${If} $R0 == ""
              File liblzma-5.dll
        ${EndIf}

        SearchPath $R0 "zlib1.dll"
        ${If} $R0 == ""
              File zlib1.dll
        ${EndIf}


	## Install the README file
	## -----------------------

	File README.ospell.txt
        !insertmacro MoveFile README.ospell.txt README.txt

	## Install hfst-ospell
	## -------------------

        File hfst-ospell.exe

	# Install hfst command line script

	File hfst.bat

        !insertmacro _ReplaceInFile hfst.bat HFST_INSTALLATION_DIRECTORY $0
        !insertmacro _ReplaceInFile hfst.bat HFST_WELCOME_MESSAGE "Welcome to the hfst-ospell directory!"

        !insertmacro MoveFile hfst.bat hfst-ospell-window.bat

	messageBox MB_OK "Installation complete. The tool hfst-ospell is in directory $0."

        messageBox MB_YESNO "Do you want to create a shortcut to hfst-ospell in Start Menu?" IDYES true
        true:
		CreateDirectory "$SMPROGRAMS\HFST-OSPELL"
                CreateShortCut "$SMPROGRAMS\HFST-OSPELL\hfst-ospell.lnk" "$0\hfst-ospell-window.bat";
	
sectionEnd
