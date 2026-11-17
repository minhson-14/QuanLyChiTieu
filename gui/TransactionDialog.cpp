#include "TransactionDialog.h"

#include <QVBoxLayout>
#include <QFormLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QDateEdit>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QSignalBlocker>
#include <QMessageBox>
#include <QDate>
#include <QStandardItemModel>

// Danh muc de xuat mac dinh
static const QStringList DANH_MUC_DE_XUAT = {
    "Ăn uống", "Di chuyển", "Lương", "Học tập", "Giải trí", "Khác"
};

TransactionDialog::TransactionDialog(const QString& tieuDe,
                                     const QStringList& danhMucHienCo,
                                     const QStringList& nguoiChiHienCo,
                                     QWidget* parent)
    : QDialog(parent) {
    setWindowTitle(tieuDe);
    setMinimumWidth(380);

    // --- O so tien ---
    oSoTien = new QLineEdit(this);
    oSoTien->setPlaceholderText("Ví dụ: 50.000");
    connect(oSoTien, &QLineEdit::textChanged, this, &TransactionDialog::onSoTienChanged);

    // --- O ngay ---
    oNgay = new QDateEdit(QDate::currentDate(), this);
    oNgay->setDisplayFormat("dd/MM/yyyy");
    oNgay->setCalendarPopup(true);

    // --- Danh muc: hien thi cua user truoc, sau do de xuat ---
    oDanhMuc = new QComboBox(this);
    oDanhMuc->setEditable(true);

    if (!danhMucHienCo.isEmpty()) {
        // Danh muc da dung
        for (const auto& dm : danhMucHienCo)
            oDanhMuc->addItem(dm);

        // Duong ke ngang phan cach
        oDanhMuc->insertSeparator(oDanhMuc->count());

        // Muc tieu de: "— Đề xuất —" (khong chon duoc)
        oDanhMuc->addItem("— Đề xuất —");
        auto* model = qobject_cast<QStandardItemModel*>(oDanhMuc->model());
        if (model) {
            auto* item = model->item(oDanhMuc->count() - 1);
            item->setFlags(Qt::NoItemFlags);
            item->setForeground(QColor("#999"));
        }

        // Them de xuat chua co trong danh sach hien co
        for (const auto& de : DANH_MUC_DE_XUAT)
            if (!danhMucHienCo.contains(de))
                oDanhMuc->addItem(de);
    } else {
        // Chua co giao dich nao -> hien toan bo de xuat
        oDanhMuc->addItems(DANH_MUC_DE_XUAT);
    }
    oDanhMuc->setCurrentIndex(0);

    // --- Ghi chu ---
    oGhiChu = new QLineEdit(this);
    oGhiChu->setPlaceholderText("Ghi chú (có thể bỏ trống)");

    // --- Thanh vien thuc hien ---
    oNguoiChi = new QComboBox(this);
    oNguoiChi->setEditable(true);
    oNguoiChi->lineEdit()->setPlaceholderText("Để trống nếu không cần ghi");
    oNguoiChi->addItem(""); // tuy chon, mac dinh trong
    for (const auto& nc : nguoiChiHienCo)
        oNguoiChi->addItem(nc);
    oNguoiChi->setCurrentIndex(0);

    // --- Form ---
    auto* form = new QFormLayout();
    form->addRow("Số tiền:", oSoTien);
    form->addRow("Ngày:", oNgay);
    form->addRow("Danh mục:", oDanhMuc);
    form->addRow("Thành viên:", oNguoiChi);
    form->addRow("Ghi chú:", oGhiChu);

    auto* nutBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    nutBox->button(QDialogButtonBox::Ok)->setText("Lưu");
    nutBox->button(QDialogButtonBox::Cancel)->setText("Hủy");
    connect(nutBox, &QDialogButtonBox::accepted, this, &TransactionDialog::kiemTraVaChapNhan);
    connect(nutBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    auto* layout = new QVBoxLayout(this);
    layout->addLayout(form);
    layout->addWidget(nutBox);
}

void TransactionDialog::onSoTienChanged(const QString& text) {
    QString digits;
    for (QChar c : text)
        if (c.isDigit()) digits += c;

    QString formatted;
    int n = digits.length();
    for (int i = 0; i < n; ++i) {
        if (i > 0 && (n - i) % 3 == 0) formatted += '.';
        formatted += digits[i];
    }

    if (formatted == text) return;
    QSignalBlocker blocker(oSoTien);
    int cursorFromEnd = text.length() - oSoTien->cursorPosition();
    oSoTien->setText(formatted);
    oSoTien->setCursorPosition(qMax(0, formatted.length() - cursorFromEnd));
}

void TransactionDialog::datGiaTri(double soTien, const QString& ngay,
                                  const QString& danhMuc, const QString& ghiChu,
                                  const QString& nguoiChi) {
    oSoTien->setText(QString::number(static_cast<long long>(soTien)));
    QDate d = QDate::fromString(ngay, "dd/MM/yyyy");
    if (d.isValid()) oNgay->setDate(d);
    oDanhMuc->setCurrentText(danhMuc);
    oGhiChu->setText(ghiChu);
    oNguoiChi->setCurrentText(nguoiChi);
}

double TransactionDialog::laySoTien() const {
    QString s = oSoTien->text();
    s.remove('.');
    return s.toDouble();
}

QString TransactionDialog::layNgay()    const { return oNgay->date().toString("dd/MM/yyyy"); }
QString TransactionDialog::layDanhMuc() const { return oDanhMuc->currentText().trimmed(); }
QString TransactionDialog::layGhiChu()  const { return oGhiChu->text().trimmed(); }
QString TransactionDialog::layNguoiChi() const { return oNguoiChi->currentText().trimmed(); }

void TransactionDialog::kiemTraVaChapNhan() {
    if (laySoTien() <= 0) {
        QMessageBox::warning(this, "Dữ liệu không hợp lệ",
                             "Số tiền phải lớn hơn 0. Vui lòng nhập lại.");
        return;
    }
    if (layDanhMuc().isEmpty()) {
        QMessageBox::warning(this, "Dữ liệu không hợp lệ",
                             "Vui lòng chọn hoặc nhập danh mục.");
        return;
    }
    accept();
}
