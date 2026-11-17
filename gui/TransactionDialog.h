#pragma once
#include <QDialog>
#include <QStringList>

class QLineEdit;
class QComboBox;
class QDateEdit;

// Hop thoai nhap/sua mot giao dich.
// danhMucHienCo: danh muc da co trong giao dich cua user (hien len dau, sau do moi den de xuat)
// nguoiChiHienCo: danh sach thanh vien da tung xuat hien (dung cho autocomplete)
class TransactionDialog : public QDialog {
    Q_OBJECT
public:
    explicit TransactionDialog(const QString& tieuDe,
                               const QStringList& danhMucHienCo = {},
                               const QStringList& nguoiChiHienCo = {},
                               QWidget* parent = nullptr);

    void datGiaTri(double soTien, const QString& ngay, const QString& danhMuc,
                   const QString& ghiChu, const QString& nguoiChi = "");

    double  laySoTien()  const;
    QString layNgay()    const;
    QString layDanhMuc() const;
    QString layGhiChu()  const;
    QString layNguoiChi() const;

private slots:
    void kiemTraVaChapNhan();
    void onSoTienChanged(const QString& text);

private:
    QLineEdit* oSoTien;
    QDateEdit* oNgay;
    QComboBox* oDanhMuc;
    QLineEdit* oGhiChu;
    QComboBox* oNguoiChi;
};
