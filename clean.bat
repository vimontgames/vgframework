del *.exe
del *.lib
ren dxil.dll dxil.dll.bak
del *.dll
ren dxil.dll.bak dxil.dll
del *.ipdb
del *.iobj
del *.exp
del *.ilk
del *.pdb
del *.opt
rmdir lib /S /Q
rmdir tmp /S /Q
rmdir .vs /S /Q