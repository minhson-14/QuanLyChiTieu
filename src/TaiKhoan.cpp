#include "TaiKhoan.h"
#include "ConsoleHelper.h"
#include <iostream>

TaiKhoan::TaiKhoan() : tenVi("Ví của tôi"), soDuBanDau(0) {}

TaiKhoan::TaiKhoan(const std::string& tenVi, double soDuBanDau)
    : tenVi(tenVi), soDuBanDau(soDuBanDau) {}

std::string TaiKhoan::getTenVi() const { return tenVi; }
double TaiKhoan::getSoDuBanDau() const { return soDuBanDau; }

void TaiKhoan::setTenVi(const std::string& tenViMoi) { tenVi = tenViMoi; }
void TaiKhoan::setSoDuBanDau(double soDuMoi) { soDuBanDau = soDuMoi; }

void TaiKhoan::congTien(double soTien) {
    // Chi cong khi so tien duong
    if (soTien > 0) soDuBanDau += soTien;
}

void TaiKhoan::truTien(double soTien) {
    // Chi tru khi so tien duong (so du goc co the am sau khi tru)
    if (soTien > 0) soDuBanDau -= soTien;
}

void TaiKhoan::inThongTin() const {
    std::cout << "Tên ví: " << tenVi << "\n";
    std::cout << "Số dư ban đầu: " << ConsoleHelper::dinhDangTien(soDuBanDau) << "\n";
}
