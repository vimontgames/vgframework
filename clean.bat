del *.exe
del *.lib
ren dxil.dll dxil.dll.bak
ren WinPixEventRuntime.dll WinPixEventRuntime.dll.bak
del *.dll
ren dxil.dll.bak dxil.dll
ren WinPixEventRuntime.dll.bak WinPixEventRuntime.dll
del *.ipdb
del *.iobj
del *.exp
del *.ilk
del *.pdb
del *.opt
rmdir capture /S /Q
rmdir lib /S /Q
rmdir tmp /S /Q
rmdir .vs /S /Q