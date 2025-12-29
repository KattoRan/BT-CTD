# KPL Interpreter

Thư mục này chứa các chương trình ví dụ viết bằng ngôn ngữ KPL (Kule Programming Language) và các bản biên dịch của chúng.

## Cấu trúc thư mục

```
compilerKPL_interpreter/
├── program1.kpl        # Chương trình KPL ví dụ 1
├── program1            # Bản biên dịch của program1
├── program2.kpl        # Chương trình KPL ví dụ 2
└── program2            # Bản biên dịch của program2
```

## Mô tả các chương trình

### program1.kpl

Chương trình đơn giản thực hiện các phép toán cơ bản:

- Đọc 2 số nguyên từ người dùng
- Tính toán tổng, hiệu và tích của hai số
- In kết quả các phép toán lên màn hình

**Input**: 2 số nguyên (cách nhau bởi xuống dòng)  
**Output**: Tổng, hiệu, tích (mỗi kết quả trên một dòng)

### program2.kpl

Chương trình tính tổng các chữ số của một số nguyên:

- Đọc 1 số nguyên từ người dùng
- Nếu số âm, chuyển đổi sang số dương
- Lặp lại: tách chữ số cuối cùng, cộng vào tổng, loại bỏ chữ số đó
- In tổng các chữ số lên màn hình

**Input**: 1 số nguyên (có thể âm)  
**Output**: Tổng các chữ số

## Cách chạy các chương trình

Các tệp `program1` và `program2` là bản biên dịch (executable) sẵn sàng:

```powershell
# Chạy program1
.\program1

# Chạy program2
.\program2
```

Nhập dữ liệu theo yêu cầu khi chương trình chạy.

## Biên dịch lại từ mã nguồn

Nếu bạn muốn biên dịch từ file `.kpl`, sử dụng trình biên dịch KPL (thường là `kpl.exe` hoặc `compiler.exe`):

```powershell
# Giả sử trình biên dịch KPL nằm trong PATH hoặc cùng thư mục
./compiler program1.kpl
./compiler program2.kpl
```

## Ví dụ sử dụng

### Program1 - Phép toán cơ bản

```
$ .\program1
5
3
8        # 5 + 3
2        # 5 - 3
15       # 5 * 3
```

### Program2 - Tính tổng chữ số

```
$ .\program2
12345
15       # 1 + 2 + 3 + 4 + 5

$ .\program2
-999
27       # 9 + 9 + 9
```

## Ghi chú

- Các chương trình này là phần của dự án biên dịch KPL
- Chúng được sử dụng để kiểm tra và minh họa khả năng của trình thông dịch
- Cú pháp KPL tương tự Pascal với một số điều chỉnh
