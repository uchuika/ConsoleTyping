@echo off
SET OutDir=%~1
SET Base=%~dp0

xcopy "%Base%External\SDL2-2.32.8\lib\x64\SDL2.dll" "%OutDir%" /y
xcopy "%Base%External\SDL2-2.32.8\SDL2_image-2.8.8\lib\x64\SDL2_image.dll" "%OutDir%" /y
xcopy "%Base%External\SDL2-2.32.8\SDL2_ttf-2.24.0\lib\x64\SDL2_ttf.dll" "%OutDir%" /y
xcopy "%Base%External\glew-2.3.1\bin\Release\x64\glew32.dll" "%OutDir%" /y
exit /b 0
