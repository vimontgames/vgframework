del *.lib
del *.ipdb
del *.iobj
del *.exp
del *.ilk
del *.pdb
del *.opt
del *.sln
del .gitattributes
del .gitignore
del TODO.txt
del clean.bat
rmdir .git /S /Q
rmdir .github /S /Q
rmdir doc /S /Q
rmdir extern /S /Q
rmdir src /S /Q
rmdir profile /S /Q
rmdir lib /S /Q
rmdir tmp /S /Q
rmdir .vs /S /Q
rmdir cache /S /Q
del extern/JoltPhysics/Build/VS2022_CL /S /Q
cd projects
cd game
rmdir src /S /Q
del *.exp
del *.lib
del *.ilk
del *.pdb
cd ..
cd ..
del package.bat
del build_all.bat

