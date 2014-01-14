
        ## Check that kernel32 and msvcrt dlls are found
        ## ---------------------------------------------

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


        ## Check if libstdc++ and libgcc dlls are found and, if needed
        ## install them to the HFST directory
        ## -----------------------------------------------------------

        SearchPath $R0 "libstdc++-6.dll"
        ${If} $R0 == ""
              File libstdc++-6.dll
        ${EndIf}

        # SearchPath $R0 "libgcc_s_dw2-1.dll"
        SearchPath $R0 "libgcc_s_seh-1.dll"
        ${If} $R0 == ""
              # File libgcc_s_dw2-1.dll
              File libgcc_s_seh-1.dll
        ${EndIf}

