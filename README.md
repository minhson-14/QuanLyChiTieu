# 💰 Phần mềm Quản lý chi tiêu cá nhân (Money+ thu nhỏ)

Đồ án môn **Lập trình hướng đối tượng (OOP)** — viết bằng **C++17**, chạy trên Windows
dưới dạng ứng dụng **console** (file `.exe`), giao diện tiếng Việt có dấu.

> Ghi chú về ngôn ngữ trong dự án:
> - **Giao diện hiển thị** cho người dùng: **tiếng Việt có dấu**.
> - **Toàn bộ comment trong code**: **tiếng Việt không dấu** (tránh lỗi font trên một số terminal Windows).

---

## 1. Mô tả dự án

Phần mềm giúp mỗi người dùng quản lý **một ví tiền** của riêng mình: nhập số tiền hiện
có ban đầu, ghi lại các khoản **thu nhập** / **chi tiêu**, theo dõi **số dư hiện tại**,
tìm kiếm – lọc – thống kê giao dịch. Dữ liệu của mỗi người dùng được **tách riêng theo
`userId`** và lưu lại để dùng cho những lần đăng nhập sau.

Tầng dữ liệu được thiết kế quanh lớp `FirebaseService`. Hiện tại chạy ở **chế độ mock**
(lưu file local trong thư mục `data/`), nhưng được thiết kế sẵn để **nâng cấp lên
Firebase Realtime Database thật** bằng REST API trong tương lai mà không phải sửa phần
còn lại của chương trình.

---

## 2. Chức năng chính

- 🔐 **Đăng ký / Đăng nhập / Đăng xuất** bằng email và mật khẩu.
- 👛 **Mỗi người dùng có đúng một ví**; lần đầu sử dụng sẽ nhập **tên ví** và **số dư ban đầu**.
- ➕ Thêm giao dịch **thu nhập** / **chi tiêu**.
- ✏️ **Sửa** giao dịch theo mã (số tiền, ngày, danh mục, ghi chú).
- 🗑️ **Xóa** giao dịch theo mã.
- 🔎 **Tìm** giao dịch theo mã; **lọc** theo danh mục / theo ngày.
- 📊 Xem **tổng thu nhập**, **tổng chi tiêu**, **số dư hiện tại**, **thống kê theo danh mục**.
- 🟥 Cho phép **số dư âm** khi chi tiêu vượt số dư; số dư âm hiển thị **màu đỏ** (nếu console hỗ trợ).
- ☁️ **Đồng bộ** dữ liệu (mock file / Firebase) và **lưu** khi thoát.

---

## 3. Công nghệ sử dụng

| Thành phần | Lựa chọn |
|---|---|
| Ngôn ngữ | C++17 (chỉ dùng thư viện chuẩn) |
| Trình biên dịch | g++ (MinGW-w64) hoặc Visual Studio |
| Giao diện | Console (UTF-8, màu ANSI) |
| Lưu trữ (mock) | File JSON local trong thư mục `data/` |
| Lưu trữ (tương lai) | Firebase Realtime Database qua REST API |

> Dự án **không dùng thư viện ngoài** nào. Phần JSON được tự viết tay đơn giản trong
> `FirebaseService.cpp` để không phụ thuộc thư viện thứ ba.

---

## 4. Cấu trúc thư mục

```
QuanLyChiTieu/
├── include/                 # Cac file header (.h)
│   ├── ConsoleHelper.h       # Tien ich console: UTF-8, mau, nhap lieu
│   ├── NguoiDung.h           # Thuc the nguoi dung
│   ├── UserSession.h         # Phien dang nhap trong RAM
│   ├── TaiKhoan.h            # Vi tien (tenVi, soDuBanDau)
│   ├── GiaoDich.h            # Lop CHA truu tuong cho giao dich
│   ├── ThuNhap.h             # Ke thua GiaoDich
│   ├── ChiTieu.h             # Ke thua GiaoDich
│   ├── QuanLyChiTieu.h       # Quan ly danh sach giao dich + thong ke
│   ├── AuthService.h         # Dang ky / dang nhap / dang xuat
│   └── FirebaseService.h     # Giao tiep Firebase (mock / that)
├── src/                     # Cac file nguon (.cpp) tuong ung
│   ├── ConsoleHelper.cpp
│   ├── NguoiDung.cpp
│   ├── UserSession.cpp
│   ├── TaiKhoan.cpp
│   ├── GiaoDich.cpp
│   ├── ThuNhap.cpp
│   ├── ChiTieu.cpp
│   ├── QuanLyChiTieu.cpp
│   ├── AuthService.cpp
│   └── FirebaseService.cpp
├── data/                    # (Tu dong tao khi chay) du lieu mock cua tung user
│   ├── _accounts.json        # Chi muc tai khoan (email -> userId, hash mat khau)
│   └── user_<n>.json         # Du lieu vi + giao dich cua tung user
├── main.cpp                 # Vong lap menu (ban CONSOLE)
├── gui/                     # Ma nguon ban GIAO DIEN (Qt)
│   ├── main_gui.cpp          # Diem vao ung dung Qt
│   ├── LoginWindow.h/.cpp    # Cua so dang nhap / dang ky
│   ├── MainWindow.h/.cpp     # Cua so chinh (bang giao dich)
│   └── TransactionDialog.h/.cpp  # Hop thoai them/sua giao dich
├── Makefile                 # Build ban console bang g++
├── build.bat                # Build nhanh ban console
├── CMakeLists.txt           # Build ban GUI Qt (CMake)
├── build-gui.bat            # Build nhanh ban GUI Qt (CMake + windeployqt)
├── README.md
├── BAOCAO.md                # Noi dung bao cao do an
└── SODO.md                  # Cac so do Mermaid
```

---

## 5. Cách biên dịch

### Cách 1 — dùng `build.bat` (Windows, đơn giản nhất)

Nhấp đúp vào `build.bat`, hoặc trong terminal:

```bat
build.bat
```

### Cách 2 — dùng `g++` trực tiếp

```bash
g++ -std=c++17 -static -static-libgcc -static-libstdc++ -Iinclude src/*.cpp main.cpp -o QuanLyChiTieu.exe
```

### Cách 3 — dùng `make`

```bash
make        # bien dich
make run    # bien dich va chay
make clean  # xoa file .exe
```

> **Vì sao có cờ `-static`?** Cờ này gói sẵn thư viện runtime của MinGW vào file
> `.exe`, giúp chương trình **chạy độc lập** mà không cần kèm theo các file DLL
> (`libstdc++-6.dll`, `libgcc_s_seh-1.dll`, `libwinpthread-1.dll`).

---

## 6. Cách chạy

```bat
QuanLyChiTieu.exe
```

Nếu chữ tiếng Việt bị lỗi font, hãy đặt console về UTF-8 trước khi chạy:

```bat
chcp 65001
QuanLyChiTieu.exe
```

> Nên dùng **Windows Terminal** hoặc **PowerShell** để màu và tiếng Việt hiển thị đẹp nhất.

---

## ⭐ 6B. Phiên bản giao diện đồ họa (Qt)

Ngoài bản console, dự án còn có **bản giao diện đồ họa viết bằng Qt 6**, **dùng lại
toàn bộ backend OOP** (`QuanLyChiTieu`, `GiaoDich`, `FirebaseService`...) — chỉ thay
tầng giao diện console bằng các cửa sổ Qt.

### Yêu cầu
- Qt 6.8.3 (mingw_64) + MinGW 13.1.0, đặt tại `C:\Qt`.
- Cài nhanh **không cần tài khoản Qt** (dùng `aqtinstall`):
  ```bash
  pip install aqtinstall
  python -m aqt install-qt   windows desktop 6.8.3 win64_mingw -O C:/Qt
  python -m aqt install-tool windows desktop tools_mingw1310    -O C:/Qt
  ```

### Biên dịch
```bat
build-gui.bat
```
Script tự chạy CMake, biên dịch và `windeployqt` (sao chép các DLL Qt) để chương trình
chạy độc lập. Hoặc làm thủ công:
```bash
cmake -S . -B build-gui -G "MinGW Makefiles" -DCMAKE_PREFIX_PATH="C:/Qt/6.8.3/mingw_64"
cmake --build build-gui
```

### Chạy
```bat
build-gui\QuanLyChiTieuGUI.exe
```
Có thể **double-click trực tiếp** vì các DLL Qt đã được đóng gói kèm theo.

### Các màn hình
- **Đăng nhập / Đăng ký**: nhập email + mật khẩu.
- **Cửa sổ chính**: hiện tên ví, **số dư** (màu đỏ nếu âm), **bảng giao dịch** (thu nhập
  màu xanh, chi tiêu màu đỏ), kèm các nút *Thu nhập / Chi tiêu / Sửa / Xóa / Lọc theo
  danh mục / Lọc theo ngày / Thống kê / Đồng bộ / Đăng xuất*.
- **Hộp thoại giao dịch**: nhập số tiền, chọn ngày (lịch), danh mục (gợi ý sẵn), ghi chú.

> Dữ liệu lưu trong `data/` cạnh thư mục chạy (ví dụ `build-gui/data/`), **cùng định dạng
> JSON** với bản console. Chạy cả hai bản từ cùng một thư mục thì chúng dùng chung dữ liệu.

---

## 7. Hướng dẫn test nhanh

1. Chạy `QuanLyChiTieu.exe`.
2. Chọn **1. Đăng ký** → nhập email `an@gmail.com`, mật khẩu `1234`.
3. Chọn **2. Đăng nhập** với tài khoản vừa tạo.
4. Lần đầu, nhập **tên ví** (vd `Ví của An`) và **số dư ban đầu** (vd `1000000`).
5. Chọn **2. Thêm thu nhập**: `5000000`, ngày `01/06/2026`, danh mục `Lương`.
6. Chọn **3. Thêm chi tiêu**: `7000000`, ngày `05/06/2026`, danh mục `Ăn uống`.
   → Số dư trở thành **-1.000.000 VND** (hiển thị **màu đỏ** nếu console hỗ trợ).
7. Chọn **5. Hiển thị tất cả giao dịch**, **13. Thống kê theo danh mục**, **12. Xem số dư**.
8. Chọn **16. Lưu và thoát**. Mở lại app, đăng nhập lại → **dữ liệu vẫn còn**.

Để kiểm tra **tách dữ liệu theo người dùng**, hãy đăng ký thêm `binh@gmail.com` và xác
nhận tài khoản này **không thấy** dữ liệu của `an@gmail.com`.

> **Tài khoản mock mẫu:** Dự án không cài sẵn tài khoản — bạn tự đăng ký ở lần chạy đầu.
> Mọi tài khoản và dữ liệu nằm trong thư mục `data/`. Muốn reset sạch, chỉ cần **xóa
> thư mục `data/`**.

---

## 8. Bật / tắt chế độ mock Firebase

Trong `main.cpp`, dòng khởi tạo dịch vụ:

```cpp
FirebaseService firebase(true);   // true = mock (file local), false = Firebase that
```

- `true`  → **chế độ mock**: đọc/ghi file trong thư mục `data/`.
- `false` → **chế độ Firebase thật**: hiện chưa triển khai (xem mục 12). Các vị trí cần
  viết REST API đã được đánh dấu bằng comment `TODO(Firebase that)` trong
  `FirebaseService.cpp`.

---

## 9. Bốn tính chất OOP (tóm tắt)

| Tính chất | Thể hiện ở đâu |
|---|---|
| **Đóng gói** | Mọi thuộc tính để `private`/`protected`, truy cập qua getter/setter; setter có kiểm tra ràng buộc (vd `GiaoDich::setSoTien` chỉ nhận giá trị > 0). |
| **Kế thừa** | `ThuNhap` và `ChiTieu` kế thừa từ lớp cha `GiaoDich`. |
| **Đa hình** | `std::vector<std::shared_ptr<GiaoDich>>` chứa cả hai loại; gọi `inThongTin()` / `layLoaiGiaoDich()` qua con trỏ lớp cha cho ra hành vi của lớp con. |
| **Trừu tượng** | `GiaoDich` là lớp trừu tượng với hàm thuần ảo `inThongTin() = 0` và `layLoaiGiaoDich() = 0`. |

Chi tiết kèm tên file/phương thức xem trong [BAOCAO.md](BAOCAO.md), mục *“Giải thích 4 tính chất OOP”*.

---

## 10. Cách nhập số dư ban đầu

Ngay sau khi **đăng nhập lần đầu** (chưa có ví), chương trình yêu cầu nhập **tên ví** và
**số tiền hiện có ban đầu** (`soDuBanDau`). Giá trị này được lưu vào lớp `TaiKhoan`.
Số dư hiện tại **không lưu cứng** mà luôn được **tính lại** theo công thức:

```
soDuHienTai = soDuBanDau + tổng thu nhập − tổng chi tiêu
```

Nhờ vậy, sau mỗi lần **thêm / sửa / xóa** giao dịch, số dư luôn chính xác.

---

## 11. Cách xử lý số dư âm

- Khi thêm **chi tiêu lớn hơn số dư**, chương trình **vẫn cho phép** — số dư có thể âm.
- Nếu `soDuHienTai < 0`:
  - Console **có** hỗ trợ màu → in số dư bằng **màu đỏ** (mã ANSI `\033[31m`).
  - Console **không** hỗ trợ màu → in số bình thường kèm cảnh báo `(!) SO DU AM`.
- Việc bật màu được xử lý trong `ConsoleHelper::thietLapConsole()` bằng cờ Windows
  `ENABLE_VIRTUAL_TERMINAL_PROCESSING`. Nếu bật được thì dùng màu, không thì tự fallback.

---

## 12. Hướng phát triển lên Firebase Realtime Database thật

Lớp `FirebaseService` là **điểm tập trung duy nhất** giao tiếp với tầng dữ liệu, nên việc
nâng cấp chỉ cần thay phần thân các hàm (khi `suDungMock == false`):

1. **Đăng ký / đăng nhập** → gọi **Firebase Authentication REST API**
   (`accounts:signUp`, `accounts:signInWithPassword`) để lấy `localId` (làm `userId`) và `idToken`.
2. **Đọc / ghi dữ liệu** → gọi **Realtime Database REST API** theo đường dẫn
   `users/{userId}.json?auth=<idToken>` (GET để tải, PUT/PATCH để lưu).
3. Phần **chuyển object C++ ↔ JSON** đã có sẵn ý tưởng trong code mock; khi lên thật chỉ
   cần thay file local bằng HTTP request (có thể dùng thư viện như `libcurl` + `nlohmann/json`).
4. **Không hardcode API key**: nên đọc từ **biến môi trường** hoặc file cấu hình riêng,
   không commit lên repository.

Mọi vị trí cần viết code thật đã được đánh dấu sẵn bằng comment `TODO(Firebase that)`
trong [src/FirebaseService.cpp](src/FirebaseService.cpp).

---

## 13. Tài liệu liên quan

- [BAOCAO.md](BAOCAO.md) — nội dung báo cáo đồ án đầy đủ.
- [SODO.md](SODO.md) — các sơ đồ Mermaid (khối, luồng xử lý, lớp UML, dữ liệu Firebase).
