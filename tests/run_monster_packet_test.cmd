@echo off
where cl >nul 2>nul
if errorlevel 1 (
  echo Run from a Visual Studio x64 Native Tools Command Prompt.
  exit /b 1
)
pushd "%~dp0"
cl /nologo /EHsc /utf-8 /Gy /MD /I ..\guakmario monster_packet_test.cpp ..\guakmario\network.cpp /Fe:monster_packet_test.exe /link /OPT:REF /INCREMENTAL:NO
if errorlevel 1 (
  popd
  exit /b 1
)
monster_packet_test.exe
set test_result=%errorlevel%
popd
exit /b %test_result%
