#include "MainWindow.h"
#include "TransactionDialog.h"
#include "PieChartDialog.h"

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QAbstractItemView>
#include <QMessageBox>
#include <QInputDialog>
#include <QFileDialog>
#include <QColor>
#include <QLineEdit>
#include <QDialog>
#include <QDialogButtonBox>
#include <QDateEdit>

#include <memory>
#include <string>
#include <map>

#include "ThuNhap.h"
#include "ChiTieu.h"
#include "ConsoleHelper.h" // dung lai ham dinh dang tien dinhDangTien()

MainWindow::MainWindow(AuthService* auth, FirebaseService* firebase, QWidget* parent)
    : QMainWindow(parent), auth(auth), firebase(firebase) {
    setWindowTitle("Quản lý chi tiêu cá nhân");
    resize(820, 520);

    auto* trungTam = new QWidget(this);
    auto* layout = new QVBoxLayout(trungTam);

    // --- Khu thong tin vi ---
    nhanTenVi = new QLabel(this);
    nhanTenVi->setStyleSheet("font-size: 15px; font-weight: bold;");
    nhanSoDu = new QLabel(this);
    nhanSoDu->setStyleSheet("font-size: 16px; font-weight: bold; color: #2c3e50;");
    layout->addWidget(nhanTenVi);
    layout->addWidget(nhanSoDu);

    // --- Hang bo loc dang ap dung ---
    auto* filterRow = new QHBoxLayout();
    nhanBoLoc = new QLabel(this);
    nhanBoLoc->setStyleSheet("color:#3498db; font-style:italic;");
    nhanBoLoc->setVisible(false);
    nutXoaLoc = new QPushButton("✕ Xóa lọc", this);
    nutXoaLoc->setVisible(false);
    nutXoaLoc->setStyleSheet("color:#e74c3c;");
    connect(nutXoaLoc, &QPushButton::clicked, this, [this]() {
        nhanBoLoc->setVisible(false);
        nutXoaLoc->setVisible(false);
        capNhatBang();
    });
    filterRow->addWidget(nhanBoLoc);
    filterRow->addStretch();
    filterRow->addWidget(nutXoaLoc);
    layout->addLayout(filterRow);

    // --- Bang giao dich ---
    bang = new QTableWidget(this);
    bang->setColumnCount(7);
    bang->setHorizontalHeaderLabels({"Mã", "Loại", "Số tiền", "Ngày", "Danh mục", "Thành viên", "Ghi chú"});
    bang->setEditTriggers(QAbstractItemView::NoEditTriggers);
    bang->setSelectionBehavior(QAbstractItemView::SelectRows);
    bang->setSelectionMode(QAbstractItemView::SingleSelection);
    bang->verticalHeader()->setVisible(false);
    bang->horizontalHeader()->setStretchLastSection(true);
    bang->setColumnWidth(0, 50);
    bang->setColumnWidth(1, 80);
    bang->setColumnWidth(2, 130);
    bang->setColumnWidth(3, 100);
    bang->setColumnWidth(4, 110);
    bang->setColumnWidth(5, 100);
    layout->addWidget(bang, 1);

    // --- Hang nut thao tac chinh ---
    auto* nutThemThu = new QPushButton("➕ Thu nhập", this);
    auto* nutThemChi = new QPushButton("➖ Chi tiêu", this);
    auto* nutSua = new QPushButton("✏️ Sửa", this);
    auto* nutXoa = new QPushButton("🗑️ Xóa", this);
    connect(nutThemThu, &QPushButton::clicked, this, &MainWindow::themThuNhap);
    connect(nutThemChi, &QPushButton::clicked, this, &MainWindow::themChiTieu);
    connect(nutSua, &QPushButton::clicked, this, &MainWindow::suaGiaoDich);
    connect(nutXoa, &QPushButton::clicked, this, &MainWindow::xoaGiaoDich);

    auto* hang1 = new QHBoxLayout();
    hang1->addWidget(nutThemThu);
    hang1->addWidget(nutThemChi);
    hang1->addWidget(nutSua);
    hang1->addWidget(nutXoa);
    layout->addLayout(hang1);

    // --- Hang nut phu ---
    auto* nutLocDM = new QPushButton("Lọc theo danh mục", this);
    auto* nutLocNgay = new QPushButton("Lọc theo ngày", this);
    auto* nutBieuDo = new QPushButton("Biểu đồ chi tiêu", this);
    auto* nutTaiLai = new QPushButton("Tải lại", this);
    auto* nutDangXuat = new QPushButton("Đăng xuất", this);
    connect(nutLocDM, &QPushButton::clicked, this, &MainWindow::locTheoDanhMuc);
    connect(nutLocNgay, &QPushButton::clicked, this, &MainWindow::locTheoNgay);
    connect(nutBieuDo, &QPushButton::clicked, this, &MainWindow::xemThongKe);
    connect(nutTaiLai, &QPushButton::clicked, this, &MainWindow::taiLai);
    connect(nutDangXuat, &QPushButton::clicked, this, &MainWindow::xuLyDangXuat);

    auto* hang2 = new QHBoxLayout();
    hang2->addWidget(nutLocDM);
    hang2->addWidget(nutLocNgay);
    hang2->addWidget(nutBieuDo);
    hang2->addWidget(nutTaiLai);
    hang2->addStretch();
    hang2->addWidget(nutDangXuat);
    layout->addLayout(hang2);

    // --- Hang nut bao cao / xuat du lieu ---
    auto* nutBaoCao = new QPushButton("Báo cáo theo tháng", this);
    auto* nutCSV = new QPushButton("Xuất CSV (Excel)", this);
    connect(nutBaoCao, &QPushButton::clicked, this, &MainWindow::baoCaoTheoThang);
    connect(nutCSV, &QPushButton::clicked, this, &MainWindow::xuatCSV);

    auto* hang3 = new QHBoxLayout();
    hang3->addWidget(nutBaoCao);
    hang3->addWidget(nutCSV);
    hang3->addStretch();
    layout->addLayout(hang3);

    setCentralWidget(trungTam);
}

void MainWindow::napDuLieuChoUser(const QString& userId, const QString& tenHienThi) {
    this->userId = userId;
    ql = QuanLyChiTieu(); // reset du lieu cho user moi

    bool coVi = firebase->taiDuLieuNguoiDung(userId.toStdString(), ql);
    if (!coVi) {
        // Lan dau dung app -> hoi tao vi
        bool ok = false;
        QString tenVi = QInputDialog::getText(this, "Tạo ví",
            "Đây là lần đầu bạn sử dụng.\nĐặt tên cho ví của bạn:",
            QLineEdit::Normal, "Ví của " + tenHienThi, &ok);
        if (!ok || tenVi.isEmpty()) tenVi = "Ví của " + tenHienThi;

        ql.setTaiKhoan(TaiKhoan(tenVi.toStdString(), 0.0));
        luu();
    }

    setWindowTitle("Quản lý chi tiêu - " + tenHienThi);
    nhanTenVi->setText(QString::fromStdString(ql.getTaiKhoan().getTenVi()));
    capNhatBang();
    capNhatSoDu();
}

void MainWindow::capNhatBangVoiDanhSach(const std::vector<std::shared_ptr<GiaoDich>>& ds) {
    bang->setRowCount(static_cast<int>(ds.size()));
    for (int i = 0; i < static_cast<int>(ds.size()); ++i) {
        const auto& gd = ds[i];
        bool laThu = (std::dynamic_pointer_cast<ThuNhap>(gd) != nullptr);

        auto* itMa   = new QTableWidgetItem(QString::number(gd->getMaGiaoDich()));
        auto* itLoai = new QTableWidgetItem(QString::fromStdString(gd->layLoaiGiaoDich()));
        auto* itTien = new QTableWidgetItem(QString::fromStdString(
                            ConsoleHelper::dinhDangTien(gd->getSoTien())));
        auto* itNgay = new QTableWidgetItem(QString::fromStdString(gd->getNgay()));
        auto* itDM   = new QTableWidgetItem(QString::fromStdString(gd->getDanhMuc()));
        auto* itNC   = new QTableWidgetItem(QString::fromStdString(gd->getNguoiChi()));
        auto* itGC   = new QTableWidgetItem(QString::fromStdString(gd->getGhiChu()));

        QColor mau = laThu ? QColor("#27ae60") : QColor("#c0392b");
        itLoai->setForeground(mau);
        itTien->setForeground(mau);

        bang->setItem(i, 0, itMa);
        bang->setItem(i, 1, itLoai);
        bang->setItem(i, 2, itTien);
        bang->setItem(i, 3, itNgay);
        bang->setItem(i, 4, itDM);
        bang->setItem(i, 5, itNC);
        bang->setItem(i, 6, itGC);
    }
}

void MainWindow::capNhatBang() {
    nhanBoLoc->setVisible(false);
    nutXoaLoc->setVisible(false);
    capNhatBangVoiDanhSach(ql.layDanhSachGiaoDich());
}

void MainWindow::capNhatSoDu() {
    double soDu = ql.tinhSoDuHienTai();
    QString s = QString::fromStdString(ConsoleHelper::dinhDangTien(soDu));
    nhanSoDu->setText("Số dư hiện tại: " + s);
    // So du am -> in mau do
    if (soDu < 0)
        nhanSoDu->setStyleSheet("font-size: 16px; font-weight: bold; color: #c0392b;");
    else
        nhanSoDu->setStyleSheet("font-size: 16px; font-weight: bold; color: #2c3e50;");
}

void MainWindow::luu() {
    firebase->luuDuLieuNguoiDung(userId.toStdString(), ql);
}

int MainWindow::layMaDangChon() {
    int row = bang->currentRow();
    if (row < 0) return -1;
    QTableWidgetItem* it = bang->item(row, 0);
    if (!it) return -1;
    return it->text().toInt();
}

void MainWindow::themThuNhap() { themGiaoDichChung(true); }
void MainWindow::themChiTieu() { themGiaoDichChung(false); }

void MainWindow::themGiaoDichChung(bool laThuNhap) {
    // Thu thap danh muc va thanh vien da co de hien thi goi y thong minh
    QStringList danhMucHienCo, nguoiChiHienCo;
    for (const auto& g : ql.layDanhSachGiaoDich()) {
        QString dm = QString::fromStdString(g->getDanhMuc());
        if (!danhMucHienCo.contains(dm)) danhMucHienCo << dm;
        QString nc = QString::fromStdString(g->getNguoiChi());
        if (!nc.isEmpty() && !nguoiChiHienCo.contains(nc)) nguoiChiHienCo << nc;
    }

    TransactionDialog dlg(laThuNhap ? "Thêm thu nhập" : "Thêm chi tiêu",
                          danhMucHienCo, nguoiChiHienCo, this);
    if (dlg.exec() != QDialog::Accepted) return;

    int ma = ql.taoMaMoi();
    double st = dlg.laySoTien();
    std::string ngay = dlg.layNgay().toStdString();
    std::string dm = dlg.layDanhMuc().toStdString();
    std::string gc = dlg.layGhiChu().toStdString();
    std::string nc = dlg.layNguoiChi().toStdString();

    std::shared_ptr<GiaoDich> gd;
    if (laThuNhap)
        gd = std::make_shared<ThuNhap>(ma, st, ngay, dm, gc, nc);
    else
        gd = std::make_shared<ChiTieu>(ma, st, ngay, dm, gc, nc);

    ql.themGiaoDich(gd);
    luu();
    capNhatBang();
    capNhatSoDu();

    if (!laThuNhap && ql.tinhSoDuHienTai() < 0)
        QMessageBox::information(this, "Lưu ý",
            "Giao dịch đã được thêm. Số dư hiện tại đang ÂM.");
}

void MainWindow::suaGiaoDich() {
    int ma = layMaDangChon();
    if (ma < 0) {
        QMessageBox::information(this, "Sửa giao dịch",
            "Vui lòng chọn một giao dịch trong bảng trước.");
        return;
    }
    auto gd = ql.timGiaoDichTheoMa(ma);
    if (!gd) return;

    QStringList danhMucHienCo, nguoiChiHienCo;
    for (const auto& g : ql.layDanhSachGiaoDich()) {
        QString dm = QString::fromStdString(g->getDanhMuc());
        if (!danhMucHienCo.contains(dm)) danhMucHienCo << dm;
        QString nc = QString::fromStdString(g->getNguoiChi());
        if (!nc.isEmpty() && !nguoiChiHienCo.contains(nc)) nguoiChiHienCo << nc;
    }

    TransactionDialog dlg("Sửa giao dịch (mã " + QString::number(ma) +
                          ") - " + QString::fromStdString(gd->layLoaiGiaoDich()),
                          danhMucHienCo, nguoiChiHienCo, this);
    dlg.datGiaTri(gd->getSoTien(),
                  QString::fromStdString(gd->getNgay()),
                  QString::fromStdString(gd->getDanhMuc()),
                  QString::fromStdString(gd->getGhiChu()),
                  QString::fromStdString(gd->getNguoiChi()));
    if (dlg.exec() != QDialog::Accepted) return;

    ql.suaGiaoDichTheoMa(ma, dlg.laySoTien(), dlg.layNgay().toStdString(),
                         dlg.layDanhMuc().toStdString(), dlg.layGhiChu().toStdString(),
                         dlg.layNguoiChi().toStdString());
    luu();
    capNhatBang();
    capNhatSoDu();
}

void MainWindow::xoaGiaoDich() {
    int ma = layMaDangChon();
    if (ma < 0) {
        QMessageBox::information(this, "Xóa giao dịch",
            "Vui lòng chọn một giao dịch trong bảng trước.");
        return;
    }
    if (QMessageBox::question(this, "Xác nhận",
            "Bạn có chắc muốn xóa giao dịch mã " + QString::number(ma) + "?")
        != QMessageBox::Yes) return;

    ql.xoaGiaoDichTheoMa(ma);
    luu();
    capNhatBang();
    capNhatSoDu();
}

void MainWindow::locTheoDanhMuc() {
    QStringList dsList;
    for (const auto& gd : ql.layDanhSachGiaoDich()) {
        QString dm = QString::fromStdString(gd->getDanhMuc());
        if (!dm.isEmpty() && !dsList.contains(dm)) dsList << dm;
    }
    if (dsList.isEmpty()) {
        QMessageBox::information(this, "Lọc theo danh mục", "Chưa có giao dịch nào.");
        return;
    }

    bool ok = false;
    QString dm = QInputDialog::getItem(this, "Lọc theo danh mục",
        "Chọn danh mục cần lọc:", dsList, 0, false, &ok);
    if (!ok) return;

    auto ds = ql.locGiaoDichTheoDanhMuc(dm.toStdString());
    nhanBoLoc->setText("Đang lọc: danh mục \"" + dm + "\" — " + QString::number(ds.size()) + " kết quả");
    nhanBoLoc->setVisible(true);
    nutXoaLoc->setVisible(true);
    capNhatBangVoiDanhSach(ds);
}

void MainWindow::locTheoNgay() {
    QDialog dlg(this);
    dlg.setWindowTitle("Lọc theo ngày");
    auto* layout = new QVBoxLayout(&dlg);
    layout->addWidget(new QLabel("Chọn ngày cần lọc:", &dlg));

    auto* dateEdit = new QDateEdit(QDate::currentDate(), &dlg);
    dateEdit->setDisplayFormat("dd/MM/yyyy");
    dateEdit->setCalendarPopup(true);
    layout->addWidget(dateEdit);

    auto* buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dlg);
    buttons->button(QDialogButtonBox::Ok)->setText("Lọc");
    buttons->button(QDialogButtonBox::Cancel)->setText("Hủy");
    connect(buttons, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);
    layout->addWidget(buttons);

    if (dlg.exec() != QDialog::Accepted) return;
    QString ngay = dateEdit->date().toString("dd/MM/yyyy");

    auto ds = ql.locGiaoDichTheoNgay(ngay.toStdString());
    nhanBoLoc->setText("Đang lọc: ngày " + ngay + " — " + QString::number(ds.size()) + " kết quả");
    nhanBoLoc->setVisible(true);
    nutXoaLoc->setVisible(true);
    capNhatBangVoiDanhSach(ds);
}

void MainWindow::xemThongKe() {
    auto bangDanhMuc = ql.thongKeTheoDanhMuc();
    auto bangNguoiChi = ql.thongKeTheoNguoiChi();
    if (bangDanhMuc.empty()) {
        QMessageBox::information(this, "Biểu đồ chi tiêu",
            "Chưa có khoản chi tiêu nào để hiển thị biểu đồ.");
        return;
    }
    PieChartDialog dlg(bangDanhMuc, bangNguoiChi, this);
    dlg.exec();
}

void MainWindow::baoCaoTheoThang() {
    auto dsThangNam = ql.layDanhSachThangNam();
    if (dsThangNam.empty()) {
        QMessageBox::information(this, "Báo cáo tháng", "Chưa có giao dịch nào.");
        return;
    }

    // Xay dung danh sach chon thang
    QStringList dsList;
    for (const auto& [thang, nam] : dsThangNam)
        dsList << QString("Tháng %1/%2")
                  .arg(thang, 2, 10, QChar('0'))
                  .arg(nam);

    bool ok = false;
    QString chon = QInputDialog::getItem(this, "Báo cáo tháng",
        "Chọn tháng cần xem:", dsList, 0, false, &ok);
    if (!ok) return;

    int idx = dsList.indexOf(chon);
    auto [thang, nam] = dsThangNam[static_cast<size_t>(idx)];
    auto bc = ql.baoCaoTheoThang(thang, nam);

    QString noi = QString(
        "Báo cáo tháng %1/%2\n\n"
        "Tổng thu nhập :  %3\n"
        "Tổng chi tiêu :  %4\n"
        "──────────────────────────\n"
        "Chênh lệch    :  %5")
        .arg(thang, 2, 10, QChar('0')).arg(nam)
        .arg(QString::fromStdString(ConsoleHelper::dinhDangTien(bc.tongThu)))
        .arg(QString::fromStdString(ConsoleHelper::dinhDangTien(bc.tongChi)))
        .arg(QString::fromStdString(ConsoleHelper::dinhDangTien(bc.chenhLech)));

    QMessageBox::information(this,
        "Báo cáo tháng " + QString::number(thang) + "/" + QString::number(nam), noi);
}

void MainWindow::xuatCSV() {
    QString duongDanGoi = QFileDialog::getSaveFileName(
        this, "Lưu file CSV",
        QString("%1.csv").arg(userId),
        "CSV Files (*.csv);;All Files (*)");
    if (duongDanGoi.isEmpty()) return;

    bool ok = firebase->xuatCSV(userId.toStdString(), ql, duongDanGoi.toStdString());
    if (ok) {
        QMessageBox::information(this, "Xuất CSV thành công",
            "Đã lưu file:\n" + duongDanGoi +
            "\n\nMở bằng Excel: kéo thả file vào Excel hoặc File → Open.");
    } else {
        QMessageBox::warning(this, "Lỗi xuất CSV",
            "Không thể tạo file. Kiểm tra lại đường dẫn.");
    }
}

void MainWindow::taiLai() {
    QuanLyChiTieu qlMoi;
    bool ok = firebase->taiDuLieuNguoiDung(userId.toStdString(), qlMoi);
    if (!ok) {
        QMessageBox::warning(this, "Tải lại", "Không thể tải dữ liệu. Kiểm tra kết nối.");
        return;
    }
    ql = std::move(qlMoi);
    nhanTenVi->setText(QString::fromStdString(ql.getTaiKhoan().getTenVi()));
    capNhatBang();
    capNhatSoDu();
    QMessageBox::information(this, "Tải lại",
        QString("Đã tải lại %1 giao dịch từ %2.")
            .arg(static_cast<int>(ql.layDanhSachGiaoDich().size()))
            .arg(firebase->dangSuDungMock() ? "file local" : "Firebase"));
}

void MainWindow::xuLyDangXuat() {
    luu();
    auth->dangXuat();
    emit dangXuat();
}
