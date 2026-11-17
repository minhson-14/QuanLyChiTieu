#include "QuanLyChiTieu.h"
#include "ThuNhap.h"
#include "ChiTieu.h"
#include <iostream>
#include <algorithm>
#include <stdexcept>

QuanLyChiTieu::QuanLyChiTieu() : taiKhoan(), maTiepTheo(1) {}

// --- Quan ly vi ---
void QuanLyChiTieu::setTaiKhoan(const TaiKhoan& tk) { taiKhoan = tk; }
TaiKhoan& QuanLyChiTieu::getTaiKhoan() { return taiKhoan; }
const TaiKhoan& QuanLyChiTieu::getTaiKhoan() const { return taiKhoan; }

// --- Quan ly ma tu tang ---
int QuanLyChiTieu::getMaTiepTheo() const { return maTiepTheo; }
void QuanLyChiTieu::setMaTiepTheo(int gt) { maTiepTheo = gt; }
int QuanLyChiTieu::taoMaMoi() { return maTiepTheo++; }

// --- Them giao dich ---
void QuanLyChiTieu::themGiaoDich(std::shared_ptr<GiaoDich> gd) {
    if (gd) danhSachGiaoDich.push_back(gd);
}

// --- Sua giao dich theo ma ---
bool QuanLyChiTieu::suaGiaoDichTheoMa(int ma, double soTienMoi, const std::string& ngayMoi,
                                      const std::string& danhMucMoi, const std::string& ghiChuMoi,
                                      const std::string& nguoiChiMoi) {
    for (auto& gd : danhSachGiaoDich) {
        if (gd->getMaGiaoDich() == ma) {
            gd->capNhatThongTin(soTienMoi, ngayMoi, danhMucMoi, ghiChuMoi, nguoiChiMoi);
            return true;
        }
    }
    return false;
}

// --- Xoa giao dich theo ma ---
bool QuanLyChiTieu::xoaGiaoDichTheoMa(int ma) {
    for (auto it = danhSachGiaoDich.begin(); it != danhSachGiaoDich.end(); ++it) {
        if ((*it)->getMaGiaoDich() == ma) {
            danhSachGiaoDich.erase(it);
            return true;
        }
    }
    return false;
}

// --- Tim giao dich theo ma ---
std::shared_ptr<GiaoDich> QuanLyChiTieu::timGiaoDichTheoMa(int ma) const {
    for (const auto& gd : danhSachGiaoDich)
        if (gd->getMaGiaoDich() == ma) return gd;
    return nullptr; // tra ve nullptr neu khong tim thay
}

// --- Hien thi tat ca giao dich ---
void QuanLyChiTieu::hienThiTatCaGiaoDich() const {
    if (danhSachGiaoDich.empty()) {
        std::cout << "Chưa có giao dịch nào.\n";
        return;
    }
    std::cout << "Tổng số giao dịch: " << danhSachGiaoDich.size() << "\n";
    for (const auto& gd : danhSachGiaoDich)
        gd->inThongTin(); // DA HINH: goi dung phuong thuc cua lop con
}

// --- Thong ke ---
double QuanLyChiTieu::tinhTongThuNhap() const {
    double tong = 0;
    for (const auto& gd : danhSachGiaoDich)
        if (std::dynamic_pointer_cast<ThuNhap>(gd)) tong += gd->getSoTien();
    return tong;
}

double QuanLyChiTieu::tinhTongChiTieu() const {
    double tong = 0;
    for (const auto& gd : danhSachGiaoDich)
        if (std::dynamic_pointer_cast<ChiTieu>(gd)) tong += gd->getSoTien();
    return tong;
}

double QuanLyChiTieu::tinhSoDuHienTai() const {
    // Tinh lai tu danh sach giao dich -> luon dung sau khi them/sua/xoa
    return taiKhoan.getSoDuBanDau() + tinhTongThuNhap() - tinhTongChiTieu();
}

// --- Loc ---
std::vector<std::shared_ptr<GiaoDich>>
QuanLyChiTieu::locGiaoDichTheoDanhMuc(const std::string& danhMuc) const {
    std::vector<std::shared_ptr<GiaoDich>> kq;
    for (const auto& gd : danhSachGiaoDich)
        if (gd->getDanhMuc() == danhMuc) kq.push_back(gd);
    return kq;
}

std::vector<std::shared_ptr<GiaoDich>>
QuanLyChiTieu::locGiaoDichTheoNgay(const std::string& ngay) const {
    std::vector<std::shared_ptr<GiaoDich>> kq;
    for (const auto& gd : danhSachGiaoDich)
        if (gd->getNgay() == ngay) kq.push_back(gd);
    return kq;
}

std::map<std::string, double> QuanLyChiTieu::thongKeTheoDanhMuc() const {
    std::map<std::string, double> bang;
    for (const auto& gd : danhSachGiaoDich)
        if (std::dynamic_pointer_cast<ChiTieu>(gd))
            bang[gd->getDanhMuc()] += gd->getSoTien();
    return bang;
}

std::map<std::string, double> QuanLyChiTieu::thongKeTheoNguoiChi() const {
    std::map<std::string, double> bang;
    for (const auto& gd : danhSachGiaoDich) {
        if (!std::dynamic_pointer_cast<ChiTieu>(gd)) continue;
        std::string nc = gd->getNguoiChi();
        if (nc.empty()) nc = "Chưa ghi";
        bang[nc] += gd->getSoTien();
    }
    return bang;
}

std::vector<std::string> QuanLyChiTieu::layDanhSachNguoiChi() const {
    std::vector<std::string> ds;
    for (const auto& gd : danhSachGiaoDich) {
        const std::string& nc = gd->getNguoiChi();
        if (!nc.empty() && std::find(ds.begin(), ds.end(), nc) == ds.end())
            ds.push_back(nc);
    }
    return ds;
}

// --- Tien ich khac ---
void QuanLyChiTieu::xoaTatCaGiaoDich() { danhSachGiaoDich.clear(); }

const std::vector<std::shared_ptr<GiaoDich>>&
QuanLyChiTieu::layDanhSachGiaoDich() const {
    return danhSachGiaoDich;
}

// --- Bao cao theo thang ---
BaoCaoThang QuanLyChiTieu::baoCaoTheoThang(int thang, int nam) const {
    BaoCaoThang bc{thang, nam, 0.0, 0.0, 0.0};
    for (const auto& gd : danhSachGiaoDich) {
        const std::string& ngay = gd->getNgay(); // dinh dang "dd/MM/yyyy"
        if (ngay.size() < 10) continue;
        try {
            int t = std::stoi(ngay.substr(3, 2));
            int n = std::stoi(ngay.substr(6, 4));
            if (t != thang || n != nam) continue;
            if (std::dynamic_pointer_cast<ThuNhap>(gd))
                bc.tongThu += gd->getSoTien();
            else
                bc.tongChi += gd->getSoTien();
        } catch (...) {}
    }
    bc.chenhLech = bc.tongThu - bc.tongChi;
    return bc;
}

std::vector<std::pair<int,int>> QuanLyChiTieu::layDanhSachThangNam() const {
    std::vector<std::pair<int,int>> ds;
    for (const auto& gd : danhSachGiaoDich) {
        const std::string& ngay = gd->getNgay();
        if (ngay.size() < 10) continue;
        try {
            int t = std::stoi(ngay.substr(3, 2));
            int n = std::stoi(ngay.substr(6, 4));
            auto p = std::make_pair(t, n);
            if (std::find(ds.begin(), ds.end(), p) == ds.end())
                ds.push_back(p);
        } catch (...) {}
    }
    std::sort(ds.begin(), ds.end(), [](const auto& a, const auto& b) {
        return a.second != b.second ? a.second < b.second : a.first < b.first;
    });
    return ds;
}
