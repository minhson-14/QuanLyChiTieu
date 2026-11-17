@echo off
REM Script bien dich nhanh tren Windows bang g++ (MinGW-w64)
REM Dat console ve UTF-8 de hien thi tieng Viet co dau
chcp 65001 >nul

echo Dang bien dich...
REM -static giup file .exe chay doc lap, khong can DLL cua MinGW di kem
g++ -std=c++17 -Wall -Wextra -static -static-libgcc -static-libstdc++ -Iinclude src\*.cpp main.cpp -o QuanLyChiTieu.exe

if %errorlevel%==0 (
    echo.
    echo Build thanh cong: QuanLyChiTieu.exe
    echo Chay chuong trinh bang lenh: QuanLyChiTieu.exe
) else (
    echo.
    echo Build that bai. Vui long kiem tra lai loi bien dich o tren.
)
