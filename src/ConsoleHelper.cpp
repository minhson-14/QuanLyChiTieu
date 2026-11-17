#include "ConsoleHelper.h"

#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <cmath>

#ifdef _WIN32
#include <windows.h> // SetConsoleOutputCP, SetConsoleMode... (chi co tren Windows)
#endif

// Khoi tao bien tinh: mac dinh chua biet console co ho tro mau hay khong
bool ConsoleHelper::coTheMau = false;

void ConsoleHelper::thietLapConsole() {
#ifdef _WIN32
    // Dat bang ma UTF-8 cho ca output va input de hien thi/nhap tieng Viet co dau
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    // Bat che do xu ly chuoi ANSI (Virtual Terminal) de in mau
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut != INVALID_HANDLE_VALUE) {
        DWORD mode = 0;
        if (GetConsoleMode(hOut, &mode)) {
            DWORD modeMoi = mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            // Neu bat thanh cong -> console ho tro mau
            coTheMau = (SetConsoleMode(hOut, modeMoi) != 0);
        }
    }
#else
    // Tren moi truong khac Windows (Linux/macOS), gia su terminal ho tro ANSI
    coTheMau = true;
#endif
}

bool ConsoleHelper::hoTroMau() { return coTheMau; }

std::string ConsoleHelper::mauDo()    { return coTheMau ? "\033[31m" : ""; }
std::string ConsoleHelper::mauXanh()  { return coTheMau ? "\033[32m" : ""; }
std::string ConsoleHelper::mauVang()  { return coTheMau ? "\033[33m" : ""; }
std::string ConsoleHelper::resetMau() { return coTheMau ? "\033[0m"  : ""; }

void ConsoleHelper::inSoDu(double soDu) {
    if (soDu < 0) {
        if (coTheMau) {
            // Console ho tro mau -> in so du am mau do
            std::cout << mauDo() << dinhDangTien(soDu) << resetMau();
        } else {
            // Khong ho tro mau -> in binh thuong kem canh bao
            std::cout << dinhDangTien(soDu) << " (!) SO DU AM";
        }
    } else {
        std::cout << dinhDangTien(soDu);
    }
}

std::string ConsoleHelper::dinhDangTien(double soTien) {
    // Lam tron ve so nguyen (don vi VND thuong khong dung phan le)
    long long n = static_cast<long long>(std::llround(soTien));
    bool am = (n < 0);
    unsigned long long g = am ? static_cast<unsigned long long>(-n)
                              : static_cast<unsigned long long>(n);
    std::string so = std::to_string(g);

    // Chen dau '.' ngan cach moi 3 chu so cho de doc
    std::string ketqua;
    int dem = 0;
    for (int i = static_cast<int>(so.size()) - 1; i >= 0; --i) {
        ketqua.push_back(so[i]);
        if (++dem % 3 == 0 && i != 0) ketqua.push_back('.');
    }
    std::reverse(ketqua.begin(), ketqua.end());
    if (am) ketqua = "-" + ketqua;
    return ketqua + " VND";
}

std::string ConsoleHelper::docChuoi(const std::string& thongBao) {
    std::cout << thongBao;
    std::string dong;
    std::getline(std::cin, dong);
    // Loai bo ky tu '\r' / '\n' thua o cuoi (neu co)
    while (!dong.empty() && (dong.back() == '\r' || dong.back() == '\n'))
        dong.pop_back();
    return dong;
}

int ConsoleHelper::docSoNguyen(const std::string& thongBao) {
    while (true) {
        std::string dong = docChuoi(thongBao);
        try {
            return std::stoi(dong);
        } catch (...) {
            std::cout << "  -> Giá trị không hợp lệ, vui lòng nhập một số nguyên.\n";
        }
    }
}

double ConsoleHelper::docSoThuc(const std::string& thongBao) {
    while (true) {
        std::string dong = docChuoi(thongBao);
        try {
            return std::stod(dong);
        } catch (...) {
            std::cout << "  -> Giá trị không hợp lệ, vui lòng nhập một số.\n";
        }
    }
}

double ConsoleHelper::docSoTienDuong(const std::string& thongBao) {
    while (true) {
        double gt = docSoThuc(thongBao);
        if (gt > 0) return gt;
        std::cout << "  -> Số tiền phải lớn hơn 0. Vui lòng nhập lại.\n";
    }
}

void ConsoleHelper::inTieuDe(const std::string& tieuDe) {
    std::cout << "\n----- " << tieuDe << " -----\n";
}

void ConsoleHelper::tamDung() {
    std::cout << "\nNhấn Enter để tiếp tục...";
    std::string tmp;
    std::getline(std::cin, tmp);
}
