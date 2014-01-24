!include LogicLib.nsh
!include EnvVarUpdate.nsh
!include StrRep.nsh
!include ReplaceInFile.nsh
# !include x64.nsh


outfile "HfstPythonInstaller.exe"

section

	## Tell the user what is going to be installed 
	## -------------------------------------------
	
	messageBox MB_OK "This program will install HFST Python bindings."


	## Define the installation directory
	## ---------------------------------

	nsDialogs::SelectFolderDialog "Select a directory where HFST functionalities will be installed" "c:\HFST"
	Pop $0
	messageBox MB_OK "Installing HFST to directory: $0."
	setOutPath $0

!include CheckLibraries.nsi

	## Install the README file
	## -----------------------

	File README


	## Install Python bindings and tagger aux module
	## ---------------------------------------------

        #  Python version 2
        #  ----------------

	${If} ${FileExists} "C:\Python27\Lib\site-packages"
	      messageBox MB_OK "Found Python directory, installing Python bindings there (to C:\Python27\Lib\site-packages)."
	      setOutPath "C:\Python27\Lib\site-packages\"
	${Else}
              ${If} ${FileExists} "C:\Python26\Lib\site-packages"
                    	      messageBox MB_OK "Found Python directory, installing Python bindings there (to C:\Python26\Lib\site-packages)."
	                      setOutPath "C:\Python26\Lib\site-packages\"
              ${Else}
	                      messageBox MB_OK "Did not find Python, so installing Python bindings only locally (to $0)."
              ${EndIf}
	${EndIf}

        ${If} $OUTDIR == $0
              File python2-_libhfst.pyd
              File python2-libhfst.py
        ${Else}
        # Remove prefix 'python2-' if installation is not local 
              File /oname="_libhfst.pyd" python2-_libhfst.pyd 
	      File /oname="libhfst.py" python2-libhfst.py
        ${EndIf}

        File tagger_aux.py

        # rest of tagger utilities are installed to $0

	setOutPath $0

        #  Python version 3
        #  ----------------

	${If} ${FileExists} "C:\Python33\Lib\site-packages"
	      messageBox MB_OK "Found Python directory, installing Python bindings there (to C:\Python33\Lib\site-packages)."
	      setOutPath "C:\Python33\Lib\site-packages\"
	${Else}
              ${If} ${FileExists} "C:\Python32\Lib\site-packages"
                    	      messageBox MB_OK "Found Python directory, installing Python bindings there (to C:\Python32\Lib\site-packages)."
	                      setOutPath "C:\Python32\Lib\site-packages\"
              ${Else}
	                      messageBox MB_OK "Did not find Python, so installing Python bindings only locally (to $0)."
              ${EndIf}
	${EndIf}

        ${If} $OUTDIR == $0
              File python3-_libhfst.pyd
              File python3-libhfst.py
        ${Else}
        # Remove prefix 'python3-' if installation is not local 
              File /oname="_libhfst.pyd" python3-_libhfst.pyd 
	      File /oname="libhfst.py" python3-libhfst.py
        ${EndIf}

        setOutPath $0


	## Install libhfst dll and HFST command line tools
	## -----------------------------------------------

	File libhfst-32.dll

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

        # Use installation directory in scripts
        !insertmacro _ReplaceInFile hfst-train-tagger.bat HFST_INSTALLATION_DIRECTORY $0

	File hfst-open-input-file-for-tagger.exe
	File hfst_tagger_compute_data_statistics.py
	# tagger_aux.py is in the same location as python (version 2) bindings
	File hfst-build-tagger.exe
	File hfst-train-tagger.bat
	File hfst-tag.exe

        # Use installation directory in scripts
        !insertmacro _ReplaceInFile hfst-twolc.bat HFST_INSTALLATION_DIRECTORY $0

	File htwolcpre1.exe
	File htwolcpre2.exe
	File htwolcpre3.exe
	File hfst-twolc.bat

        # Use installation directory in scripts
        !insertmacro _ReplaceInFile hfst.bat HFST_INSTALLATION_DIRECTORY $0

        File hfst.bat

	messageBox MB_OK "Installation complete. HFST functionalities are in directory $0."

        messageBox MB_YESNO "Do you want to create a shortcut to HFST tools?" IDYES true
        # Use installation directory in scripts
        !insertmacro _ReplaceInFile hfst.bat HFST_INSTALLATION_DIRECTORY $0

        File hfst.bat

        messageBox MB_OK "Installation complete. HFST functionalities are in directory $0."

        messageBox MB_YESNO "Do you want to create a shortcut to HFST tools?" IDYES true
        true:
                CreateShortCut "$SMPROGRAMS\HFST\hfst.lnk" $0"\hfst.bat";
        true:
                CreateShortCut "$SMPROGRAMS\HFST\hfst.lnk" $0"\hfst.bat";
        


	## Add HFST directory to the PATH environment variable
	## ---------------------------------------------------

	# ${EnvVarUpdate} $1 "PATH" "A" "HKCU" "$0"
	#  messageBox MB_OK "Test: path is: $1."
	

sectionEnd
