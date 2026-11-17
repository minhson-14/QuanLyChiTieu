#include "UserSession.h"

UserSession::UserSession()
    : userId(""), email(""), idToken(""), tenHienThi(""), dangDangNhap(false) {}

void UserSession::thietLapPhien(const std::string& userId, const std::string& email,
                                const std::string& idToken, const std::string& tenHienThi) {
    this->userId = userId;
    this->email = email;
    this->idToken = idToken;
    this->tenHienThi = tenHienThi;
    this->dangDangNhap = true;
}

void UserSession::xoaPhien() {
    // Xoa sach thong tin phien khoi RAM khi dang xuat
    userId.clear();
    email.clear();
    idToken.clear();
    tenHienThi.clear();
    dangDangNhap = false;
}

std::string UserSession::getUserId() const { return userId; }
std::string UserSession::getEmail() const { return email; }
std::string UserSession::getIdToken() const { return idToken; }
std::string UserSession::getTenHienThi() const { return tenHienThi; }
bool UserSession::kiemTraDangNhap() const { return dangDangNhap; }
