#pragma once
#include <string>

// Lop tien ich ho tro console:
//  - Thiet lap bang ma UTF-8 de hien thi tieng Viet co dau
//  - Ho tro in mau ANSI (mau do cho so du am)
//  - Cac ham nhap lieu an toan (doc nguyen dong roi phan tich)
// Toan bo comment trong file viet bang tieng Viet KHONG dau de tranh loi font.
class ConsoleHelper {
private:
    // Co cho biet console hien tai co ho tro mau ANSI hay khong
    static bool coTheMau;

public:
    // Thiet lap console: bang ma UTF-8 + bat che do mau ANSI (neu duoc)
    static void thietLapConsole();

    // Tra ve true neu console ho tro in mau
    static bool hoTroMau();

    // Tra ve ma mau ANSI, hoac chuoi rong neu console khong ho tro mau
    static std::string mauDo();
    static std::string mauXanh();
    static std::string mauVang();
    static std::string resetMau();

    // In so du: neu am thi in mau do (hoac kem canh bao neu khong co mau)
    static void inSoDu(double soDu);

    // Dinh dang so tien dang 1.000.000 VND cho de doc
    static std::string dinhDangTien(double soTien);

    // Cac ham nhap lieu an toan
    static std::string docChuoi(const std::string& thongBao);
    static int docSoNguyen(const std::string& thongBao);
    static double docSoThuc(const std::string& thongBao);
    static double docSoTienDuong(const std::string& thongBao); // bat buoc > 0

    // In mot tieu de phu de menu de nhin
    static void inTieuDe(const std::string& tieuDe);

    // Tam dung cho nguoi dung doc ket qua truoc khi quay lai menu
    static void tamDung();
};
