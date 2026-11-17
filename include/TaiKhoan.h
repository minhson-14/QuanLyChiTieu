#pragma once
#include <string>

// Lop TaiKhoan dai dien cho VI tien cua mot nguoi dung.
// Moi nguoi dung co dung MOT vi.
// Luu y: so du hien tai KHONG luu cung o day, ma duoc tinh lai trong
//        QuanLyChiTieu theo cong thuc: soDuBanDau + tongThuNhap - tongChiTieu.
class TaiKhoan {
private:
    std::string tenVi;   // ten vi do nguoi dung tu dat
    double soDuBanDau;   // so tien co san luc bat dau dung app

public:
    TaiKhoan();
    TaiKhoan(const std::string& tenVi, double soDuBanDau);

    // Getter / Setter (DONG GOI)
    std::string getTenVi() const;
    double getSoDuBanDau() const;
    void setTenVi(const std::string& tenViMoi);
    void setSoDuBanDau(double soDuMoi);

    // Dieu chinh so du goc cua vi (vd nguoi dung nap them / rut bot vao so goc)
    void congTien(double soTien); // tang so du ban dau, chi nhan so tien > 0
    void truTien(double soTien);  // giam so du ban dau, chi nhan so tien > 0

    // In thong tin vi ra man hinh
    void inThongTin() const;
};
