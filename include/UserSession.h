#pragma once
#include <string>

// Lop UserSession luu phien dang nhap hien tai trong RAM.
// Khi dang xuat, toan bo thong tin phien se bi xoa.
class UserSession {
private:
    std::string userId;      // ma nguoi dung dang dang nhap
    std::string email;       // email cua phien hien tai
    std::string idToken;     // token xac thuc (dung khi ket noi Firebase that)
    std::string tenHienThi;  // ten hien thi cua nguoi dung
    bool dangDangNhap;       // true neu dang co nguoi dung dang nhap

public:
    UserSession();

    // Thiet lap phien khi dang nhap thanh cong
    void thietLapPhien(const std::string& userId, const std::string& email,
                       const std::string& idToken, const std::string& tenHienThi);

    // Xoa thong tin phien khi dang xuat
    void xoaPhien();

    // Getter
    std::string getUserId() const;
    std::string getEmail() const;
    std::string getIdToken() const;
    std::string getTenHienThi() const;

    // Kiem tra trang thai dang nhap
    bool kiemTraDangNhap() const;
};
