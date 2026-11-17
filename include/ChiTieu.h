#pragma once
#include "GiaoDich.h"
#include <string>

// Lop ChiTieu KE THUA tu GiaoDich.
// Dai dien cho cac khoan tien lam giam so du (an uong, di chuyen, mua sam...).
class ChiTieu : public GiaoDich {
public:
    ChiTieu(int maGiaoDich, double soTien, const std::string& ngay,
            const std::string& danhMuc, const std::string& ghiChu,
            const std::string& nguoiChi = "");

    // Ghi de (override) -> the hien tinh DA HINH
    void inThongTin() const override;
    std::string layLoaiGiaoDich() const override; // tra ve "Chi tieu"
};
