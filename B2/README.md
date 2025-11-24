# B2: Lexical Scanner (Tuần 2)

## Mô tả bài tập

Bài tập này yêu cầu xây dựng một **lexical scanner** (hay còn gọi là tokenizer) cho ngôn ngữ lập trình **KPL** (Kiet Programming Language).

Nhiệm vụ chính:

- Đọc tệp tin nguồn `.kpl`
- Nhận dạng các token (từ khóa, định danh, operators, literals, ...)
- Xuất danh sách token với thông tin chi tiết (dòng, cột, loại token, giá trị)

## Cấu trúc dự án

```
B2/
├── charcode.c, charcode.h       # Xử lý các ký tự đặc biệt
├── reader.c, reader.h           # Đọc file và quản lý buffer
├── scanner.c, scanner.exe       # Hàm scanner chính
├── token.c, token.h             # Định nghĩa và quản lý token
├── error.c, error.h             # Xử lý lỗi
├── Makefile                      # Build script
├── run_tests.ps1                 # Script test tự động (PowerShell)
├── test/                         # Thư mục test
│   ├── example1.kpl, result1.txt
│   ├── example2.kpl, result2.txt
│   ├── ...
│   ├── my_output1.txt, my_output2.txt, ... (kết quả của bạn)
│   └── ...
```

## Các bước thực hiện

### 1. Biên dịch dự án

Sử dụng Makefile để biên dịch:

```powershell
cd .\B2
make
```

Hoặc biên dịch thủ công (nếu không có make):

```powershell
gcc -c charcode.c -o charcode.o
gcc -c reader.c -o reader.o
gcc -c token.c -o token.o
gcc -c error.c -o error.o
gcc -c scanner.c -o scanner.o
gcc charcode.o reader.o token.o error.o scanner.o -o scanner.exe
```

### 2. Chạy scanner với file test

```powershell
# Chạy scanner với tệp input
.\scanner.exe .\test\example1.kpl

# Kết quả được ghi ra file output (tên file được xác định bằng tên input)
# Ví dụ: example1.kpl -> my_output1.txt (nếu có cơ chế đặt tên)
```

### 3. Chạy bộ test tự động

```powershell
# Chạy tất cả test case
.\run_tests.ps1
```

Script này sẽ:

- Chạy scanner cho từng file `.kpl` trong `test/`
- So sánh kết quả `my_output*.txt` với `result*.txt`
- Báo cáo các test pass/fail

## File input/output

### Input (`.kpl`)

- Các file `example1.kpl`, `example2.kpl`, ... là file lập trình nguồn KPL cần được scan.

### Output

- `my_output1.txt`, `my_output2.txt`, ... là kết quả scan của bạn.
- `result1.txt`, `result2.txt`, ... là kết quả mong đợi (để so sánh).

Nếu `my_output*.txt == result*.txt`, test pass ✓

## Lưu ý kỹ thuật

### Token Types (có thể thay đổi tùy yêu cầu)

- **Từ khóa** (Keywords): `int`, `float`, `if`, `else`, `while`, ...
- **Định danh** (Identifiers): tên biến, hàm, ...
- **Hằng số** (Literals): số nguyên, số thực, chuỗi, ...
- **Operators**: `+`, `-`, `*`, `/`, `=`, `==`, `!=`, ...
- **Delimiters**: `(`, `)`, `{`, `}`, `;`, `,`, ...
- **Comment**: bỏ qua hoặc báo cáo

### Xử lý lỗi

- Các ký tự không hợp lệ → báo lỗi hoặc bỏ qua tùy yêu cầu
- Ghi lại vị trí lỗi (dòng, cột) để hỗ trợ debug

## Cách thực hiện nhanh nhất

1. **Biên dịch:**

   ```powershell
   cd B2; make
   ```

2. **Test một file:**

   ```powershell
   .\scanner.exe .\test\example1.kpl
   ```

3. **Test tất cả:**
   ```powershell
   .\run_tests.ps1
   ```

## Gợi ý debug

- Nếu build thất bại, kiểm tra các file header có tồn tại không.
- Nếu output sai, kiểm tra xử lý comment, string, số thực.
- Dùng lệnh `make clean` để xóa file object cũ trước khi rebuild.

## Tài liệu thêm

- Xem lại `scanner.c`, `token.h` để hiểu cách token được định nghĩa.
- Xem `reader.c` để hiểu cách file được đọc từng ký tự.
- Xem `error.h` để biết cơ chế báo cáo lỗi.

---

**Cập nhật lần cuối:** 24/11/2025
