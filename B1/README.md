# Hướng Dẫn Chạy Chương Trình

## Mô Tả

Chương trình này thực hiện phân tích và lập chỉ mục từ khóa từ một file văn bản.

- Đọc file văn bản đầu vào
- Loại bỏ các từ dừng (stop words) được định nghĩa trong file `stopw.txt`
- Loại bỏ các danh từ riêng (bắt đầu bằng chữ hoa)
- Đếm số lần xuất hiện của mỗi từ
- Ghi ra file `out.txt` với định dạng: `từ: số_lần_xuất_hiện, dòng_1, dòng_2, ...`

## Yêu Cầu

- Trình biên dịch C (GCC hoặc MSVC)
- File `stopw.txt` chứa danh sách các từ dừng (một từ mỗi dòng)
- File văn bản đầu vào để phân tích

## Các File Cần Thiết

| File        | Mô Tả                                               |
| ----------- | --------------------------------------------------- |
| `main.c`    | Mã nguồn chương trình                               |
| `main.exe`  | File thực thi (đã được biên dịch)                   |
| `stopw.txt` | Danh sách các từ dừng (ví dụ: the, a, and, is, ...) |
| `*.txt`     | File văn bản đầu vào để phân tích                   |

## Cách Chạy Chương Trình

### Phương Pháp 1: Sử Dụng File Thực Thi Đã Biên Dịch

```bash
main.exe <tên_file_văn_bản>
```

**Ví dụ:**

```bash
main.exe alice30.txt
```

**Kết quả:** File `out.txt` sẽ được tạo chứa các từ được lập chỉ mục

### Phương Pháp 2: Biên Dịch từ Mã Nguồn (nếu cần)

#### Trên Windows (GCC):

```bash
gcc -o main main.c
main.exe <tên_file_văn_bản>
```

#### Trên Linux/Mac:

```bash
gcc -o main main.c
./main <tên_file_văn_bản>
```

## Ví Dụ Sử Dụng

### Bước 1: Chuẩn bị file đầu vào

Đảm bảo bạn có file `alice30.txt` hoặc `vanban.txt` trong cùng thư mục

### Bước 2: Chạy chương trình

```bash
main.exe alice30.txt
```

### Bước 3: Xem kết quả

Mở file `out.txt` để xem kết quả phân tích

## Định Dạng File `out.txt`

```
word: count, line1, line2, line3, ...
example: 5, 1, 3, 7, 10, 15
```

## Lưu Ý

- File `stopw.txt` phải tồn tại trong cùng thư mục với chương trình
- Chương trình sẽ chuyển tất cả các từ thành chữ thường khi xử lý
- Các danh từ riêng (bắt đầu bằng chữ hoa) sẽ bị loại bỏ
- Các ký tự không phải chữ cái sẽ được xem như dấu tách từ

## Xử Lý Lỗi

| Lỗi                           | Nguyên Nhân                | Giải Pháp                   |
| ----------------------------- | -------------------------- | --------------------------- |
| "Loi: Thieu ten file dau vao" | Chưa cung cấp tên file     | Chạy: `main.exe <tên_file>` |
| "Khong tim thay file"         | File đầu vào không tồn tại | Kiểm tra tên file và vị trí |
| "Khong the tao file out.txt"  | Không có quyền ghi         | Kiểm tra quyền thư mục      |

## Liên Hệ

Nếu gặp vấn đề, kiểm tra các file yêu cầu đầy đủ và các quyền truy cập thích hợp.
