#include "GiaoDich.h"

GiaoDich::GiaoDich(int maGiaoDich, double soTien, const std::string& ngay,
                   const std::string& danhMuc, const std::string& ghiChu,
                   const std::string& nguoiChi)
    : maGiaoDich(maGiaoDich), soTien(soTien), ngay(ngay),
      danhMuc(danhMuc), ghiChu(ghiChu), nguoiChi(nguoiChi) {
    if (this->soTien <= 0) this->soTien = 0;
}

GiaoDich::~GiaoDich() {
    // Khong co tai nguyen dong can giai phong thu cong.
    // Destructor de ao de huy dung lop con khi xoa qua con tro lop cha.
}

int GiaoDich::getMaGiaoDich() const { return maGiaoDich; }
double GiaoDich::getSoTien() const { return soTien; }
std::string GiaoDich::getNgay() const { return ngay; }
std::string GiaoDich::getDanhMuc() const { return danhMuc; }
std::string GiaoDich::getGhiChu() const { return ghiChu; }
std::string GiaoDich::getNguoiChi() const { return nguoiChi; }

void GiaoDich::setMaGiaoDich(int maMoi) { maGiaoDich = maMoi; }

void GiaoDich::setSoTien(double soTienMoi) {
    if (soTienMoi > 0) soTien = soTienMoi;
}

void GiaoDich::setNgay(const std::string& ngayMoi) { ngay = ngayMoi; }
void GiaoDich::setDanhMuc(const std::string& danhMucMoi) { danhMuc = danhMucMoi; }
void GiaoDich::setGhiChu(const std::string& ghiChuMoi) { ghiChu = ghiChuMoi; }
void GiaoDich::setNguoiChi(const std::string& nguoiChiMoi) { nguoiChi = nguoiChiMoi; }

void GiaoDich::capNhatThongTin(double soTienMoi, const std::string& ngayMoi,
                               const std::string& danhMucMoi, const std::string& ghiChuMoi,
                               const std::string& nguoiChiMoi) {
    setSoTien(soTienMoi);
    setNgay(ngayMoi);
    setDanhMuc(danhMucMoi);
    setGhiChu(ghiChuMoi);
    setNguoiChi(nguoiChiMoi);
}
