!include LogicLib.nsh
!include EnvVarUpdate.nsh
!include StrRep.nsh
!include ReplaceInFile.nsh
# !include x64.nsh


outfile "HfstInstaller.exe"

section

	# Tell the user what is going to be installed 
	# -------------------------------------------
	
	messageBox MB_OK "This program will install HFST command line tools and Python/Swig bindings locally to a chosen directory. After installation, you can use the functionalities by opening Command Prompt and moving to that directory. There you will also find a README file that gives more information on the functionalities and some simple examples."


	# Define the installation directory
	# ---------------------------------

	nsDialogs::SelectFolderDialog "Select a directory where HFST functionalities will be installed" "c:\HFST"
	Pop $0
	messageBox MB_OK "Installing HFST to directory: $0."
	setOutPath $0


	# Check that kernel32 and msvcrt dlls are found
	# ---------------------------------------------

	SearchPath $R0 "kernel32.dll"
	${If} $R0 == ""
	      messageBox MB_OK "ERROR: kernel32.dll not found."
	      return
	${EndIf}

	SearchPath $R0 "msvcrt.dll"
	${If} $R0 == ""
	      messageBox MB_OK "ERROR: msvcrt.dll not found."
	      return
	${EndIf}


	# Check if libstdc++ and libgcc dlls are found and, if needed
	# install them to the HFST directory
	# -----------------------------------------------------------

	SearchPath $R0 "libstdc++-6.dll"
	${If} $R0 == ""
	      File libstdc++-6.dll
	${EndIf}

	SearchPath $R0 "libgcc_s_dw2-1.dll"
	${If} $R0 == ""
	      File libgcc_s_dw2-1.dll
	${EndIf}


	# Install the README file
	# -----------------------

	File README


	# Install Swig/Python bindings
	# ----------------------------

	${If} ${FileExists} "C:\Python27\Lib\site-packages"
	      messageBox MB_OK "Found Python directory, installing Swig/Python bindings there (to C:\Python27\Lib\site-packages)."
	      setOutPath "C:\Python27\Lib\site-packages\"
	${Else}
	      messageBox MB_OK "Did not find Python, so installing Swig/Python bindings only locally (to $0)."	
	${EndIf}

	File _libhfst.pyd
	File libhfst.py
        File tagger_aux.py
        # rest of tagger utilities are installed to $0

	setOutPath $0


	# Install libhfst dll and HFST command line tools
	# -----------------------------------------------

	File libhfst-31.dll

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

        !insertmacro _ReplaceInFile hfst-train-tagger.bat HFST_INSTALLATION_DIRECTORY $0

	File hfst-open-input-file-for-tagger.exe
	File hfst_tagger_compute_data_statistics.py
	# tagger_aux.py is in the same location as python (version 2) bindings
	File hfst-build-tagger.exe
	File hfst-train-tagger.bat
	File hfst-tag.exe

        !insertmacro _ReplaceInFile hfst-twolc.bat HFST_INSTALLATION_DIRECTORY $0

	File htwolcpre1.exe
	File htwolcpre2.exe
	File htwolcpre3.exe
	File hfst-twolc.bat

	messageBox MB_OK "Installation complete. HFST functionalities are in directory $0."


	# Add HFST directory to the PATH environment variable
	# ---------------------------------------------------

	${EnvVarUpdate} $1 "PATH" "A" "HKCU" "$0"
	#  messageBox MB_OK "Test: path is: $1."
	

sectionEnd
