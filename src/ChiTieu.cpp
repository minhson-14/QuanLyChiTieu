#include "ChiTieu.h"
#include "ConsoleHelper.h"
#include <iostream>

ChiTieu::ChiTieu(int maGiaoDich, double soTien, const std::string& ngay,
                 const std::string& danhMuc, const std::string& ghiChu,
                 const std::string& nguoiChi)
    : GiaoDich(maGiaoDich, soTien, ngay, danhMuc, ghiChu, nguoiChi) {}

void ChiTieu::inThongTin() const {
    // Ghi de phuong thuc cua lop cha -> DA HINH
    std::cout << "  [Mã " << maGiaoDich << "] "
              << ConsoleHelper::mauVang() << "(-) Chi tiêu" << ConsoleHelper::resetMau()
              << " | Số tiền: " << ConsoleHelper::dinhDangTien(soTien)
              << " | Ngày: " << ngay
              << " | Danh mục: " << danhMuc
              << " | Ghi chú: " << (ghiChu.empty() ? "(không có)" : ghiChu)
              << "\n";
}

std::string ChiTieu::layLoaiGiaoDich() const {
    return "Chi tiêu";
}
