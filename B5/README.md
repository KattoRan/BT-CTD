# B5 — Scanner, Parser và Symbol Table

Thư mục này chứa mã nguồn cho bài tập tuần 5: xây dựng các thành phần của một trình biên dịch nhỏ (scanner, parser, symbol table).

Nội dung chính:

- `charcode.c`, `charcode.h` — hàm/ma trận mã ký tự hỗ trợ scanner
- `reader.c`, `reader.h` — đọc file nguồn theo từng ký tự
- `scanner.c`, `scanner.h` — lexical scanner (tách token)
- `token.c`, `token.h` — định nghĩa token và giúp xử lý token
- `parser.c`, `parser.h` — parser (cú pháp) cho ngôn ngữ mục tiêu
- `symtab.c`, `symtab.h` — bảng ký hiệu (symbol table) để lưu biến, hàm, ...
- `error.c`, `error.h` — xử lý lỗi và in thông báo lỗi
- `debug.c`, `debug.h` — tiện ích debug (nếu có)
- `main.c` — điểm vào để biên dịch/kiểm thử
- `Makefile` — cách biên dịch trên hệ Unix/MinGW
- `tests/` — tập ví dụ đầu vào (`.kpl`) để thử chương trình

Mục tiêu:

- Cài đặt scanner để chuyển mã nguồn thành token
- Cài đặt parser để kiểm tra cú pháp và xây dựng cấu trúc dữ liệu trung gian
- Xây dựng bảng ký hiệu để lưu thông tin các định danh

Hướng dẫn nhanh (Windows PowerShell):

```powershell
cd .\B5
make         # hoặc dùng gcc trực tiếp nếu không có make
# Ví dụ: gcc *.c -o main.exe
.\main.exe tests\example1.kpl
```

Gợi ý kiểm thử:

- Thư mục `tests/` chứa file ví dụ (`example*.kpl`). So sánh output với kết quả mong đợi nếu có.
- Khi gặp lỗi biên dịch, kiểm tra `charcode.c` / `reader.c` trước (các hàm I/O cơ bản).

Ghi chú:

- Nếu bạn muốn chạy trên Windows mà không có `make`, dùng GCC (MinGW/MSYS2) hoặc Visual Studio Developer Command Prompt.
- Thêm `README.md` này vào commit khi đã hoàn thành các phần chức năng.
