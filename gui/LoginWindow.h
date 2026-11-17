#pragma once
#include <QWidget>
#include "AuthService.h"
#include "FirebaseService.h"

class QLineEdit;

// Cua so dang nhap / dang ky (man hinh dau tien cua ung dung GUI)
class LoginWindow : public QWidget {
    Q_OBJECT
public:
    LoginWindow(AuthService* auth, FirebaseService* firebase, QWidget* parent = nullptr);

signals:
    // Phat ra khi dang nhap thanh cong, kem theo userId va ten hien thi
    void dangNhapThanhCong(const QString& userId, const QString& tenHienThi);

private slots:
    void xuLyDangKy();
    void xuLyDangNhap();

private:
    AuthService* auth;       // khong so huu
    FirebaseService* firebase; // khong so huu
    QLineEdit* oEmail;
    QLineEdit* oMatKhau;
};
