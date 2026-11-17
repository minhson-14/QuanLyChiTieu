#include "FirebaseService.h"
#include "ThuNhap.h"
#include "ChiTieu.h"

#include <fstream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <string>
#include <functional>
#include <filesystem>
#include <memory>

// Qt Network — dung cho che do Firebase that
#ifdef _WIN32
#include <windows.h>
#endif
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QEventLoop>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUrl>
#include <QByteArray>

namespace fs = std::filesystem;

// ============================================================
//  TIEN ICH JSON NOI BO
// ============================================================

static std::string escapeJson(const std::string& s) {
    std::string r;
    for (char c : s) {
        if (c == '\\' || c == '"') { r.push_back('\\'); r.push_back(c); }
        else if (c == '\n') { r += "\\n"; }
        else if (c == '\r') {}
        else r.push_back(c);
    }
    return r;
}

static std::string unescapeJson(const std::string& s) {
    std::string r;
    for (size_t i = 0; i < s.size(); ++i) {
        if (s[i] == '\\' && i + 1 < s.size()) {
            char n = s[i + 1];
            if (n == 'n') { r.push_back('\n'); ++i; }
            else { r.push_back(n); ++i; }
        } else {
            r.push_back(s[i]);
        }
    }
    return r;
}

static std::string trim(const std::string& s) {
    size_t a = s.find_first_not_of(" \t\r\n");
    if (a == std::string::npos) return "";
    size_t b = s.find_last_not_of(" \t\r\n");
    return s.substr(a, b - a + 1);
}

static bool dongCoKhoa(const std::string& line, const std::string& key) {
    return line.find("\"" + key + "\"") != std::string::npos;
}

static std::string phanGiaTri(const std::string& line) {
    size_t pos = line.find(':');
    if (pos == std::string::npos) return "";
    std::string v = trim(line.substr(pos + 1));
    if (!v.empty() && v.back() == ',') v.pop_back();
    return trim(v);
}

static std::string layGiaTriChuoi(const std::string& line) {
    std::string v = phanGiaTri(line);
    if (v.size() >= 2 && v.front() == '"' && v.back() == '"')
        v = v.substr(1, v.size() - 2);
    return unescapeJson(v);
}

static double layGiaTriSo(const std::string& line) {
    std::string v = phanGiaTri(line);
    try { return std::stod(v); } catch (...) { return 0; }
}

static std::string soRaChuoi(double v) {
    if (v == static_cast<double>(static_cast<long long>(v)))
        return std::to_string(static_cast<long long>(v));
    std::ostringstream ss;
    ss << std::fixed << std::setprecision(2) << v;
    return ss.str();
}

// Bam mat khau don gian (chi dung o che do mock)
static std::string hashMatKhau(const std::string& matKhau) {
    std::hash<std::string> h;
    return std::to_string(h(matKhau + "_muoi_co_dinh_demo"));
}

// ============================================================
//  TIEN ICH HTTP — goi Firebase REST API
// ============================================================

static std::string httpPost(const std::string& url, const std::string& body) {
    QNetworkAccessManager mgr;
    QNetworkRequest req(QUrl(QString::fromStdString(url)));
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QEventLoop loop;
    auto* reply = mgr.post(req, QByteArray(body.c_str(), static_cast<int>(body.size())));
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();
    std::string result = reply->readAll().toStdString();
    reply->deleteLater();
    return result;
}

static std::string httpGet(const std::string& url) {
    QNetworkAccessManager mgr;
    QNetworkRequest req(QUrl(QString::fromStdString(url)));
    QEventLoop loop;
    auto* reply = mgr.get(req);
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();
    std::string result = reply->readAll().toStdString();
    reply->deleteLater();
    return result;
}

static bool httpPut(const std::string& url, const std::string& body) {
    QNetworkAccessManager mgr;
    QNetworkRequest req(QUrl(QString::fromStdString(url)));
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QEventLoop loop;
    auto* reply = mgr.put(req, QByteArray(body.c_str(), static_cast<int>(body.size())));
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();
    bool ok = (reply->error() == QNetworkReply::NoError);
    reply->deleteLater();
    return ok;
}

// Trich xuat truong chuoi tu JSON phan hoi Firebase
static std::string jsonField(const std::string& json, const std::string& key) {
    QJsonDocument doc = QJsonDocument::fromJson(QByteArray::fromStdString(json));
    if (!doc.isObject()) return "";
    return doc.object().value(QString::fromStdString(key)).toString().toStdString();
}

static bool jsonCoLoi(const std::string& json) {
    QJsonDocument doc = QJsonDocument::fromJson(QByteArray::fromStdString(json));
    return doc.isObject() && doc.object().contains("error");
}

static std::string jsonLayLoi(const std::string& json) {
    QJsonDocument doc = QJsonDocument::fromJson(QByteArray::fromStdString(json));
    if (!doc.isObject()) return "Lỗi không xác định";
    QJsonValue err = doc.object().value("error");
    if (err.isObject())
        return err.toObject().value("message").toString().toStdString();
    return err.toString().toStdString();
}

// ============================================================
//  XAY DUNG / PHAN TICH JSON CHO DU LIEU NGUOI DUNG
//  (dung chung cho ca che do mock va Firebase that)
// ============================================================

static std::string buildUserJson(const QuanLyChiTieu& ql) {
    std::ostringstream f;
    const TaiKhoan& tk = ql.getTaiKhoan();
    f << "{\n";
    f << "  \"taiKhoan\": {\n";
    f << "    \"tenVi\": \""    << escapeJson(tk.getTenVi())     << "\",\n";
    f << "    \"soDuBanDau\": " << soRaChuoi(tk.getSoDuBanDau()) << "\n";
    f << "  },\n";
    f << "  \"maTiepTheo\": " << ql.getMaTiepTheo() << ",\n";
    f << "  \"giaoDich\": [\n";
    const auto& ds = ql.layDanhSachGiaoDich();
    for (size_t i = 0; i < ds.size(); ++i) {
        const auto& gd = ds[i];
        std::string loai = (std::dynamic_pointer_cast<ThuNhap>(gd) != nullptr) ? "ThuNhap" : "ChiTieu";
        f << "    {\n";
        f << "      \"maGiaoDich\": " << gd->getMaGiaoDich()           << ",\n";
        f << "      \"loai\": \""     << loai                          << "\",\n";
        f << "      \"soTien\": "     << soRaChuoi(gd->getSoTien())    << ",\n";
        f << "      \"ngay\": \""     << escapeJson(gd->getNgay())     << "\",\n";
        f << "      \"danhMuc\": \""  << escapeJson(gd->getDanhMuc())  << "\",\n";
        f << "      \"ghiChu\": \""   << escapeJson(gd->getGhiChu())   << "\",\n";
        f << "      \"nguoiChi\": \"" << escapeJson(gd->getNguoiChi()) << "\"\n";
        f << "    }" << (i + 1 < ds.size() ? "," : "") << "\n";
    }
    f << "  ]\n";
    f << "}\n";
    return f.str();
}

// Phan tich 1 object giao dich (dung chung cho ca 2 parser)
static void parseGiaoDichObject(const QJsonObject& obj, QuanLyChiTieu& ql) {
    int ma           = obj.value("maGiaoDich").toInt(0);
    std::string loai = obj.value("loai").toString().toStdString();
    double soTien    = obj.value("soTien").toDouble(0.0);
    std::string ngay = obj.value("ngay").toString().toStdString();
    std::string dm   = obj.value("danhMuc").toString().toStdString();
    std::string gc   = obj.value("ghiChu").toString().toStdString();
    std::string nc   = obj.value("nguoiChi").toString().toStdString();
    if (ma <= 0 || soTien <= 0) return; // bo qua ban ghi rong/loi

    if (loai == "ThuNhap")
        ql.themGiaoDich(std::make_shared<ThuNhap>(ma, soTien, ngay, dm, gc, nc));
    else
        ql.themGiaoDich(std::make_shared<ChiTieu>(ma, soTien, ngay, dm, gc, nc));
}

// Parser chinh: dung QJsonDocument de xu ly ca format Firebase (key alpha-sort,
// array -> object {"0":{...}}) lan format mock (line-by-line JSON chuan).
static bool parseUserJson(const std::string& content, QuanLyChiTieu& ql) {
    if (content.empty() || content == "null" ||
        content.find("\"error\"") != std::string::npos) return false;

    QJsonDocument doc = QJsonDocument::fromJson(QByteArray::fromStdString(content));
    if (!doc.isObject()) return false;

    QJsonObject root = doc.object();

    // Doc taiKhoan
    QJsonObject tkObj = root.value("taiKhoan").toObject();
    std::string tenVi = tkObj.value("tenVi").toString().toStdString();
    double soDuBanDau = tkObj.value("soDuBanDau").toDouble(0.0);
    if (tenVi.empty()) return false;

    int maTiepTheo = root.value("maTiepTheo").toInt(1);

    // Doc giaoDich: Firebase tra ve object {"0":{...},"1":{...}} thay vi array
    ql.xoaTatCaGiaoDich();
    QJsonValue gdVal = root.value("giaoDich");

    if (gdVal.isArray()) {
        for (const auto& item : gdVal.toArray())
            if (item.isObject()) parseGiaoDichObject(item.toObject(), ql);

    } else if (gdVal.isObject()) {
        // Firebase chuyen array thanh object voi khoa "0","1","2",...
        QJsonObject gdObj = gdVal.toObject();
        QStringList keys = gdObj.keys();
        // Sap xep theo so nguyen de dam bao thu tu giao dich
        std::sort(keys.begin(), keys.end(), [](const QString& a, const QString& b) {
            bool okA, okB;
            int ia = a.toInt(&okA), ib = b.toInt(&okB);
            if (okA && okB) return ia < ib;
            return a < b;
        });
        for (const QString& k : keys)
            if (gdObj[k].isObject()) parseGiaoDichObject(gdObj[k].toObject(), ql);
    }

    TaiKhoan tk;
    tk.setTenVi(tenVi);
    tk.setSoDuBanDau(soDuBanDau);
    ql.setTaiKhoan(tk);
    ql.setMaTiepTheo(maTiepTheo);
    return true;
}

// ============================================================
//  BAN GHI TAI KHOAN (chi dung cho che do mock)
// ============================================================

struct BanGhiTaiKhoan {
    std::string userId, email, matKhauHash, tenHienThi;
};

static bool docDanhSachTaiKhoan(const std::string& duongDan,
                                std::vector<BanGhiTaiKhoan>& ds, int& userTiepTheo) {
    ds.clear();
    userTiepTheo = 1;
    std::ifstream f(duongDan);
    if (!f.is_open()) return false;

    std::string line;
    bool trongMang = false, dangDoc = false;
    BanGhiTaiKhoan tam;
    while (std::getline(f, line)) {
        std::string t = trim(line);
        if (dongCoKhoa(t, "userTiepTheo")) { userTiepTheo = static_cast<int>(layGiaTriSo(t)); continue; }
        if (dongCoKhoa(t, "danhSachTaiKhoan")) { trongMang = true; continue; }
        if (!trongMang) continue;
        if (t.find('{') != std::string::npos) { tam = BanGhiTaiKhoan(); dangDoc = true; continue; }
        if (t.find('}') != std::string::npos) { if (dangDoc) { ds.push_back(tam); dangDoc = false; } continue; }
        if      (dongCoKhoa(t, "userId"))      tam.userId      = layGiaTriChuoi(t);
        else if (dongCoKhoa(t, "email"))       tam.email       = layGiaTriChuoi(t);
        else if (dongCoKhoa(t, "matKhauHash")) tam.matKhauHash = layGiaTriChuoi(t);
        else if (dongCoKhoa(t, "tenHienThi"))  tam.tenHienThi  = layGiaTriChuoi(t);
    }
    return true;
}

static void ghiDanhSachTaiKhoan(const std::string& duongDan,
                                const std::vector<BanGhiTaiKhoan>& ds, int userTiepTheo) {
    std::ofstream f(duongDan, std::ios::trunc);
    f << "{\n  \"userTiepTheo\": " << userTiepTheo << ",\n  \"danhSachTaiKhoan\": [\n";
    for (size_t i = 0; i < ds.size(); ++i) {
        f << "    {\n";
        f << "      \"userId\": \""      << escapeJson(ds[i].userId)      << "\",\n";
        f << "      \"email\": \""       << escapeJson(ds[i].email)       << "\",\n";
        f << "      \"matKhauHash\": \"" << escapeJson(ds[i].matKhauHash) << "\",\n";
        f << "      \"tenHienThi\": \""  << escapeJson(ds[i].tenHienThi)  << "\"\n";
        f << "    }" << (i + 1 < ds.size() ? "," : "") << "\n";
    }
    f << "  ]\n}\n";
}

// ============================================================
//  CAI DAT LOP FirebaseService
// ============================================================

FirebaseService::FirebaseService(bool suDungMock)
    : suDungMock(suDungMock), thuMucDuLieu("data") {
    // Tu dong tat mock neu co file cau hinh Firebase hop le
    if (docCauHinhFirebase())
        this->suDungMock = false;
}

bool FirebaseService::docCauHinhFirebase() {
    // Thu nhieu duong dan de ho tro moi cach khoi dong exe
    std::vector<std::string> paths = {
        "firebase_config.json",            // thu muc hien tai (double-click)
        "build-gui/firebase_config.json",  // chay qua build-gui.bat tu thu muc goc
        "../firebase_config.json",         // fallback
    };
#ifdef _WIN32
    // Duong dan chinh xac nhat: cung thu muc voi file .exe
    char buf[MAX_PATH] = {0};
    GetModuleFileNameA(nullptr, buf, MAX_PATH);
    std::string exePath(buf);
    auto pos = exePath.find_last_of("/\\");
    if (pos != std::string::npos)
        paths.insert(paths.begin(), exePath.substr(0, pos) + "\\firebase_config.json");
#endif

    std::ifstream f;
    for (const auto& p : paths) {
        f.open(p);
        if (f.is_open()) break;
    }
    if (!f.is_open()) return false;

    std::string line;
    while (std::getline(f, line)) {
        std::string t = trim(line);
        if (dongCoKhoa(t, "apiKey"))       apiKey      = layGiaTriChuoi(t);
        if (dongCoKhoa(t, "databaseUrl"))  databaseUrl = layGiaTriChuoi(t);
    }

    // Tra ve false neu van la gia tri mau
    return !apiKey.empty() && !databaseUrl.empty() &&
           apiKey.find("YOUR_") == std::string::npos;
}

bool FirebaseService::dangSuDungMock() const { return suDungMock; }
void FirebaseService::datCheDoMock(bool batMock) { suDungMock = batMock; }

std::string FirebaseService::duongDanFileUser(const std::string& userId) const {
    return thuMucDuLieu + "/" + userId + ".json";
}
std::string FirebaseService::duongDanFileTaiKhoan() const {
    return thuMucDuLieu + "/_accounts.json";
}
void FirebaseService::taoThuMucNeuChua() const {
    std::error_code ec;
    fs::create_directories(thuMucDuLieu, ec);
}

// ------------------------------------------------------------
//  XAC THUC
// ------------------------------------------------------------

bool FirebaseService::dangKyNguoiDung(const std::string& email, const std::string& matKhau,
                                      std::string& userIdRa, std::string& thongBaoLoi) {
    if (email.empty() || matKhau.empty()) {
        thongBaoLoi = "Email và mật khẩu không được để trống.";
        return false;
    }
    if (matKhau.size() < 6) {
        thongBaoLoi = "Mật khẩu phải có ít nhất 6 ký tự.";
        return false;
    }

    if (suDungMock) {
        // ----- CHE DO MOCK: luu vao file data/_accounts.json -----
        taoThuMucNeuChua();
        std::vector<BanGhiTaiKhoan> ds;
        int userTiepTheo = 1;
        docDanhSachTaiKhoan(duongDanFileTaiKhoan(), ds, userTiepTheo);

        for (const auto& bg : ds) {
            if (bg.email == email) { thongBaoLoi = "Email đã được đăng ký."; return false; }
        }

        BanGhiTaiKhoan moi;
        moi.userId = "user_" + std::to_string(userTiepTheo);
        moi.email = email;
        moi.matKhauHash = hashMatKhau(matKhau);
        size_t at = email.find('@');
        moi.tenHienThi = (at == std::string::npos) ? email : email.substr(0, at);

        ds.push_back(moi);
        ghiDanhSachTaiKhoan(duongDanFileTaiKhoan(), ds, userTiepTheo + 1);
        userIdRa = moi.userId;
        return true;

    } else {
        // ----- CHE DO FIREBASE THAT -----
        // POST https://identitytoolkit.googleapis.com/v1/accounts:signUp?key=API_KEY
        size_t at = email.find('@');
        std::string ten = (at == std::string::npos) ? email : email.substr(0, at);
        std::string body = "{\"email\":\"" + escapeJson(email) + "\","
                           "\"password\":\"" + escapeJson(matKhau) + "\","
                           "\"displayName\":\"" + escapeJson(ten) + "\","
                           "\"returnSecureToken\":true}";
        std::string url = "https://identitytoolkit.googleapis.com/v1/accounts:signUp?key=" + apiKey;
        std::string resp = httpPost(url, body);

        if (jsonCoLoi(resp)) {
            std::string msg = jsonLayLoi(resp);
            if (msg.find("EMAIL_EXISTS") != std::string::npos)
                thongBaoLoi = "Email đã được đăng ký.";
            else if (msg.find("WEAK_PASSWORD") != std::string::npos)
                thongBaoLoi = "Mật khẩu quá yếu (cần ít nhất 6 ký tự).";
            else
                thongBaoLoi = "Đăng ký thất bại: " + msg;
            return false;
        }

        userIdRa = jsonField(resp, "localId");
        idTokenHienTai = jsonField(resp, "idToken");
        return !userIdRa.empty();
    }
}

bool FirebaseService::dangNhapNguoiDung(const std::string& email, const std::string& matKhau,
                                        std::string& userIdRa, std::string& idTokenRa,
                                        std::string& tenHienThiRa, std::string& thongBaoLoi) {
    if (email.empty() || matKhau.empty()) {
        thongBaoLoi = "Email và mật khẩu không được để trống.";
        return false;
    }

    if (suDungMock) {
        // ----- CHE DO MOCK -----
        std::vector<BanGhiTaiKhoan> ds;
        int userTiepTheo = 1;
        if (!docDanhSachTaiKhoan(duongDanFileTaiKhoan(), ds, userTiepTheo)) {
            thongBaoLoi = "Chưa có tài khoản nào. Vui lòng đăng ký trước.";
            return false;
        }

        std::string h = hashMatKhau(matKhau);
        for (const auto& bg : ds) {
            if (bg.email == email) {
                if (bg.matKhauHash == h) {
                    userIdRa = bg.userId;
                    idTokenRa = "mock-token-" + bg.userId;
                    tenHienThiRa = bg.tenHienThi;
                    return true;
                }
                thongBaoLoi = "Sai mật khẩu.";
                return false;
            }
        }
        thongBaoLoi = "Email chưa được đăng ký.";
        return false;

    } else {
        // ----- CHE DO FIREBASE THAT -----
        // POST https://identitytoolkit.googleapis.com/v1/accounts:signInWithPassword?key=API_KEY
        std::string body = "{\"email\":\"" + escapeJson(email) + "\","
                           "\"password\":\"" + escapeJson(matKhau) + "\","
                           "\"returnSecureToken\":true}";
        std::string url = "https://identitytoolkit.googleapis.com/v1/accounts:signInWithPassword?key=" + apiKey;
        std::string resp = httpPost(url, body);

        if (jsonCoLoi(resp)) {
            std::string msg = jsonLayLoi(resp);
            if (msg.find("INVALID_PASSWORD") != std::string::npos ||
                msg.find("INVALID_LOGIN_CREDENTIALS") != std::string::npos ||
                msg.find("EMAIL_NOT_FOUND") != std::string::npos)
                thongBaoLoi = "Sai email hoặc mật khẩu.";
            else if (msg.find("USER_DISABLED") != std::string::npos)
                thongBaoLoi = "Tài khoản đã bị khoá.";
            else
                thongBaoLoi = "Đăng nhập thất bại: " + msg;
            return false;
        }

        userIdRa = jsonField(resp, "localId");
        idTokenRa = jsonField(resp, "idToken");
        idTokenHienTai = idTokenRa;

        tenHienThiRa = jsonField(resp, "displayName");
        if (tenHienThiRa.empty()) {
            size_t at = email.find('@');
            tenHienThiRa = (at == std::string::npos) ? email : email.substr(0, at);
        }
        return !userIdRa.empty();
    }
}

// ------------------------------------------------------------
//  DU LIEU NGUOI DUNG — users/{userId}/...
// ------------------------------------------------------------

void FirebaseService::luuDuLieuNguoiDung(const std::string& userId, const QuanLyChiTieu& ql) {
    std::string jsonStr = buildUserJson(ql);

    if (suDungMock) {
        // ----- CHE DO MOCK: ghi ra file local -----
        taoThuMucNeuChua();
        std::ofstream f(duongDanFileUser(userId), std::ios::trunc);
        f << jsonStr;

    } else {
        // ----- CHE DO FIREBASE THAT -----
        // PUT https://{databaseUrl}/users/{userId}.json?auth={idToken}
        std::string dbUrl = databaseUrl;
        if (!dbUrl.empty() && dbUrl.back() == '/') dbUrl.pop_back();
        std::string url = dbUrl + "/users/" + userId + ".json?auth=" + idTokenHienTai;
        httpPut(url, jsonStr);
    }
}

bool FirebaseService::taiDuLieuNguoiDung(const std::string& userId, QuanLyChiTieu& ql) {
    if (suDungMock) {
        // ----- CHE DO MOCK: doc tu file local -----
        std::ifstream f(duongDanFileUser(userId));
        if (!f.is_open()) return false;
        std::string content((std::istreambuf_iterator<char>(f)),
                             std::istreambuf_iterator<char>());
        return parseUserJson(content, ql);

    } else {
        // ----- CHE DO FIREBASE THAT -----
        // GET https://{databaseUrl}/users/{userId}.json?auth={idToken}
        std::string dbUrl = databaseUrl;
        if (!dbUrl.empty() && dbUrl.back() == '/') dbUrl.pop_back();
        std::string url = dbUrl + "/users/" + userId + ".json?auth=" + idTokenHienTai;
        std::string resp = httpGet(url);
        return parseUserJson(resp, ql);
    }
}

// ------------------------------------------------------------
//  CAC HAM PHU (delegate lai 2 ham chinh)
// ------------------------------------------------------------

bool FirebaseService::taiTaiKhoan(const std::string& userId, TaiKhoan& taiKhoanRa) {
    QuanLyChiTieu tmp;
    if (!taiDuLieuNguoiDung(userId, tmp)) return false;
    taiKhoanRa = tmp.getTaiKhoan();
    return true;
}

void FirebaseService::luuTaiKhoan(const std::string& userId, const TaiKhoan& taiKhoan) {
    QuanLyChiTieu tmp;
    taiDuLieuNguoiDung(userId, tmp);
    tmp.setTaiKhoan(taiKhoan);
    luuDuLieuNguoiDung(userId, tmp);
}

void FirebaseService::taiDanhSachGiaoDich(const std::string& userId, QuanLyChiTieu& ql) {
    QuanLyChiTieu tmp;
    if (!taiDuLieuNguoiDung(userId, tmp)) return;
    ql.xoaTatCaGiaoDich();
    for (const auto& gd : tmp.layDanhSachGiaoDich()) ql.themGiaoDich(gd);
    ql.setMaTiepTheo(tmp.getMaTiepTheo());
}

void FirebaseService::luuDanhSachGiaoDich(const std::string& userId, const QuanLyChiTieu& ql) {
    luuDuLieuNguoiDung(userId, ql);
}

void FirebaseService::dongBoDuLieu(const std::string& userId, const QuanLyChiTieu& ql) {
    luuDuLieuNguoiDung(userId, ql);
}

// ------------------------------------------------------------
//  XUAT CSV
// ------------------------------------------------------------

bool FirebaseService::xuatCSV(const std::string& userId, const QuanLyChiTieu& ql,
                              const std::string& duongDan) const {
    taoThuMucNeuChua();
    std::string path = duongDan.empty() ? (thuMucDuLieu + "/" + userId + ".csv") : duongDan;
    std::ofstream f(path, std::ios::trunc);
    if (!f.is_open()) return false;

    f << "\xEF\xBB\xBF"; // UTF-8 BOM de Excel nhan dien tieng Viet
    f << "Ma,Loai,So tien (VND),Ngay,Danh muc,Thanh vien,Ghi chu\n";

    auto escapeCSV = [](const std::string& s) -> std::string {
        bool canNgoac = s.find(',') != std::string::npos || s.find('"') != std::string::npos;
        if (!canNgoac) return s;
        std::string r = "\"";
        for (char c : s) { if (c == '"') r += "\"\""; else r += c; }
        return r + "\"";
    };

    for (const auto& gd : ql.layDanhSachGiaoDich()) {
        f << gd->getMaGiaoDich()                      << ","
          << escapeCSV(gd->layLoaiGiaoDich())         << ","
          << static_cast<long long>(gd->getSoTien())  << ","
          << escapeCSV(gd->getNgay())                 << ","
          << escapeCSV(gd->getDanhMuc())              << ","
          << escapeCSV(gd->getNguoiChi())             << ","
          << escapeCSV(gd->getGhiChu())               << "\n";
    }
    return true;
}
