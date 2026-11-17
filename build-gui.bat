@echo off
REM ============================================================
REM  Build PHIEN BAN GIAO DIEN Qt (QuanLyChiTieuGUI.exe)
REM  Yeu cau: da cai Qt 6.8.3 (mingw_64) va MinGW 13.1.0 tai C:\Qt
REM  (cai bang: pip install aqtinstall; aqt install-qt ...)
REM ============================================================
chcp 65001 >nul

set "QT_DIR=C:\Qt\6.8.3\mingw_64"
set "MINGW_BIN=C:\Qt\Tools\mingw1310_64\bin"
set "PATH=%MINGW_BIN%;%PATH%"

if not exist "%QT_DIR%\bin\qmake.exe" (
    echo [LOI] Khong tim thay Qt tai %QT_DIR%
    echo Vui long cai Qt 6.8.3 mingw_64 truoc khi build ban GUI.
    goto :eof
)

echo === Cau hinh CMake ===
cmake -S . -B build-gui -G "MinGW Makefiles" ^
  -DCMAKE_PREFIX_PATH="%QT_DIR%" ^
  -DCMAKE_CXX_COMPILER="%MINGW_BIN%/g++.exe" ^
  -DCMAKE_MAKE_PROGRAM="%MINGW_BIN%/mingw32-make.exe"
if errorlevel 1 goto :fail

echo === Bien dich ===
cmake --build build-gui -j
if errorlevel 1 goto :fail

echo === Sao chep DLL Qt de chay doc lap ===
"%QT_DIR%\bin\windeployqt.exe" --compiler-runtime --no-translations build-gui\QuanLyChiTieuGUI.exe

echo.
echo === Build GUI THANH CONG ===
echo Chay chuong trinh: build-gui\QuanLyChiTieuGUI.exe
goto :eof

:fail
echo.
echo === BUILD THAT BAI - kiem tra log loi o tren ===
