#pragma once
#include "GiaoDich.h"
#include <string>

// Lop ThuNhap KE THUA tu GiaoDich.
// Dai dien cho cac khoan tien lam tang so du (luong, thuong, duoc tang...).
class ThuNhap : public GiaoDich {
public:
    ThuNhap(int maGiaoDich, double soTien, const std::string& ngay,
            const std::string& danhMuc, const std::string& ghiChu,
            const std::string& nguoiChi = "");

    // Ghi de (override) -> the hien tinh DA HINH
    void inThongTin() const override;
    std::string layLoaiGiaoDich() const override; // tra ve "Thu nhap"
};
