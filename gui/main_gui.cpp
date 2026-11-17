// ============================================================
//  PHIEN BAN GIAO DIEN DO HOA (Qt) cua phan mem Quan ly chi tieu
//  Dung lai TOAN BO backend OOP cua ban console:
//   AuthService, FirebaseService, QuanLyChiTieu, GiaoDich, ThuNhap, ChiTieu...
//  Diem khac biet: thay menu console bang cac cua so Qt.
// ============================================================
#include <QApplication>
#include <QTimer>

#include "LoginWindow.h"
#include "MainWindow.h"
#include "FirebaseService.h"
#include "AuthService.h"

int main(int argc, char** argv) {
    QApplication app(argc, argv);

    // Khoi tao backend o che do MOCK: luu file local trong thu muc data/
    // (nam canh thu muc lam viec luc chay chuong trinh). Neu chay ban GUI va
    // ban console tu CUNG mot thu muc thi chung se dung chung du lieu.
    FirebaseService firebase(true);
    AuthService auth(&firebase);

    LoginWindow login(&auth, &firebase);
    MainWindow mainWin(&auth, &firebase);

    // Dang nhap thanh cong -> nap du lieu user va chuyen sang cua so chinh
    QObject::connect(&login, &LoginWindow::dangNhapThanhCong,
                     [&](const QString& userId, const QString& tenHienThi) {
        mainWin.napDuLieuChoUser(userId, tenHienThi);
        login.hide();
        mainWin.show();
    });

    // Dang xuat -> quay lai cua so dang nhap
    QObject::connect(&mainWin, &MainWindow::dangXuat, [&]() {
        mainWin.hide();
        login.show();
    });

    login.show();

    // Ho tro kiem thu tu dong (smoke test): neu dat bien moi truong QLCT_SMOKE
    // thi tu dong thoat sau 1.5 giay de xac nhan ung dung khoi dong khong loi.
    if (qEnvironmentVariableIsSet("QLCT_SMOKE"))
        QTimer::singleShot(1500, &app, &QApplication::quit);

    return app.exec();
}
