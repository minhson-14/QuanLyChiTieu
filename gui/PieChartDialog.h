#pragma once
#include <QDialog>
#include <QWidget>
#include <QColor>
#include <vector>
#include <map>
#include <set>
#include <string>

// Widget ve bieu do tron (pie chart) thuan bang QPainter — khong can them thu vien ngoai.
class PieChartWidget : public QWidget {
    Q_OBJECT
public:
    explicit PieChartWidget(const std::map<std::string, double>& duLieu,
                            QWidget* parent = nullptr);

    // An/hien mot lop: checked=true -> hien, false -> an
    void datHienThi(const QString& ten, bool hienThi);

    inline static const std::vector<QColor> BANG_MAU = {
        QColor("#e74c3c"), QColor("#3498db"), QColor("#2ecc71"),
        QColor("#f39c12"), QColor("#9b59b6"), QColor("#1abc9c"),
        QColor("#e67e22"), QColor("#e91e63"), QColor("#00bcd4"),
        QColor("#8bc34a")
    };

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    struct LopBieu { QString ten; double gia; QColor mau; };
    std::vector<LopBieu> danhSachLop;
    double tongCong;
    std::set<std::string> anhAn; // cac lop dang bi an
};

// Hop thoai bieu do tron co 2 tab:
//   Tab "Theo thể loại"  -> theoDanhMuc
//   Tab "Bởi thành viên" -> theoNguoiChi (hien thi thong bao neu trong)
class PieChartDialog : public QDialog {
    Q_OBJECT
public:
    PieChartDialog(const std::map<std::string, double>& theoDanhMuc,
                   const std::map<std::string, double>& theoNguoiChi,
                   QWidget* parent = nullptr);
};
