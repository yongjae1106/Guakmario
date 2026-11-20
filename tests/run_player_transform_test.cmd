@echo off
where cl >nul 2>nul
if errorlevel 1 (
  echo Run from a Visual Studio x64 Native Tools Command Prompt.
  exit /b 1
)
pushd "%~dp0"
cl /nologo /EHsc /utf-8 /UNDEBUG /MD /I ..\guakmario player_transform_test.cpp ..\guakmario\transform.cpp /Fe:player_transform_test.exe
if errorlevel 1 (
  popd
  exit /b 1
)
player_transform_test.exe
set test_result=%errorlevel%
popd
exit /b %test_result%
