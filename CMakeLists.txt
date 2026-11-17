# ============================================================
#  CMake build cho PHIEN BAN GIAO DIEN (Qt 6) cua phan mem
#  Quan ly chi tieu ca nhan.
#
#  Cach build (xem them README, muc "Ban giao dien Qt"):
#    cmake -S . -B build-gui -G "MinGW Makefiles" ^
#          -DCMAKE_PREFIX_PATH="C:/Qt/6.8.3/mingw_64"
#    cmake --build build-gui
# ============================================================
cmake_minimum_required(VERSION 3.16)
project(QuanLyChiTieuGUI LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Tim Qt6 (can dat CMAKE_PREFIX_PATH toi thu muc Qt mingw_64)
find_package(Qt6 REQUIRED COMPONENTS Widgets Network)
qt_standard_project_setup() # bat AUTOMOC/AUTOUIC/AUTORCC

# --- Backend OOP dung chung voi ban console (KHONG gom main.cpp console) ---
set(BACKEND_SOURCES
    src/TaiKhoan.cpp
    src/GiaoDich.cpp
    src/ThuNhap.cpp
    src/ChiTieu.cpp
    src/QuanLyChiTieu.cpp
    src/NguoiDung.cpp
    src/UserSession.cpp
    src/AuthService.cpp
    src/FirebaseService.cpp
    src/ConsoleHelper.cpp   # dung lai ham dinh dang tien dinhDangTien()
)

# --- Ma nguon rieng cho giao dien Qt ---
set(GUI_SOURCES
    gui/main_gui.cpp
    gui/LoginWindow.cpp
    gui/MainWindow.cpp
    gui/TransactionDialog.cpp
    gui/PieChartDialog.cpp
)

qt_add_executable(QuanLyChiTieuGUI WIN32 ${BACKEND_SOURCES} ${GUI_SOURCES})

target_include_directories(QuanLyChiTieuGUI PRIVATE include gui)
target_link_libraries(QuanLyChiTieuGUI PRIVATE Qt6::Widgets Qt6::Network)
