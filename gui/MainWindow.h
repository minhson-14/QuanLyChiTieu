#pragma once
#include <QMainWindow>
#include <QString>
#include "QuanLyChiTieu.h"
#include "FirebaseService.h"
#include "AuthService.h"

class QTableWidget;
class QLabel;
class QPushButton;

// Cua so chinh sau khi dang nhap: hien thi vi + danh sach giao dich,
// va cac nut them/sua/xoa/loc/thong ke/dong bo/dang xuat.
class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(AuthService* auth, FirebaseService* firebase, QWidget* parent = nullptr);

    // Nap du lieu cua user sau khi dang nhap; neu chua co vi thi hoi tao vi
    void napDuLieuChoUser(const QString& userId, const QString& tenHienThi);

signals:
    void dangXuat();

private slots:
    void themThuNhap();
    void themChiTieu();
    void suaGiaoDich();
    void xoaGiaoDich();
    void locTheoDanhMuc();
    void locTheoNgay();
    void xemThongKe();   // mo bieu do tron (pie chart) chi tieu theo danh muc
    void baoCaoTheoThang();
    void xuatCSV();
    void taiLai();
    void xuLyDangXuat();

private:
    void capNhatBang();   // ve lai bang voi toan bo giao dich
    void capNhatBangVoiDanhSach(const std::vector<std::shared_ptr<GiaoDich>>& ds);
    void capNhatSoDu();   // cap nhat nhan so du (in mau do neu am)
    void luu();           // luu du lieu xuong FirebaseService
    void themGiaoDichChung(bool laThuNhap);
    int layMaDangChon();

    AuthService* auth;
    FirebaseService* firebase;
    QuanLyChiTieu ql;
    QString userId;

    QLabel*       nhanTenVi;
    QLabel*       nhanSoDu;
    QTableWidget* bang;
    QLabel*       nhanBoLoc;   // hien thi bo loc dang ap dung
    QPushButton*  nutXoaLoc;   // xoa bo loc, hien lai tat ca
};
