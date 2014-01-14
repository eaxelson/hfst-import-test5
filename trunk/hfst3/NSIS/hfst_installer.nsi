!include LogicLib.nsh
!include StrRep.nsh
!include ReplaceInFile.nsh

outfile "install-FOO-XX-bit.exe"

section

	## Tell the user what is going to be installed 
	## -------------------------------------------
	
	messageBox MB_OK "This program will install HFST command line tools locally to a chosen directory. After installation, you can use the functionalities by opening Command Prompt and moving to that directory. There you will also find a README.txt file that gives more information on the functionalities and some simple examples."


	## Define the installation directory
	## ---------------------------------

	nsDialogs::SelectFolderDialog "Select a directory where HFST functionalities will be installed" "c:\HFST"
	Pop $0
	messageBox MB_OK "Installing HFST to directory: $0."
	setOutPath $0


!include CheckLibraries.nsi


	## Install the README file
	## -----------------------

	File README.hfst.txt README.txt


	## Install tagger tools
	## --------------------

	${If} ${FileExists} "C:\Python27\Lib\site-packages"
	      setOutPath "C:\Python27\Lib\site-packages\"
	${Else}
              ${If} ${FileExists} "C:\Python26\Lib\site-packages"
	                      setOutPath "C:\Python26\Lib\site-packages\"
              ${Else}
	                      messageBox MB_OK "Did not find Python, so omitting hfst-train-tagger tool"
              ${EndIf}
	${EndIf}

        ${If} $OUTDIR == $0
              File hfst-tag.exe
        ${Else}
              File tagger_aux.py
              setOutPath $0
              File hfst-tag.exe
              # Use installation directory in scripts
              !insertmacro _ReplaceInFile hfst-train-tagger.bat HFST_INSTALLATION_DIRECTORY $0
	      File hfst-open-input-file-for-tagger.exe
	      File hfst_tagger_compute_data_statistics.py
	      File hfst-build-tagger.exe
	      File hfst-train-tagger.bat              
        ${EndIf}


	## Install libhfst dll and HFST command line tools
	## -----------------------------------------------

!include AddHfstLibrary.nsi

        File hfst-affix-guessify.exe
        File hfst-calculate.exe
        File hfst-compare.exe
        File hfst-compose.exe
        File hfst-compose-intersect.exe
        File hfst-concatenate.exe
        File hfst-conjunct.exe
        File hfst-determinize.exe
        File hfst-disjunct.exe
        File hfst-edit-metadata.exe
        File hfst-format.exe
        File hfst-fst2fst.exe
        File hfst-fst2strings.exe
        File hfst-fst2txt.exe
        File hfst-grep.exe
        File hfst-guess.exe
        File hfst-guessify.exe
        File hfst-head.exe
        File hfst-info.exe
        File hfst-invert.exe
        File hfst-lexc.exe
        File hfst-lexc2fst.exe
        File hfst-lookup.exe
        File hfst-pair-test.exe
        File hfst-minimize.exe
        File hfst-multiply.exe
        File hfst-name.exe
        File hfst-optimized-lookup.exe
        File hfst-pmatch.exe
        File hfst-pmatch2fst.exe
        File hfst-project.exe
        File hfst-push-weights.exe
        File hfst-regexp2fst.exe
        File hfst-remove-epsilons.exe
        File hfst-repeat.exe
        File hfst-reverse.exe
        File hfst-reweight.exe
        File hfst-shuffle.exe
        File hfst-split.exe
        File hfst-strings2fst.exe
        File hfst-substitute.exe
        File hfst-subtract.exe
        File hfst-summarize.exe
        File hfst-tail.exe
        File hfst-txt2fst.exe
        File hfst-xfst.exe

	File htwolcpre1.exe
	File htwolcpre2.exe
	File htwolcpre3.exe
	File hfst-twolc.bat

        # Use installation directory in scripts
        !insertmacro _ReplaceInFile hfst-twolc.bat HFST_INSTALLATION_DIRECTORY $0

	# Install hfst command line script

	File hfst.bat hfst.bat

        !insertmacro _ReplaceInFile hfst.bat HFST_INSTALLATION_DIRECTORY $0
        !insertmacro _ReplaceInFile hfst.bat HFST_WELCOME_MESSAGE "Welcome to the HFST interface!"

	messageBox MB_OK "Installation complete. HFST functionalities are in directory $0."

        messageBox MB_YESNO "Do you want to create a shortcut to HFST tools in Start Menu?" IDYES true
        true:
		CreateDirectory "$SMPROGRAMS\HFST"
                CreateShortCut "$SMPROGRAMS\HFST\hfst.lnk" "$0\hfst.bat";
	
sectionEnd
