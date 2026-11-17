#pragma once
#include <string>
#include "TaiKhoan.h"
#include "QuanLyChiTieu.h"

// Lop FirebaseService la DIEM TAP TRUNG moi giao tiep voi "Firebase".
// Hien tai chay o che do MOCK: luu/doc du lieu tren file local trong thu muc data/.
// Khi nang cap len Firebase that, chi can cai dat lai cac ham nay bang REST API
// (HTTP) ma KHONG can sua phan con lai cua chuong trinh.
//
// Cau truc du lieu tren Firebase Realtime Database tuong duong:
//   users/{userId}/taiKhoan/{tenVi, soDuBanDau}
//   users/{userId}/giaoDich/{maGiaoDich}/{loai, soTien, ngay, danhMuc, ghiChu}
class FirebaseService {
private:
    bool suDungMock;          // true = mock (file local), false = Firebase that
    std::string thuMucDuLieu; // thu muc luu file mock (mac dinh "data")

    // Cau hinh Firebase that — doc tu firebase_config.json, KHONG hardcode o day
    std::string apiKey;           // Web API Key
    std::string databaseUrl;      // Realtime Database URL
    std::string idTokenHienTai;   // idToken sau khi dang nhap thanh cong

    // Doc firebase_config.json, tra ve true neu hop le -> tu dong tat mock
    bool docCauHinhFirebase();

    // Ham tien ich noi bo cho che do mock
    std::string duongDanFileUser(const std::string& userId) const; // data/{userId}.json
    std::string duongDanFileTaiKhoan() const;                      // data/_accounts.json
    void taoThuMucNeuChua() const;

public:
    FirebaseService(bool suDungMock = true);

    bool dangSuDungMock() const;
    void datCheDoMock(bool batMock);

    // --- Xac thuc (tuong duong Firebase Authentication) ---
    // Mock: doc/ghi file data/_accounts.json
    // That: goi Firebase Authentication REST API (signUp / signInWithPassword)
    bool dangKyNguoiDung(const std::string& email, const std::string& matKhau,
                         std::string& userIdRa, std::string& thongBaoLoi);
    bool dangNhapNguoiDung(const std::string& email, const std::string& matKhau,
                           std::string& userIdRa, std::string& idTokenRa,
                           std::string& tenHienThiRa, std::string& thongBaoLoi);

    // --- Du lieu nguoi dung (tuong duong Realtime Database: users/{userId}/...) ---
    bool taiTaiKhoan(const std::string& userId, TaiKhoan& taiKhoanRa); // false neu chua co vi
    void luuTaiKhoan(const std::string& userId, const TaiKhoan& taiKhoan);

    void taiDanhSachGiaoDich(const std::string& userId, QuanLyChiTieu& ql);
    void luuDanhSachGiaoDich(const std::string& userId, const QuanLyChiTieu& ql);

    // Tai/luu TOAN BO du lieu cua mot user (vi + danh sach giao dich)
    bool taiDuLieuNguoiDung(const std::string& userId, QuanLyChiTieu& ql); // false neu chua co vi
    void luuDuLieuNguoiDung(const std::string& userId, const QuanLyChiTieu& ql);

    // Dong bo du lieu (mock: ghi lai file; that: PUT/PATCH len RTDB)
    void dongBoDuLieu(const std::string& userId, const QuanLyChiTieu& ql);

    // Xuat du lieu ra file CSV (UTF-8 BOM) de mo bang Excel.
    // duongDan trong: mac dinh luu tai data/{userId}.csv
    bool xuatCSV(const std::string& userId, const QuanLyChiTieu& ql,
                 const std::string& duongDan = "") const;
};
