!include LogicLib.nsh

outfile "HfstInstaller.exe"

section

	nsDialogs::SelectFolderDialog "Select directory" "c:\HFST"
	Pop $0
	messageBox MB_OK "Installing to directory: $0."

	setOutPath $0

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

					  SearchPath $R0 "libstdc++-6.dll"
					  ${If} $R0 == ""
					  ${OrIf} $R0 == "Z:\NSIS\libstdc++-6.dll"
					  	  File libstdc++-6.dll
						  ${Else}
							messageBox MB_OK "libstdc... found in $R0."
							${EndIf}

							SearchPath $R0 "libgcc_s_dw2-1.dll"
							${If} $R0 == ""
							${OrIf} $R0 == "Z:\NSIS\libgcc_s_dw2-1.dll"
								File libgcc_s_dw2-1.dll
								${EndIf}

								File libhfst-21.dll
								File hfst-calculate.exe
								File hfst-fst2strings.exe

								
								
sectionEnd
