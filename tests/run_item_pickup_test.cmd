@echo off
where cl >nul 2>nul
if errorlevel 1 (
  echo Run from a Visual Studio x64 Native Tools Command Prompt.
  exit /b 1
)
pushd "%~dp0"
cl /nologo /EHsc /utf-8 /W4 /UNDEBUG /MD /I ..\guakmario item_pickup_test.cpp ..\guakmario\item_pickup.cpp /Fe:item_pickup_test.exe
if errorlevel 1 (
  popd
  exit /b 1
)
item_pickup_test.exe
set test_result=%errorlevel%
popd
exit /b %test_result%
