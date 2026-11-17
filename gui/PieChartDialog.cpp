#include "PieChartDialog.h"
#include "ConsoleHelper.h"

#include <QPainter>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFrame>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QTabWidget>
#include <QScrollArea>
#include <QCheckBox>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// ============================================================
//  PieChartWidget
// ============================================================

PieChartWidget::PieChartWidget(const std::map<std::string, double>& duLieu, QWidget* parent)
    : QWidget(parent), tongCong(0.0) {
    setMinimumSize(280, 280);
    int i = 0;
    for (const auto& [dm, gt] : duLieu) {
        if (gt > 0) {
            danhSachLop.push_back({QString::fromStdString(dm), gt,
                                   BANG_MAU[i % static_cast<int>(BANG_MAU.size())]});
            tongCong += gt;
            ++i;
        }
    }
}

void PieChartWidget::datHienThi(const QString& ten, bool hienThi) {
    if (hienThi) anhAn.erase(ten.toStdString());
    else         anhAn.insert(ten.toStdString());
    update();
}

void PieChartWidget::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    // Tinh tong chi tu cac lop dang hien thi
    double tongHienThi = 0;
    for (const auto& lop : danhSachLop)
        if (!anhAn.count(lop.ten.toStdString())) tongHienThi += lop.gia;

    if (tongHienThi <= 0) {
        p.drawText(rect(), Qt::AlignCenter,
                   danhSachLop.empty() ? "Không có dữ liệu" : "Không có mục nào đang hiển thị");
        return;
    }

    int side = qMin(width(), height()) - 20;
    int ox = (width() - side) / 2;
    int oy = 10;
    QRect pieRect(ox, oy, side, side);

    // Qt: goc 0 = 3 gio (phai), CCW. Bat dau tu 12 gio = 90 * 16.
    double startAngle = 90.0 * 16.0;

    for (const auto& lop : danhSachLop) {
        if (anhAn.count(lop.ten.toStdString())) continue; // bo qua lop dang an

        double span = lop.gia / tongHienThi * 360.0 * 16.0;
        p.setBrush(lop.mau);
        p.setPen(QPen(Qt::white, 1.5));
        p.drawPie(pieRect, qRound(startAngle), -qRound(span));

        double pct = lop.gia / tongHienThi;
        if (pct >= 0.04) {
            double midDeg = startAngle / 16.0 - span / 16.0 / 2.0;
            double midRad = midDeg * M_PI / 180.0;
            double r = side / 2.0 * 0.60;
            int cx = ox + side / 2 + qRound(r * std::cos(midRad));
            int cy = oy + side / 2 - qRound(r * std::sin(midRad));

            QFont f = p.font();
            f.setPointSize(8);
            f.setBold(true);
            p.setFont(f);
            p.setPen(Qt::white);
            p.drawText(QRect(cx - 24, cy - 9, 48, 18), Qt::AlignCenter,
                       QString::number(pct * 100.0, 'f', 1) + "%");
        }
        startAngle -= span;
    }
}

// ============================================================
//  Ham tien ich: tao 1 trang tab tu bo du lieu
// ============================================================

static QWidget* taoTrang(const std::map<std::string, double>& duLieu, QWidget* parent) {
    auto* trang = new QWidget(parent);
    auto* layout = new QVBoxLayout(trang);

    if (duLieu.empty()) {
        auto* thongBao = new QLabel("Chưa có dữ liệu cho chế độ xem này.\n"
                                   "(Hãy thêm giao dịch và điền thành viên để xem biểu đồ này.)",
                                   trang);
        thongBao->setAlignment(Qt::AlignCenter);
        thongBao->setStyleSheet("color:#888; padding:40px;");
        layout->addWidget(thongBao);
        return trang;
    }

    double tongTatCa = 0;
    for (const auto& [dm, gt] : duLieu) if (gt > 0) tongTatCa += gt;

    auto* pie = new PieChartWidget(duLieu, trang);
    layout->addWidget(pie, 1);

    // --- Chu giai (co the click de an/hien lop) ---
    auto* legendWidget = new QWidget(trang);
    auto* legendLayout = new QVBoxLayout(legendWidget);
    legendLayout->setSpacing(4);
    legendLayout->setContentsMargins(8, 4, 8, 4);

    int i = 0;
    for (const auto& [dm, gt] : duLieu) {
        if (gt <= 0) { ++i; continue; }

        QColor mau = PieChartWidget::BANG_MAU[i % static_cast<int>(PieChartWidget::BANG_MAU.size())];
        double pct = tongTatCa > 0 ? gt / tongTatCa * 100.0 : 0.0;

        auto* row = new QHBoxLayout();
        row->setSpacing(8);

        // Checkbox: indicator mau theo danh muc, click de an/hien lop tren bieu do
        auto* cb = new QCheckBox(legendWidget);
        cb->setChecked(true);
        cb->setCursor(Qt::PointingHandCursor);
        cb->setToolTip("Click để ẩn/hiện mục này trên biểu đồ");
        cb->setStyleSheet(QString(
            "QCheckBox::indicator { width:14px; height:14px; border-radius:3px; }"
            "QCheckBox::indicator:checked   { background:%1; }"
            "QCheckBox::indicator:unchecked { background:#444; border:1px solid #666; }"
        ).arg(mau.name()));

        auto* tenLabel  = new QLabel(QString::fromStdString(dm), legendWidget);
        auto* tienLabel = new QLabel(
            QString::fromStdString(ConsoleHelper::dinhDangTien(gt)), legendWidget);
        tienLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        auto* pctLabel  = new QLabel(
            QString("(%1%)").arg(QString::number(pct, 'f', 1)), legendWidget);
        pctLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        pctLabel->setStyleSheet("color:#777;");

        // Khi toggle: cap nhat bieu do + gray out cac nhan
        QString dmStr = QString::fromStdString(dm);
        QObject::connect(cb, &QCheckBox::toggled,
                         [pie, tenLabel, tienLabel, pctLabel, dmStr](bool checked) {
            pie->datHienThi(dmStr, checked);
            tenLabel->setEnabled(checked);
            tienLabel->setEnabled(checked);
            pctLabel->setEnabled(checked);
        });

        row->addWidget(cb);
        row->addWidget(tenLabel, 1);
        row->addWidget(tienLabel);
        row->addSpacing(4);
        row->addWidget(pctLabel);
        legendLayout->addLayout(row);
        ++i;
    }
    legendLayout->addStretch();
    layout->addWidget(legendWidget);

    // --- Tong cong ---
    auto* sep = new QFrame(trang);
    sep->setFrameShape(QFrame::HLine);
    sep->setFrameShadow(QFrame::Sunken);
    layout->addWidget(sep);

    auto* tongRow = new QHBoxLayout();
    tongRow->addWidget(new QLabel("<b>Tổng chi tiêu:</b>", trang), 1);
    auto* tongLabel = new QLabel(
        "<b>" + QString::fromStdString(ConsoleHelper::dinhDangTien(tongTatCa)) + "</b>", trang);
    tongLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    tongRow->addWidget(tongLabel);
    layout->addLayout(tongRow);

    return trang;
}

// ============================================================
//  PieChartDialog
// ============================================================

PieChartDialog::PieChartDialog(const std::map<std::string, double>& theoDanhMuc,
                               const std::map<std::string, double>& theoNguoiChi,
                               QWidget* parent)
    : QDialog(parent) {
    setWindowTitle("Biểu đồ chi tiêu");
    setMinimumWidth(480);

    auto* tabs = new QTabWidget(this);
    tabs->addTab(taoTrang(theoDanhMuc, this), "Theo thể loại");
    tabs->addTab(taoTrang(theoNguoiChi, this), "Bởi thành viên");

    auto* nutDong = new QDialogButtonBox(QDialogButtonBox::Close, this);
    nutDong->button(QDialogButtonBox::Close)->setText("Đóng");
    connect(nutDong, &QDialogButtonBox::rejected, this, &QDialog::reject);

    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(tabs, 1);
    mainLayout->addWidget(nutDong);
}
