#include "ThuNhap.h"
#include "ConsoleHelper.h"
#include <iostream>

ThuNhap::ThuNhap(int maGiaoDich, double soTien, const std::string& ngay,
                 const std::string& danhMuc, const std::string& ghiChu,
                 const std::string& nguoiChi)
    : GiaoDich(maGiaoDich, soTien, ngay, danhMuc, ghiChu, nguoiChi) {}

void ThuNhap::inThongTin() const {
    // Ghi de phuong thuc cua lop cha -> DA HINH
    std::cout << "  [Mã " << maGiaoDich << "] "
              << ConsoleHelper::mauXanh() << "(+) Thu nhập" << ConsoleHelper::resetMau()
              << " | Số tiền: " << ConsoleHelper::dinhDangTien(soTien)
              << " | Ngày: " << ngay
              << " | Danh mục: " << danhMuc
              << " | Ghi chú: " << (ghiChu.empty() ? "(không có)" : ghiChu)
              << "\n";
}

std::string ThuNhap::layLoaiGiaoDich() const {
    return "Thu nhập";
}
