#include "AuthService.h"

AuthService::AuthService(FirebaseService* firebase) : firebase(firebase) {}

bool AuthService::dangKy(const std::string& email, const std::string& matKhau,
                         std::string& thongBaoLoi) {
    std::string userId;
    // Goi xuong FirebaseService de tao tai khoan (mock hoac that)
    return firebase->dangKyNguoiDung(email, matKhau, userId, thongBaoLoi);
}

bool AuthService::dangNhap(const std::string& email, const std::string& matKhau,
                           std::string& thongBaoLoi) {
    std::string userId, idToken, tenHienThi;
    if (firebase->dangNhapNguoiDung(email, matKhau, userId, idToken, tenHienThi, thongBaoLoi)) {
        // Dang nhap thanh cong -> luu phien vao RAM
        phienHienTai.thietLapPhien(userId, email, idToken, tenHienThi);
        return true;
    }
    return false;
}

void AuthService::dangXuat() {
    // Xoa thong tin phien hien tai khoi RAM
    phienHienTai.xoaPhien();
}

bool AuthService::kiemTraDangNhap() const {
    return phienHienTai.kiemTraDangNhap();
}

NguoiDung AuthService::layNguoiDungHienTai() const {
    return NguoiDung(phienHienTai.getUserId(), phienHienTai.getEmail(),
                     phienHienTai.getTenHienThi());
}

const UserSession& AuthService::layPhienHienTai() const {
    return phienHienTai;
}
