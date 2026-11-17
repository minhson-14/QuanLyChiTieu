#pragma once
#include <string>

// Lop NguoiDung dai dien cho mot tai khoan nguoi dung trong he thong.
// The hien tinh DONG GOI: thuoc tinh private, truy cap qua getter/setter.
class NguoiDung {
private:
    std::string userId;      // ma dinh danh duy nhat cua nguoi dung
    std::string email;       // email dang nhap
    std::string tenHienThi;  // ten hien thi (suy ra tu email neu khong nhap)

public:
    NguoiDung();
    NguoiDung(const std::string& userId, const std::string& email,
              const std::string& tenHienThi);

    // Getter
    std::string getUserId() const;
    std::string getEmail() const;
    std::string getTenHienThi() const;

    // Setter
    void setUserId(const std::string& id);
    void setEmail(const std::string& e);
    void setTenHienThi(const std::string& ten);

    // In thong tin nguoi dung ra man hinh
    void inThongTin() const;
};
