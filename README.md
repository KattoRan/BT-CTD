# BTL / Bài tập theo tuần

Mỗi thư mục ở cấp gốc trong workspace này là một bài tập (hoặc tập các bài tập) theo tuần.

Ví dụ:

- `B1` — thư mục chứa bài tập của tuần 1
- `B2` — thư mục chứa bài tập của tuần 2
- ... và tương tự

Mục đích của README này:

- Giải thích quy ước đặt tên các thư mục
- Cho biết cách chạy/biên dịch nhanh các file mẫu (nếu có)
- Gợi ý cách nộp/bổ sung bài tập

## Quy ước thư mục và tệp

- Tên thư mục bắt đầu bằng chữ `B` và số tuần: `B1`, `B2`, ...
- Mỗi thư mục có thể chứa mã nguồn, dữ liệu đầu vào và file kết quả.
- Nếu có file `main.c` trong thư mục, đó thường là file ví dụ hoặc bài tập C chính.
- Mỗi thư mục `B*` có thể có `README.md` riêng để mô tả chi tiết bài tập.

## Ví dụ nhanh

Trong workspace hiện tại có:

```
B1/
    alice30.txt
    main.c
    out.txt
    README.md
    stopw.txt
    vanban.txt

B2/
    charcode.c, charcode.h
    scanner.c, scanner.exe
    reader.c, reader.h
    token.c, token.h
    error.c, error.h
    Makefile
    run_tests.ps1
    test/
        (input file .kpl và expected output)
    README.md
```

- `B1` chứa bài tập tuần 1 (xử lý file text)
- `B2` chứa bài tập tuần 2 (lexical scanner/tokenizer) — xem `B2/README.md` để hiểu chi tiết

## Cách biên dịch và chạy (Windows PowerShell)

### B1 (đơn giản: main.c)

Nếu bạn có GCC (MinGW) hoặc MSYS2 đã cài:

```powershell
gcc .\B1\main.c -o .\B1\main.exe
.\B1\main.exe
```

### B2 (dự án lớn: scanner với Makefile)

```powershell
cd .\B2
make
# Chạy script test (so sánh output với expected)
.\run_tests.ps1
# Hoặc chạy scanner trực tiếp
.\scanner.exe
```

Nếu bạn dùng Microsoft Visual C (cl.exe), hãy mở Developer Command Prompt hoặc thiết lập môi trường MSVC trước.

## Gợi ý tổ chức nội dung bài tập

- Đặt input mẫu trong `*_in.txt` hoặc `input.txt` để dễ kiểm thử.
- Ghi kết quả mẫu vào `out.txt` hoặc `expected_*.txt` để so sánh.
- Nếu cần giải thích thuật toán hoặc cách hoạt động, thêm `README.md` riêng trong mỗi `B*`.

## Cách nộp / cập nhật

- Thêm file hoặc sửa nội dung trong thư mục tương ứng (`B1`, `B2`, ...).
- Commit các thay đổi với thông báo rõ ràng (ví dụ: "B2: hoàn thành scanner tuần 2").

---

Để hiểu chi tiết về từng bài tập, vào thư mục tương ứng và đọc file `README.md` (nếu có).
