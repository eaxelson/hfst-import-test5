!include LogicLib.nsh

outfile "HfstInstaller.exe"

section


	# Define the installation directory
	# ---------------------------------

	nsDialogs::SelectFolderDialog "Select directory where HFST tools will be installed" "c:\HFST"
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


	# Install libhfst dll and HFST command line tools
	# -----------------------------------------------

	File libhfst-21.dll

	File hfst-affix-guessify.exe
	File hfst-calculate.exe
	File hfst-compare.exe
	File hfst-compose.exe
	File hfst-compose-intersect.exe
	File hfst-concatenate.exe
	File hfst-conjunct.exe
	File hfst-determinize.exe
	File hfst-disjunct.exe
	File hfst-duplicate.exe
	File hfst-edit-metadata.exe
	File hfst-expand-equivalences.exe
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
	File hfst-name.exe
	File hfst-optimized-lookup.exe
	File hfst-pmatch.exe
	File hfst-pmatch2fst.exe
	File hfst-preprocess-for-optimized-lookup-format.exe
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
	File hfst-strip-header.exe
	File hfst-substitute.exe
	File hfst-subtract.exe
	File hfst-summarize.exe
	File hfst-tail.exe
	#   -- File hfst-train-tagger.exe
	File hfst-traverse.exe
	#   -- File hfst-twolc.exe
	File hfst-txt2fst.exe

	
sectionEnd

