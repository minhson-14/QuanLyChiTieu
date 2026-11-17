#pragma once
#include <string>
#include "FirebaseService.h"
#include "UserSession.h"
#include "NguoiDung.h"

// Lop AuthService xu ly DANG KY / DANG NHAP / DANG XUAT.
// Lop nay goi xuong FirebaseService de xac thuc (mock hoac that) va
// giu phien dang nhap hien tai trong RAM thong qua UserSession.
class AuthService {
private:
    FirebaseService* firebase; // con tro toi dich vu Firebase (khong so huu)
    UserSession phienHienTai;  // phien dang nhap hien tai trong RAM

public:
    AuthService(FirebaseService* firebase);

    // Dang ky tai khoan moi
    bool dangKy(const std::string& email, const std::string& matKhau,
                std::string& thongBaoLoi);

    // Dang nhap; neu thanh cong thi thiet lap phien hien tai
    bool dangNhap(const std::string& email, const std::string& matKhau,
                  std::string& thongBaoLoi);

    // Dang xuat: xoa phien hien tai khoi RAM
    void dangXuat();

    // Kiem tra co ai dang dang nhap khong
    bool kiemTraDangNhap() const;

    // Lay nguoi dung hien tai duoi dang doi tuong NguoiDung
    NguoiDung layNguoiDungHienTai() const;

    // Lay tham chieu toi phien hien tai (de doc userId, email, idToken...)
    const UserSession& layPhienHienTai() const;
};
