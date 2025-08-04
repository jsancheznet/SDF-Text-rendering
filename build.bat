
@echo off

pushd build

set SDLInclude=-I"..\libraries\SDL3-3.2.10\include"
set GLADInclude=-I"..\libraries\glad\include"
set GLMInclude=-I"..\libraries\glm-1.0.1-light"
set STBInclude=-I"..\libraries\stb"
set JSONInclude=-I"..\libraries\json"

set IncludeDirectories= %SDLInclude% %GLADInclude% %GLMInclude% %STBInclude% %JSONInclude%

IF NOT EXIST "assets" MKDIR "assets"
COPY /Y "..\assets\*.*" "assets\"

IF NOT EXIST "shaders" MKDIR "shaders"
COPY /Y "..\shaders\*.*" "shaders\"

clang-cl /Zi ..\src\main.cpp ..\src\renderer.cpp ..\src\camera.cpp ..\src\mouse.cpp ..\libraries\glad\src\glad.c %GLADInclude%  %IncludeDirectories% /link /LIBPATH:"..\libraries\SDL3-3.2.10\build\Debug" -SUBSYSTEM:CONSOLE SDL3.lib

popd
