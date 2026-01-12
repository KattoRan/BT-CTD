# Hướng Dẫn Viết File KPL (KPL Language Guide)

## 1. Cấu Trúc Chung của File KPL

Một file KPL hợp lệ cần có cấu trúc cơ bản sau:

```kpl
PROGRAM <TênChương>;
  <Khai báo hằng số>
  <Khai báo kiểu dữ liệu>
  <Khai báo biến>
  <Khai báo hàm và thủ tục>
BEGIN
  <Các lệnh>
END.
```

---

## 2. Chi Tiết Từng Phần

### 2.1 Khai Báo Chương Trình (PROGRAM)

- Bắt buộc phải có ở đầu file
- Format: `PROGRAM <Tên>;`
- Tên chương trình không được vượt quá 15 ký tự
- Phải kết thúc bằng dấu chấm phẩy (`;`)

**Ví dụ:**

```kpl
PROGRAM MyProgram;
```

### 2.2 Khai Báo Hằng Số (CONST)

- Khai báo các hằng số sẽ dùng trong chương trình
- Format:

```kpl
CONST
  <Tên> = <Giá trị>;
  <Tên> = <Giá trị>;
```

**Ví dụ:**

```kpl
CONST
  MAX = 10;
  NAME = 'ABC';
```

### 2.3 Khai Báo Kiểu Dữ Liệu (TYPE)

- Định nghĩa các kiểu dữ liệu mới
- Các kiểu cơ bản: `INTEGER`, `CHAR`
- Format:

```kpl
TYPE
  <Tên kiểu> = <Kiểu dữ liệu>;
  <Tên kiểu> = <Kiểu dữ liệu>;
```

**Ví dụ:**

```kpl
TYPE
  Number = INTEGER;
  Letter = CHAR;
  IntArray = ARRAY(. 10 .) OF INTEGER;
```

**Lưu ý:**

- Kích thước mảng được khai báo với cú pháp `ARRAY(. <size> .) OF <type>`
- Dấu chấm `(.)` xung quanh kích thước là bắt buộc

### 2.4 Khai Báo Biến (VAR)

- Khai báo các biến sẽ sử dụng
- Format:

```kpl
VAR
  <Tên1>, <Tên2> : <Kiểu>;
  <Tên3> : <Kiểu>;
```

**Ví dụ:**

```kpl
VAR
  n, sum : INTEGER;
  ch : CHAR;
  A : ARRAY(. 100 .) OF INTEGER;
```

### 2.5 Khai Báo Hàm (FUNCTION)

- Hàm phải trả về một giá trị
- Format:

```kpl
FUNCTION <Tên>(<Tham số>) : <Kiểu trả về>;
  <Khai báo cục bộ>
BEGIN
  <Các lệnh>
END;
```

**Ví dụ:**

```kpl
FUNCTION F(n : INTEGER) : INTEGER;
BEGIN
  IF n = 0 THEN F := 1 ELSE F := N * F(N - 1)
END;
```

**Lưu ý:**

- Để trả về giá trị, gán cho tên hàm (ví dụ: `F := 1`)
- Tham số được khai báo bên trong dấu ngoặc

### 2.6 Khai Báo Thủ Tục (PROCEDURE)

- Thủ tục không trả về giá trị
- Format:

```kpl
PROCEDURE <Tên>(<Tham số>);
  <Khai báo cục bộ>
BEGIN
  <Các lệnh>
END;
```

**Ví dụ:**

```kpl
PROCEDURE PRINT_RESULT(x : INTEGER);
BEGIN
  CALL WRITEI(x);
  CALL WRITELN
END;
```

---

## 3. Các Câu Lệnh (Statements)

### 3.1 Gán Giá Trị (Assignment)

- Format: `<Biến> := <Giá trị>;`
- Dùng `:=` để gán (không phải `=`)

**Ví dụ:**

```kpl
n := 5;
sum := sum + n;
ch := 'A';
```

### 3.2 Gọi Hàm/Thủ Tục (CALL)

- Format: `CALL <Tên>(<Tham số>);`
- Hoặc gộp trong biểu thức: `<Biến> := <Tên>(<Tham số>);`

**Ví dụ:**

```kpl
CALL WRITEI(100);
CALL WRITELN;
x := F(n);
```

### 3.3 Khối Lệnh (BEGIN ... END)

- Nhóm nhiều lệnh lại
- Các lệnh được ngăn cách bằng dấu chấm phẩy (`;`)

**Ví dụ:**

```kpl
BEGIN
  n := 5;
  sum := 0;
  ch := 'A'
END
```

### 3.4 Lệnh Điều Kiện (IF ... THEN ... ELSE)

- Format: `IF <Điều kiện> THEN <Lệnh> ELSE <Lệnh>`
- ELSE là tùy chọn

**Ví dụ:**

```kpl
IF n > 0 THEN
  sum := sum + n
ELSE
  sum := 0;
```

### 3.5 Vòng Lặp WHILE

- Format: `WHILE <Điều kiện> DO <Lệnh>`

**Ví dụ:**

```kpl
WHILE n != 0 DO
BEGIN
  digit := n - (n / 10) * 10;
  sum := sum + digit;
  n := n / 10
END;
```

### 3.6 Vòng Lặp FOR

- Format: `FOR <Biến> := <Giá trị khởi tạo> TO <Giá trị kết thúc> DO <Lệnh>`

**Ví dụ:**

```kpl
FOR i := 1 TO 10 DO
BEGIN
  CALL WRITEI(i);
  CALL WRITELN
END;
```

---

## 4. Biểu Thức (Expressions)

### 4.1 Các Toán Tử

- **Cộng:** `+`
- **Trừ:** `-`
- **Nhân:** `*`
- **Chia:** `/` (chia nguyên)

### 4.2 Các Phép So Sánh

- **Bằng:** `=`
- **Không bằng:** `!=`
- **Nhỏ hơn:** `<`
- **Nhỏ hơn hoặc bằng:** `<=`
- **Lớn hơn:** `>`
- **Lớn hơn hoặc bằng:** `>=`

**Ví dụ:**

```kpl
x := (n + 5) * 2;
IF n < 0 THEN n := 0 - n;
digit := n - (n / 10) * 10;
```

### 4.3 Truy Cập Mảng

- Format: `<Tên mảng>(. <Index> .)`
- Dấu chấm `(.)` xung quanh index là bắt buộc

**Ví dụ:**

```kpl
A(. i .) := 100;
sum := sum + A(. i .);
```

---

## 5. Các Hàm Built-in (Hàm Có Sẵn)

### 5.1 Đọc Dữ Liệu

- `READI` - Đọc một số nguyên (INTEGER)
- `READC` - Đọc một ký tự (CHAR)

**Ví dụ:**

```kpl
n := READI;
ch := READC;
```

### 5.2 Viết Dữ Liệu

- `WRITEI(n)` - Viết một số nguyên
- `WRITEC(ch)` - Viết một ký tự
- `WRITELN` - Viết xuống dòng mới

**Ví dụ:**

```kpl
CALL WRITEI(n);
CALL WRITEC(ch);
CALL WRITELN;
```

---

## 6. Quy Tắc và Lưu Ý Quan Trọng

### 6.1 Kết Thúc File

- Chương trình phải kết thúc bằng `END.` (END + dấu chấm)

### 6.2 Bình Luận (Comment)

- Sử dụng `(* ... *)` để viết bình luận
- Có thể nhiều dòng

**Ví dụ:**

```kpl
(* Đây là bình luận *)
n := 5; (* Gán giá trị 5 cho n *)
```

### 6.3 Phân Biệt Chữ Hoa/Thường

- Ngôn ngữ KPL không phân biệt chữ hoa và chữ thường
- `PROGRAM` = `program` = `Program`

### 6.4 Tên Biến/Hàm

- Tối đa 15 ký tự
- Bắt đầu bằng chữ cái
- Chỉ chứa chữ cái, chữ số, và dấu gạch dưới `_`

### 6.5 Dấu Chấm Phẩy

- Bắt buộc sau mỗi khai báo
- Bắt buộc sau mỗi câu lệnh
- **Lưu ý:** Không có dấu chấm phẩy trước `END` trong khối lệnh

---

## 7. Ví Dụ Hoàn Chỉnh

### Ví dụ 1: Chương trình đơn giản

```kpl
PROGRAM Example1;
VAR n : INTEGER;
BEGIN
  n := 5;
  CALL WRITEI(n);
  CALL WRITELN
END.
```

### Ví dụ 2: Tính giai thừa

```kpl
PROGRAM Factorial;
VAR n : INTEGER;

FUNCTION F(n : INTEGER) : INTEGER;
BEGIN
  IF n = 0 THEN F := 1 ELSE F := N * F(N - 1)
END;

BEGIN
  FOR n := 1 TO 7 DO
    BEGIN
      CALL WRITEI(F(n));
      CALL WRITELN
    END
END.
```

### Ví dụ 3: Sử dụng mảng

```kpl
PROGRAM ArraySum;
CONST MAX = 10;
TYPE IntArray = ARRAY(. MAX .) OF INTEGER;

VAR
  A : IntArray;
  N : INTEGER;
  i : INTEGER;
  sum : INTEGER;

PROCEDURE INPUT;
BEGIN
  N := READI;
  FOR i := 1 TO N DO
    A(. i .) := READI
END;

PROCEDURE OUTPUT;
BEGIN
  sum := 0;
  FOR i := 1 TO N DO
    BEGIN
      CALL WRITEI(A(. i .));
      CALL WRITELN;
      sum := sum + A(. i .)
    END;
  CALL WRITEI(sum)
END;

BEGIN
  CALL INPUT;
  CALL OUTPUT
END.
```

### Ví dụ 4: Tính tổng các chữ số

```kpl
PROGRAM SumDigit;
VAR
  n : INTEGER;
  sum : INTEGER;
  digit : INTEGER;

BEGIN
  n := READI;

  (* Nếu n âm thì đổi sang dương *)
  IF n < 0 THEN
    n := 0 - n;

  sum := 0;

  WHILE n != 0 DO
  BEGIN
    digit := n - (n / 10) * 10;
    sum := sum + digit;
    n := n / 10
  END;

  CALL WRITEI(sum);
  CALL WRITELN
END.
```

---

## 8. Các Lỗi Thường Gặp

| Lỗi                         | Nguyên Nhân                    | Cách Sửa                      |
| --------------------------- | ------------------------------ | ----------------------------- |
| `Missing token`             | Thiếu dấu chấm phẩy hoặc token | Kiểm tra cú pháp cẩn thận     |
| `Undeclared identifier`     | Sử dụng biến chưa khai báo     | Khai báo biến trong VAR       |
| `Illegal use of keyword`    | Dùng từ khóa làm tên biến      | Dùng tên khác                 |
| `Invalid type`              | Kiểu dữ liệu không hợp lệ      | Chỉ dùng INTEGER, CHAR, ARRAY |
| `Wrong parameter count`     | Số tham số sai                 | Kiểm tra lại định nghĩa hàm   |
| `Array index out of bounds` | Chỉ số mảng vượt phạm vi       | Kiểm tra giá trị index        |

---

## 9. Cách Biên Dịch

Để biên dịch file KPL, sử dụng lệnh:

```bash
kplc.exe <tên_file.kpl> <tên_output>
```

**Ví dụ:**

```bash
kplc.exe factorial.kpl output
```

Nếu biên dịch thành công, sẽ tạo ra file `output` là chương trình object (mã máy ảo).

---

## 10. Chạy Chương Trình

Để chạy chương trình đã biên dịch:

```bash
interpreter <tên_output>
```

**Ví dụ:**

```bash
interpreter output
```

---

## Tóm Tắt Quy Tắc Cú Pháp

1. Kết thúc file bằng `END.`
2. Dùng `:=` để gán (không dùng `=`)
3. Dùng `=` để so sánh (không dùng `==`)
4. Dấu chấm phẩy `;` sau mỗi khai báo/câu lệnh
5. Không có dấu chấm phẩy trước `END` trong khối lệnh
6. Mảng: `ARRAY(. size .) OF type` và truy cập: `A(. i .)`
7. Bình luận: `(* ... *)`
8. Hàm: trả về bằng cách gán cho tên hàm
9. Gọi thủ tục: `CALL <Tên>(<Tham số>);`
10. Chương trình bắt đầu bằng `PROGRAM <Tên>;`

Chúc bạn viết code KPL thành công!
