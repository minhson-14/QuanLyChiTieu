#pragma once
#include <string>
#include <vector>
#include <memory>
#include <map>
#include "GiaoDich.h"
#include "TaiKhoan.h"

// Bao cao tong hop thu/chi trong mot thang cu the
struct BaoCaoThang {
    int thang, nam;
    double tongThu, tongChi, chenhLech; // chenhLech = tongThu - tongChi
};

// Lop QuanLyChiTieu quan ly VI va DANH SACH GIAO DICH cua MOT nguoi dung.
// Danh sach giao dich dung vector<shared_ptr<GiaoDich>> de luu DA HINH:
// mot vector chua duoc ca ThuNhap lan ChiTieu thong qua con tro lop cha.
class QuanLyChiTieu {
private:
    TaiKhoan taiKhoan;                                       // vi cua nguoi dung
    std::vector<std::shared_ptr<GiaoDich>> danhSachGiaoDich; // luu giao dich da hinh
    int maTiepTheo;                                          // ma giao dich tu tang

public:
    QuanLyChiTieu();

    // --- Quan ly vi ---
    void setTaiKhoan(const TaiKhoan& tk);
    TaiKhoan& getTaiKhoan();
    const TaiKhoan& getTaiKhoan() const;

    // --- Quan ly ma tu tang ---
    int getMaTiepTheo() const;
    void setMaTiepTheo(int gt);
    int taoMaMoi(); // tra ve ma moi va tang bo dem len 1

    // --- Them giao dich (da tao san o ben ngoai) ---
    void themGiaoDich(std::shared_ptr<GiaoDich> gd);

    // --- Sua / xoa / tim theo ma ---
    bool suaGiaoDichTheoMa(int ma, double soTienMoi, const std::string& ngayMoi,
                           const std::string& danhMucMoi, const std::string& ghiChuMoi,
                           const std::string& nguoiChiMoi = "");
    bool xoaGiaoDichTheoMa(int ma);
    std::shared_ptr<GiaoDich> timGiaoDichTheoMa(int ma) const;

    // --- Hien thi ---
    void hienThiTatCaGiaoDich() const;

    // --- Thong ke ---
    double tinhTongThuNhap() const;
    double tinhTongChiTieu() const;
    double tinhSoDuHienTai() const; // soDuBanDau + tongThuNhap - tongChiTieu

    // --- Loc ---
    std::vector<std::shared_ptr<GiaoDich>> locGiaoDichTheoDanhMuc(const std::string& danhMuc) const;
    std::vector<std::shared_ptr<GiaoDich>> locGiaoDichTheoNgay(const std::string& ngay) const;

    // Thong ke tong chi tieu theo tung danh muc
    std::map<std::string, double> thongKeTheoDanhMuc() const;
    // Thong ke tong chi tieu theo tung thanh vien (nguoiChi)
    std::map<std::string, double> thongKeTheoNguoiChi() const;
    // Danh sach thanh vien da xuat hien trong giao dich
    std::vector<std::string> layDanhSachNguoiChi() const;

    // Bao cao thu/chi cua mot thang (ngay dinh dang "dd/MM/yyyy")
    BaoCaoThang baoCaoTheoThang(int thang, int nam) const;
    // Danh sach (thang, nam) co giao dich, sap xep theo thu tu thoi gian
    std::vector<std::pair<int,int>> layDanhSachThangNam() const;

    // --- Tien ich khac ---
    void xoaTatCaGiaoDich();
    const std::vector<std::shared_ptr<GiaoDich>>& layDanhSachGiaoDich() const;
};
