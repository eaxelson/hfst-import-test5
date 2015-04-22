cl /EHsc /LD /DOPENFSTEXPORT /D_MSC_VER /DWINDOWS /DWIN32 /Feopenfst.dll /I ..\include\ compat.cpp flags.cpp fst.cpp properties.cpp symbol-table.cpp util.cpp
lib.exe /OUT:openfst.lib compat.obj flags.obj fst.obj properties.obj symbol-table.obj util.obj
