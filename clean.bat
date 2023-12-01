del *.exe
del *.lib
ren dxcompiler.dll dxcompiler.dll.bak
ren dxil.dll dxil.dll.bak
ren WinPixEventRuntime.dll WinPixEventRuntime.dll.bak
ren libfbxsdk.dll libfbxsdk.dll.bak
del *.dll
ren dxcompiler.dll.bak dxcompiler.dll
ren dxil.dll.bak dxil.dll
ren WinPixEventRuntime.dll.bak WinPixEventRuntime.dll
ren libfbxsdk.dll.bak libfbxsdk.dll
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
rmdir cache /S /Q

cd projects
cd game
del *.exp
del *.lib
del *.dll
del *.ilk
del *.pdb
cd ..
cd ..