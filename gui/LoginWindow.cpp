#include "LoginWindow.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>
#include <string>

LoginWindow::LoginWindow(AuthService* auth, FirebaseService* firebase, QWidget* parent)
    : QWidget(parent), auth(auth), firebase(firebase) {
    setWindowTitle("Quản lý chi tiêu cá nhân - Đăng nhập");
    setMinimumWidth(380);

    auto* tieuDe = new QLabel("💰 QUẢN LÝ CHI TIÊU CÁ NHÂN", this);
    tieuDe->setAlignment(Qt::AlignCenter);
    tieuDe->setStyleSheet("font-size: 18px; font-weight: bold; margin: 12px;");

    oEmail = new QLineEdit(this);
    oEmail->setPlaceholderText("email@example.com");

    oMatKhau = new QLineEdit(this);
    oMatKhau->setEchoMode(QLineEdit::Password);
    oMatKhau->setPlaceholderText("Mật khẩu (ít nhất 4 ký tự)");

    auto* form = new QFormLayout();
    form->addRow("Email:", oEmail);
    form->addRow("Mật khẩu:", oMatKhau);

    auto* nutDangNhap = new QPushButton("Đăng nhập", this);
    nutDangNhap->setDefault(true);
    auto* nutDangKy = new QPushButton("Đăng ký", this);

    connect(nutDangNhap, &QPushButton::clicked, this, &LoginWindow::xuLyDangNhap);
    connect(nutDangKy, &QPushButton::clicked, this, &LoginWindow::xuLyDangKy);
    // Nhan Enter o o mat khau -> dang nhap
    connect(oMatKhau, &QLineEdit::returnPressed, this, &LoginWindow::xuLyDangNhap);

    auto* hangNut = new QHBoxLayout();
    hangNut->addWidget(nutDangNhap);
    hangNut->addWidget(nutDangKy);

    auto* layout = new QVBoxLayout(this);
    layout->addWidget(tieuDe);
    layout->addLayout(form);
    layout->addLayout(hangNut);
    layout->addStretch();
}

void LoginWindow::xuLyDangKy() {
    std::string email = oEmail->text().trimmed().toStdString();
    std::string matKhau = oMatKhau->text().toStdString();
    std::string loi;
    if (auth->dangKy(email, matKhau, loi)) {
        QMessageBox::information(this, "Thành công",
                                "Đăng ký thành công! Bây giờ hãy đăng nhập.");
    } else {
        QMessageBox::warning(this, "Đăng ký thất bại", QString::fromStdString(loi));
    }
}

void LoginWindow::xuLyDangNhap() {
    std::string email = oEmail->text().trimmed().toStdString();
    std::string matKhau = oMatKhau->text().toStdString();
    std::string loi;
    if (auth->dangNhap(email, matKhau, loi)) {
        QString userId = QString::fromStdString(auth->layPhienHienTai().getUserId());
        QString ten = QString::fromStdString(auth->layPhienHienTai().getTenHienThi());
        oMatKhau->clear();
        emit dangNhapThanhCong(userId, ten);
    } else {
        QMessageBox::warning(this, "Đăng nhập thất bại", QString::fromStdString(loi));
    }
}
