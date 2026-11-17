// ============================================================
//  PHAN MEM QUAN LY CHI TIEU CA NHAN (Money+ thu nho)
//  Do an mon Lap trinh huong doi tuong (OOP) - C++17
//
//  Luu y ve ngon ngu:
//   - Giao dien hien thi cho nguoi dung: TIENG VIET CO DAU.
//   - Comment trong code: TIENG VIET KHONG DAU (tranh loi font terminal).
// ============================================================
#include <iostream>
#include <string>
#include <memory>
#include <map>

#include "ConsoleHelper.h"
#include "FirebaseService.h"
#include "AuthService.h"
#include "QuanLyChiTieu.h"
#include "GiaoDich.h"
#include "ThuNhap.h"
#include "ChiTieu.h"

// ------------------------------------------------------------
//  CAC HAM TIEN ICH NHAP LIEU CHO MENU
// ------------------------------------------------------------

// Kiem tra dinh dang ngay dd/mm/yyyy co ban
static bool ngayHopLe(const std::string& ngay) {
    if (ngay.size() != 10) return false;
    if (ngay[2] != '/' || ngay[5] != '/') return false;
    for (int i = 0; i < 10; ++i) {
        if (i == 2 || i == 5) continue;
        if (ngay[i] < '0' || ngay[i] > '9') return false;
    }
    int d = std::stoi(ngay.substr(0, 2));
    int m = std::stoi(ngay.substr(3, 2));
    int y = std::stoi(ngay.substr(6, 4));
    if (m < 1 || m > 12) return false;
    if (d < 1 || d > 31) return false;
    if (y < 1900 || y > 2200) return false;
    return true;
}

// Doc ngay, lap lai cho den khi dung dinh dang dd/mm/yyyy
static std::string docNgay(const std::string& thongBao) {
    while (true) {
        std::string ngay = ConsoleHelper::docChuoi(thongBao);
        if (ngayHopLe(ngay)) return ngay;
        std::cout << "  -> Ngày không hợp lệ. Vui lòng nhập theo định dạng dd/mm/yyyy.\n";
    }
}

// Cho nguoi dung chon danh muc tu danh sach goi y, hoac tu nhap
static std::string chonDanhMuc() {
    std::cout << "Chọn danh mục:\n"
              << "  1. Ăn uống\n"
              << "  2. Di chuyển\n"
              << "  3. Lương\n"
              << "  4. Học tập\n"
              << "  5. Giải trí\n"
              << "  6. Khác (tự nhập)\n";
    int c = ConsoleHelper::docSoNguyen("Lựa chọn danh mục: ");
    switch (c) {
        case 1: return "Ăn uống";
        case 2: return "Di chuyển";
        case 3: return "Lương";
        case 4: return "Học tập";
        case 5: return "Giải trí";
        default: return ConsoleHelper::docChuoi("Nhập tên danh mục: ");
    }
}

// ------------------------------------------------------------
//  CAC HAM XU LY CHUC NANG MENU
// ------------------------------------------------------------

static void xuLyXemVi(const QuanLyChiTieu& ql) {
    ConsoleHelper::inTieuDe("THÔNG TIN VÍ");
    ql.getTaiKhoan().inThongTin();
    std::cout << "Tổng thu nhập: " << ConsoleHelper::dinhDangTien(ql.tinhTongThuNhap()) << "\n";
    std::cout << "Tổng chi tiêu: " << ConsoleHelper::dinhDangTien(ql.tinhTongChiTieu()) << "\n";
    std::cout << "Số dư hiện tại: ";
    ConsoleHelper::inSoDu(ql.tinhSoDuHienTai());
    std::cout << "\n";
}

// laThuNhap = true -> them ThuNhap; false -> them ChiTieu
static void xuLyThemGiaoDich(QuanLyChiTieu& ql, bool laThuNhap) {
    ConsoleHelper::inTieuDe(laThuNhap ? "THÊM THU NHẬP" : "THÊM CHI TIÊU");
    double soTien = ConsoleHelper::docSoTienDuong("Nhập số tiền: ");
    std::string ngay = docNgay("Nhập ngày (dd/mm/yyyy): ");
    std::string danhMuc = chonDanhMuc();
    std::string ghiChu = ConsoleHelper::docChuoi("Nhập ghi chú (có thể bỏ trống): ");

    int ma = ql.taoMaMoi();
    std::shared_ptr<GiaoDich> gd;
    if (laThuNhap)
        gd = std::make_shared<ThuNhap>(ma, soTien, ngay, danhMuc, ghiChu);
    else
        gd = std::make_shared<ChiTieu>(ma, soTien, ngay, danhMuc, ghiChu);

    ql.themGiaoDich(gd);
    std::cout << "\n✓ Đã thêm giao dịch [Mã " << ma << "] thành công!\n";

    // Canh bao neu so du am sau khi them (vd chi tieu vuot so du)
    double soDu = ql.tinhSoDuHienTai();
    if (soDu < 0) {
        std::cout << "Lưu ý: Số dư hiện tại đang ÂM: ";
        ConsoleHelper::inSoDu(soDu);
        std::cout << "\n";
    }
}

static void xuLySuaGiaoDich(QuanLyChiTieu& ql) {
    ConsoleHelper::inTieuDe("SỬA GIAO DỊCH");
    int ma = ConsoleHelper::docSoNguyen("Nhập mã giao dịch cần sửa: ");
    auto gd = ql.timGiaoDichTheoMa(ma);
    if (!gd) {
        std::cout << "Không tìm thấy giao dịch có mã " << ma << ".\n";
        return;
    }
    std::cout << "Thông tin hiện tại:\n";
    gd->inThongTin();
    std::cout << "(Lưu ý: không thể đổi loại giao dịch \"" << gd->layLoaiGiaoDich()
              << "\", chỉ sửa thông tin bên trong.)\n";

    double soTien = ConsoleHelper::docSoTienDuong("Số tiền mới: ");
    std::string ngay = docNgay("Ngày mới (dd/mm/yyyy): ");
    std::string danhMuc = chonDanhMuc();
    std::string ghiChu = ConsoleHelper::docChuoi("Ghi chú mới (có thể bỏ trống): ");

    if (ql.suaGiaoDichTheoMa(ma, soTien, ngay, danhMuc, ghiChu))
        std::cout << "\n✓ Đã cập nhật giao dịch [Mã " << ma << "] thành công!\n";
    else
        std::cout << "Cập nhật thất bại.\n";
}

static void xuLyXoaGiaoDich(QuanLyChiTieu& ql) {
    ConsoleHelper::inTieuDe("XÓA GIAO DỊCH");
    int ma = ConsoleHelper::docSoNguyen("Nhập mã giao dịch cần xóa: ");
    if (ql.xoaGiaoDichTheoMa(ma))
        std::cout << "✓ Đã xóa giao dịch [Mã " << ma << "].\n";
    else
        std::cout << "Không tìm thấy giao dịch có mã " << ma << ".\n";
}

static void xuLyTimGiaoDich(const QuanLyChiTieu& ql) {
    ConsoleHelper::inTieuDe("TÌM GIAO DỊCH THEO MÃ");
    int ma = ConsoleHelper::docSoNguyen("Nhập mã giao dịch: ");
    auto gd = ql.timGiaoDichTheoMa(ma);
    if (gd) gd->inThongTin();
    else std::cout << "Không tìm thấy giao dịch có mã " << ma << ".\n";
}

static void xuLyLocDanhMuc(const QuanLyChiTieu& ql) {
    ConsoleHelper::inTieuDe("LỌC GIAO DỊCH THEO DANH MỤC");
    std::string dm = ConsoleHelper::docChuoi("Nhập danh mục cần lọc: ");
    auto ds = ql.locGiaoDichTheoDanhMuc(dm);
    if (ds.empty()) {
        std::cout << "Không có giao dịch nào thuộc danh mục \"" << dm << "\".\n";
        return;
    }
    for (const auto& gd : ds) gd->inThongTin();
    std::cout << "Tổng cộng: " << ds.size() << " giao dịch.\n";
}

static void xuLyLocNgay(const QuanLyChiTieu& ql) {
    ConsoleHelper::inTieuDe("LỌC GIAO DỊCH THEO NGÀY");
    std::string ngay = docNgay("Nhập ngày cần lọc (dd/mm/yyyy): ");
    auto ds = ql.locGiaoDichTheoNgay(ngay);
    if (ds.empty()) {
        std::cout << "Không có giao dịch nào trong ngày " << ngay << ".\n";
        return;
    }
    for (const auto& gd : ds) gd->inThongTin();
    std::cout << "Tổng cộng: " << ds.size() << " giao dịch.\n";
}

static void xuLyThongKe(const QuanLyChiTieu& ql) {
    ConsoleHelper::inTieuDe("THỐNG KÊ CHI TIÊU THEO DANH MỤC");
    auto bang = ql.thongKeTheoDanhMuc();
    if (bang.empty()) {
        std::cout << "Chưa có khoản chi tiêu nào để thống kê.\n";
        return;
    }
    for (const auto& cap : bang)
        std::cout << "  - " << cap.first << ": "
                  << ConsoleHelper::dinhDangTien(cap.second) << "\n";
}

// ------------------------------------------------------------
//  MENU CHINH
// ------------------------------------------------------------
static void inMenuChinh(const QuanLyChiTieu& ql, const std::string& email) {
    std::cout << "\n========== MENU CHÍNH (" << email << ") ==========\n";
    std::cout << "Số dư hiện tại: ";
    ConsoleHelper::inSoDu(ql.tinhSoDuHienTai());
    std::cout << "\n--------------------------------------------\n";
    std::cout << " 1. Xem thông tin ví\n";
    std::cout << " 2. Thêm giao dịch thu nhập\n";
    std::cout << " 3. Thêm giao dịch chi tiêu\n";
    std::cout << " 4. Sửa giao dịch theo mã\n";
    std::cout << " 5. Hiển thị tất cả giao dịch\n";
    std::cout << " 6. Xóa giao dịch theo mã\n";
    std::cout << " 7. Tìm giao dịch theo mã\n";
    std::cout << " 8. Lọc giao dịch theo danh mục\n";
    std::cout << " 9. Lọc giao dịch theo ngày\n";
    std::cout << "10. Xem tổng thu nhập\n";
    std::cout << "11. Xem tổng chi tiêu\n";
    std::cout << "12. Xem số dư hiện tại\n";
    std::cout << "13. Thống kê theo danh mục\n";
    std::cout << "14. Đồng bộ Firebase (mock/thật)\n";
    std::cout << "15. Đăng xuất\n";
    std::cout << "16. Lưu và thoát\n";
    std::cout << "============================================\n";
}

// Tra ve true neu nguoi dung chon "Luu va thoat" (thoat han chuong trinh);
// tra ve false neu chi dang xuat (quay lai menu ban dau).
static bool chayMenuChinh(AuthService& auth, FirebaseService& firebase,
                          QuanLyChiTieu& ql, const std::string& userId) {
    while (auth.kiemTraDangNhap()) {
        inMenuChinh(ql, auth.layPhienHienTai().getEmail());
        int chon = ConsoleHelper::docSoNguyen("Lựa chọn của bạn: ");

        switch (chon) {
            case 1: xuLyXemVi(ql); break;
            case 2: xuLyThemGiaoDich(ql, true);  firebase.luuDuLieuNguoiDung(userId, ql); break;
            case 3: xuLyThemGiaoDich(ql, false); firebase.luuDuLieuNguoiDung(userId, ql); break;
            case 4: xuLySuaGiaoDich(ql);         firebase.luuDuLieuNguoiDung(userId, ql); break;
            case 5: ConsoleHelper::inTieuDe("TẤT CẢ GIAO DỊCH"); ql.hienThiTatCaGiaoDich(); break;
            case 6: xuLyXoaGiaoDich(ql);         firebase.luuDuLieuNguoiDung(userId, ql); break;
            case 7: xuLyTimGiaoDich(ql); break;
            case 8: xuLyLocDanhMuc(ql); break;
            case 9: xuLyLocNgay(ql); break;
            case 10:
                ConsoleHelper::inTieuDe("TỔNG THU NHẬP");
                std::cout << ConsoleHelper::dinhDangTien(ql.tinhTongThuNhap()) << "\n";
                break;
            case 11:
                ConsoleHelper::inTieuDe("TỔNG CHI TIÊU");
                std::cout << ConsoleHelper::dinhDangTien(ql.tinhTongChiTieu()) << "\n";
                break;
            case 12:
                ConsoleHelper::inTieuDe("SỐ DƯ HIỆN TẠI");
                ConsoleHelper::inSoDu(ql.tinhSoDuHienTai());
                std::cout << "\n";
                break;
            case 13: xuLyThongKe(ql); break;
            case 14:
                firebase.dongBoDuLieu(userId, ql);
                std::cout << "✓ Đã đồng bộ dữ liệu (chế độ "
                          << (firebase.dangSuDungMock() ? "mock - file local" : "Firebase thật")
                          << ").\n";
                break;
            case 15:
                firebase.luuDuLieuNguoiDung(userId, ql);
                auth.dangXuat();
                std::cout << "✓ Đã đăng xuất. Dữ liệu đã được lưu.\n";
                return false; // quay lai menu ban dau
            case 16:
                firebase.luuDuLieuNguoiDung(userId, ql);
                std::cout << "✓ Đã lưu dữ liệu. Tạm biệt!\n";
                return true; // thoat han chuong trinh
            default:
                std::cout << "Lựa chọn không hợp lệ. Vui lòng chọn lại.\n";
        }

        // Tam dung sau cac chuc nang xem/thao tac (tru dang xuat/thoat da return)
        if (chon >= 1 && chon <= 14) ConsoleHelper::tamDung();
    }
    return false;
}

// ------------------------------------------------------------
//  HAM MAIN
// ------------------------------------------------------------
int main() {
    // Thiet lap console UTF-8 + mau ngay khi khoi dong
    ConsoleHelper::thietLapConsole();

    // Khoi tao dich vu Firebase o che do MOCK (luu file local trong data/)
    FirebaseService firebase(true);
    AuthService auth(&firebase);

    if (!ConsoleHelper::hoTroMau()) {
        std::cout << "(Lưu ý: Console hiện không hỗ trợ màu. Số dư âm sẽ kèm cảnh báo "
                     "thay vì hiển thị màu đỏ.)\n";
    }

    bool dangChay = true;
    while (dangChay) {
        // ----- Menu ban dau -----
        std::cout << "\n===== PHẦN MỀM QUẢN LÝ CHI TIÊU CÁ NHÂN =====\n";
        std::cout << "1. Đăng ký tài khoản\n";
        std::cout << "2. Đăng nhập\n";
        std::cout << "3. Thoát\n";
        int chon = ConsoleHelper::docSoNguyen("Lựa chọn của bạn: ");

        if (chon == 1) {
            // ----- Dang ky -----
            std::string email = ConsoleHelper::docChuoi("Nhập email: ");
            std::string matKhau = ConsoleHelper::docChuoi("Nhập mật khẩu: ");
            std::string loi;
            if (auth.dangKy(email, matKhau, loi))
                std::cout << "✓ Đăng ký thành công! Hãy đăng nhập để bắt đầu.\n";
            else
                std::cout << "✗ Đăng ký thất bại: " << loi << "\n";

        } else if (chon == 2) {
            // ----- Dang nhap -----
            std::string email = ConsoleHelper::docChuoi("Nhập email: ");
            std::string matKhau = ConsoleHelper::docChuoi("Nhập mật khẩu: ");
            std::string loi;
            if (!auth.dangNhap(email, matKhau, loi)) {
                std::cout << "✗ Đăng nhập thất bại: " << loi << "\n";
                continue;
            }
            std::cout << "✓ Đăng nhập thành công! Xin chào "
                      << auth.layPhienHienTai().getTenHienThi() << ".\n";

            // ----- Tai du lieu cua dung user nay -----
            std::string userId = auth.layPhienHienTai().getUserId();
            QuanLyChiTieu ql;
            bool coDuLieu = firebase.taiDuLieuNguoiDung(userId, ql);

            if (!coDuLieu) {
                // Lan dau dung app -> tao vi
                std::cout << "\nĐây là lần đầu bạn sử dụng. Hãy tạo ví của bạn.\n";
                std::string tenVi = ConsoleHelper::docChuoi("Đặt tên ví: ");
                if (tenVi.empty()) tenVi = "Ví của tôi";
                double soDuBanDau = ConsoleHelper::docSoThuc("Nhập số tiền hiện có ban đầu trong ví: ");
                ql.setTaiKhoan(TaiKhoan(tenVi, soDuBanDau));
                firebase.luuDuLieuNguoiDung(userId, ql);
                std::cout << "✓ Đã tạo ví \"" << tenVi << "\" với số dư ban đầu "
                          << ConsoleHelper::dinhDangTien(soDuBanDau) << ".\n";
            }

            // ----- Vao menu chinh -----
            bool thoatHan = chayMenuChinh(auth, firebase, ql, userId);
            if (thoatHan) dangChay = false;

        } else if (chon == 3) {
            dangChay = false;
            std::cout << "Tạm biệt! Hẹn gặp lại.\n";
        } else {
            std::cout << "Lựa chọn không hợp lệ. Vui lòng chọn lại.\n";
        }
    }

    return 0;
}
