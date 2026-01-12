# B4: Symbol Table (Bảng ký hiệu) — Tuần 4

## Mô tả chung

Thư mục `B4` chứa bài tập xây dựng **bảng ký hiệu (symbol table)** cho ngôn ngữ KPL.
Mục tiêu:

- Lưu trữ thông tin về định danh (tên biến, hàm): kiểu, phạm vi (scope), vị trí khai báo.
- Hỗ trợ các thao tác: thêm, tìm, xóa khi thoát scope, kiểm tra trùng khai báo.
- Tích hợp với parser/scanner để kiểm tra semantic cơ bản.

## Cấu trúc dự án

```
B4/
├── symtab.c, symtab.h       # hiện thực bảng ký hiệu
├── main.c                   # ví dụ/entry point dùng symtab
├── debug.c, debug.h         # helper debug (tuỳ chọn)
├── Makefile                 # build script
├── symtab.exe               # binary (nếu đã build)
└── test/ (nếu có)          # test cases
```

## Biên dịch

Sử dụng Makefile:

```powershell
cd .\B4
make
```

Nếu không có `make`, biên dịch thủ công:

```powershell
gcc -c symtab.c -o symtab.o
gcc -c debug.c -o debug.o
gcc -c main.c -o main.o
gcc symtab.o debug.o main.o -o symtab.exe
```

## Chạy

- Chạy chương trình demo:

```powershell
.\symtab.exe
```

## Công việc thường làm

- Kiểm tra hàm thêm symbol: ghi tên, kiểu, vị trí.
- Triển khai quản lý scope: push_scope / pop_scope.
- Xử lý tái khai báo và báo lỗi hợp lệ.
- Tích hợp tìm symbol trong lồng scope.

## Gợi ý debug

- Dùng `debug.c` để in bảng ký hiệu hiện tại khi chạy.
- Viết test nhỏ: khai báo biến trong inner scope và kiểm tra visibility.
- Dùng `make clean` trước khi rebuild nếu cần.

---

**Cập nhật lần cuối:** 08/12/2025
