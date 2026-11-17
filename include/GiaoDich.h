#pragma once
#include <string>

// Lop GiaoDich la lop CHA TRUU TUONG cho moi loai giao dich.
// The hien tinh TRUU TUONG: co phuong thuc thuan ao (pure virtual) nen
// khong the tao truc tiep doi tuong GiaoDich, ma phai qua lop con.
class GiaoDich {
protected:
    // De protected de lop con (ThuNhap, ChiTieu) co the truy cap khi ghi de inThongTin
    int maGiaoDich;        // ma so giao dich, tu tang theo tung nguoi dung
    double soTien;         // so tien giao dich, luon > 0
    std::string ngay;      // ngay giao dich dang dd/MM/yyyy
    std::string danhMuc;   // danh muc: An uong, Di chuyen, Luong...
    std::string ghiChu;    // ghi chu them cho giao dich
    std::string nguoiChi;  // thanh vien thuc hien giao dich (tuy chon, mac dinh rong)

public:
    // Ham khoi tao (nguoiChi la tuy chon)
    GiaoDich(int maGiaoDich, double soTien, const std::string& ngay,
             const std::string& danhMuc, const std::string& ghiChu,
             const std::string& nguoiChi = "");

    // Destructor ao: bat buoc khi co da hinh + xoa qua con tro lop cha
    virtual ~GiaoDich();

    // Getter (DONG GOI)
    int getMaGiaoDich() const;
    double getSoTien() const;
    std::string getNgay() const;
    std::string getDanhMuc() const;
    std::string getGhiChu() const;
    std::string getNguoiChi() const;

    // Setter co kiem tra rang buoc
    void setMaGiaoDich(int maMoi);
    void setSoTien(double soTienMoi); // chi chap nhan gia tri > 0
    void setNgay(const std::string& ngayMoi);
    void setDanhMuc(const std::string& danhMucMoi);
    void setGhiChu(const std::string& ghiChuMoi);
    void setNguoiChi(const std::string& nguoiChiMoi);

    // Cap nhat nhieu thong tin cung luc (nguoiChi tuy chon)
    void capNhatThongTin(double soTienMoi, const std::string& ngayMoi,
                         const std::string& danhMucMoi, const std::string& ghiChuMoi,
                         const std::string& nguoiChiMoi = "");

    // Phuong thuc THUAN AO -> bat buoc lop con cai dat (TRUU TUONG + DA HINH)
    virtual void inThongTin() const = 0;
    virtual std::string layLoaiGiaoDich() const = 0;
};
