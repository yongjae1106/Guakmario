@echo off
where cl >nul 2>nul
if errorlevel 1 (
  echo Run from a Visual Studio x64 Native Tools Command Prompt.
  exit /b 1
)
pushd "%~dp0"
cl /nologo /EHsc /utf-8 /UNDEBUG /MD /I ..\guakmario bowser_combat_test.cpp ..\guakmario\collision.cpp ..\guakmario\data.cpp ..\guakmario\game.cpp ..\guakmario\input.cpp ..\guakmario\item.cpp ..\guakmario\item_pickup.cpp ..\guakmario\map.cpp ..\guakmario\monster.cpp ..\guakmario\network.cpp ..\guakmario\player.cpp ..\guakmario\shot.cpp ..\guakmario\sound.cpp ..\guakmario\transform.cpp /Fe:bowser_combat_test.exe
if errorlevel 1 (
  popd
  exit /b 1
)
bowser_combat_test.exe
set test_result=%errorlevel%
popd
exit /b %test_result%
