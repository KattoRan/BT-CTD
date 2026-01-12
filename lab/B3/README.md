# B3: Parser (Tuần 3)

## Mô tả chung

Thư mục `B3` là bài tập tuần 3, tập trung vào việc xây dựng **parser** (trình phân tích cú pháp) cho ngôn ngữ **KPL**.

Dự án bao gồm cả scanner (lexical analyzer) và parser, và có sẵn Makefile để biên dịch.

## Cấu trúc thư mục

```
B3/
├── scanner.c, scanner.h         # lexical scanner (tùy dùng)
├── parser.c, parser.h           # parser chính
├── main.c                       # entry point để chạy parser
├── reader.c, reader.h           # helper đọc file
├── token.c, token.h             # định nghĩa token
├── error.c, error.h             # xử lý lỗi
├── Makefile                      # build script
├── run_tests.ps1                 # script test (PowerShell)
├── parser.exe                    # binary (nếu đã build)
├── test/                         # thư mục chứa các case test
└── *.o                           # các object files
```

## Mục tiêu bài tập

- Viết hoặc hoàn thiện `parser.c` để chuyển dãy token thành cấu trúc cú pháp (AST hoặc biểu diễn trung gian).
- Báo lỗi cú pháp với vị trí (dòng, cột) rõ ràng.
- Tích hợp với scanner sẵn có để chạy end-to-end: source.kpl -> tokens -> parse -> output/diagnostics.

## Biên dịch

Dùng Makefile (khuyến nghị):

```powershell
cd .\B3
make
```

Nếu cần biên dịch thủ công (khi không có `make`):

```powershell
gcc -c reader.c -o reader.o
gcc -c token.c -o token.o
gcc -c scanner.c -o scanner.o
gcc -c parser.c -o parser.o
gcc -c error.c -o error.o
gcc -c main.c -o main.o
gcc reader.o token.o scanner.o parser.o error.o main.o -o parser.exe
```

## Chạy

- Chạy binary với một file nguồn KPL:

```powershell
.\parser.exe .\test\example1.kpl
```

- Chạy toàn bộ test cases (nếu có script):

```powershell
.\run_tests.ps1
```

Script `run_tests.ps1` sẽ so sánh output của chương trình với các file `result*.txt` tương ứng trong thư mục `test` và báo pass/fail.

## Gợi ý phát triển & debug

- Kiểm tra `token.h` và `scanner.c` để hiểu định dạng token đầu vào cho parser.
- Bắt đầu từ các quy tắc ngữ pháp đơn giản, viết parser theo phương pháp recursive descent nếu phù hợp.
- Ghi log/token stream ra file tạm để dễ debug.
- Dùng `make clean` trước khi build lại nếu gặp vấn đề với object cũ.

## Kiểm thử

- Thêm các trường hợp biên dịch lỗi vào `test/` (ví dụ: `err1.kpl`, `err1.result.txt`) để kiểm thử thông báo lỗi.
- So sánh kết quả parse (hoặc diagnostics) với file `result*.txt` để xác nhận.

## Lưu ý

- Nếu bạn sử dụng Windows mà chưa có `make`, cài `make` (MinGW/MSYS2) hoặc dùng lệnh gcc thủ công.
- Cập nhật `README.md` này nếu bạn thêm file/tiện ích mới vào `B3`.

---

**Cập nhật lần cuối:** 01/12/2025
