# B6 - Semantic Analysis

Thư mục này chứa bài tập tuần 6: Phân tích ngữ nghĩa (Semantic Analysis) cho ngôn ngữ KPL.

## Mô tả bài tập

Bài tập này tập trung vào việc xây dựng module phân tích ngữ nghĩa, kiểm tra các lỗi ngữ nghĩa như:

- Khai báo biến trùng lặp
- Sử dụng biến chưa khai báo
- Kiểm tra kiểu dữ liệu
- Xác minh các quy tắc ngữ nghĩa khác

## Cấu trúc file

- `semantics.c`, `semantics.h`: Module phân tích ngữ nghĩa chính
- `symtab.c`, `symtab.h`: Bảng ký hiệu (Symbol Table)
- `parser.c`, `parser.h`: Bộ phân tích cú pháp
- `scanner.c`, `scanner.h`: Bộ quét từ vựng
- `token.c`, `token.h`: Định nghĩa token
- `charcode.c`, `charcode.h`: Xử lý ký tự
- `error.c`, `error.h`: Xử lý lỗi
- `debug.c`, `debug.h`: Công cụ debug
- `main.c`: File chính để chạy chương trình
- `Makefile`: File build
- `run_tests.ps1`: Script chạy test
- `tests/`: Thư mục chứa các file test .kpl và kết quả mong đợi

## Cách chạy

### Biên dịch

```bash
make
```

### Chạy test

```powershell
.\run_tests.ps1
```

### Chạy với file cụ thể

```bash
./main <file.kpl>
```

## Các file test

Thư mục `tests/` chứa các file ví dụ:

- `example1.kpl` đến `example6.kpl`: Các chương trình KPL mẫu
- `result*.txt`: Kết quả mong đợi
- `my_output*.txt`: Kết quả thực tế (sẽ được tạo khi chạy test)

## Lưu ý

- Đảm bảo đã cài đặt GCC hoặc MinGW để biên dịch C
- Sử dụng PowerShell để chạy script test trên Windows
