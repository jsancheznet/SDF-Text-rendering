@echo off

pushd build

REM make Shift-f5 run the game!

REM TODO LSP NEXT
REM TODO GO TO DEFINITION
REM TODO GO TO DECLARATION
REM FIND ALL USAGES/REFERENCES
REM BUILD WITH ONE KEY
REM OPEN RADDBG WITH ONE KEY AND RUN IT?
REM TODO COPY ASSETS TO BUILD DIRECTORY
REM Make it so we can call build.bat from any directory inside the project! (Maybe use projectile?)
REM PUSHD, POPD always start at base directory

set SDLInclude=-I"..\libraries\SDL3-3.2.10\include"
set GladInclude=-I"..\libraries\glad\include"

set IncludeDirectories= %SDLInclude% %GladInclude%

REM Build glad
REM clang-cl ..\libraries\glad\src\glad.c %GladInclude%
clang-cl ..\src\main.cpp ..\libraries\glad\src\glad.c %IncludeDirectories% /link /LIBPATH:"W:\Projects\Something\libraries\SDL3-3.2.10\build\Debug" -SUBSYSTEM:WINDOWS SDL3.lib

popd
