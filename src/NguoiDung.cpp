#include "NguoiDung.h"
#include <iostream>

NguoiDung::NguoiDung() : userId(""), email(""), tenHienThi("") {}

NguoiDung::NguoiDung(const std::string& userId, const std::string& email,
                     const std::string& tenHienThi)
    : userId(userId), email(email), tenHienThi(tenHienThi) {}

std::string NguoiDung::getUserId() const { return userId; }
std::string NguoiDung::getEmail() const { return email; }
std::string NguoiDung::getTenHienThi() const { return tenHienThi; }

void NguoiDung::setUserId(const std::string& id) { userId = id; }
void NguoiDung::setEmail(const std::string& e) { email = e; }
void NguoiDung::setTenHienThi(const std::string& ten) { tenHienThi = ten; }

void NguoiDung::inThongTin() const {
    std::cout << "Người dùng: " << tenHienThi
              << " | Email: " << email
              << " | ID: " << userId << "\n";
}
