# KPL Compiler - Tài Liệu Giải Thích

## Giới Thiệu Dự Án

**KPL Compiler** là một compiler hoàn chỉnh cho ngôn ngữ lập trình KPL (một ngôn ngữ lập trình đơn giản). Dự án được phát triển bởi Hedspi - Hanoi University of Technology.

---

## Kiến Trúc Tổng Quan

KPL Compiler được xây dựng theo các giai đoạn chính của một compiler:

1. **Scanning/Lexical Analysis** (Phân tích từ vựng)
2. **Parsing/Syntax Analysis** (Phân tích cú pháp)
3. **Semantic Analysis** (Phân tích ngữ nghĩa)
4. **Code Generation** (Sinh mã)

---

## Chi Tiết Các File

### 1. **main.c / main.h**

- **Mục đích**: Điểm nhập chính của chương trình compiler
- **Chức năng**:
  - Xử lý các tham số dòng lệnh (input file, output file, flags như `-dump`)
  - Điều phối quá trình biên dịch bằng cách gọi các module khác
  - Quản lý luồng chính của compiler
- **Cách sử dụng**: `kplc input.kpl output [-dump]`

---

### 2. **reader.c / reader.h**

- **Mục đích**: Quản lý đọc file nguồn
- **Chức năng**:
  - Mở và đóc file mã nguồn KPL
  - Cung cấp các hàm để lấy ký tự tiếp theo từ file
  - Quản lý vị trí dòng và cột để báo lỗi chính xác
  - Hỗ trợ tái định vị trong file
- **Kết nối**: Cung cấp dữ liệu cho Scanner

---

### 3. **token.c / token.h**

- **Mục đích**: Định nghĩa kiến trúc Token
- **Chức năng**:
  - Lưu trữ thông tin về các token (đơn vị nhỏ nhất của ngôn ngữ)
  - Định nghĩa các loại token (keyword, identifier, operator, literal, v.v.)
  - Cấu trúc dữ liệu lưu giá trị token, loại token, vị trí trong file
- **Các loại Token**:
  - Keywords: PROGRAM, BEGIN, END, CALL, FOR, v.v.
  - Identifiers: tên biến, hàm
  - Numbers: hằng số số
  - Operators: +, -, \*, /, =, v.v.
  - Delimiters: (, ), ;, ., v.v.

---

### 4. **scanner.c / scanner.h**

- **Mục đích**: Phân tích từ vựng (Lexical Analyzer)
- **Chức năng**:
  - Đọc ký tự từ file và tạo thành các token
  - Nhận diện keywords, identifiers, constants, operators
  - Bỏ qua whitespace và comment
  - Cung cấp hai hàm chính:
    - `getToken()`: Lấy token tiếp theo (kể cả comment)
    - `getValidToken()`: Lấy token tiếp theo (bỏ comment)
  - `printToken()`: In thông tin token (dùng cho debug)

---

### 5. **charcode.c / charcode.h**

- **Mục đích**: Quản lý mã ký tự
- **Chức năng**:
  - Lưu trữ bảng mã ASCII cho các ký tự đặc biệt
  - Cung cấp các hàm để kiểm tra loại ký tự (số, chữ, operator, v.v.)
  - Hỗ trợ đó ký tự trong scanner
- **Ứng dụng**: Giúp scanner xác định loại ký tự để tạo token phù hợp

---

### 6. **error.c / error.h**

- **Mục đích**: Xử lý lỗi trong quá trình biên dịch
- **Chức năng**:
  - Ghi lại các lỗi phát hiện được (syntax error, semantic error, v.v.)
  - In thông báo lỗi với vị trí (dòng, cột)
  - Cung cấp mã lỗi để báo cáo kết quả biên dịch
  - Có thể dừng hoặc tiếp tục biên dịch tùy theo loại lỗi
- **Kiểu lỗi**: Syntax Error, Semantic Error, Type Mismatch Error, v.v.

---

### 7. **parser.c / parser.h**

- **Mục đích**: Phân tích cú pháp (Syntax Analyzer)
- **Chức năng**:
  - Kiểm tra cú pháp của mã nguồn
  - Xây dựng cây phân tích (parse tree) theo ngữ pháp KPL
  - Các hàm chính:
    - `compileProgram()`: Phân tích chương trình chính
    - `compileBlock()`, `compileBlock2()`, ...: Phân tích khối mã
    - `compileConstDecls()`: Phân tích khai báo hằng
    - `compileTypeDecls()`: Phân tích khai báo kiểu
    - `compileVarDecls()`: Phân tích khai báo biến
    - `compileSubDecls()`: Phân tích khai báo hàm/procedure
    - `compileFuncDecl()`, `compileProcDecl()`: Phân tích hàm và procedure
    - `compileStatement()`, `compileExpression()`: Phân tích lệnh và biểu thức
  - `eat()`: Hàm tiện ích để kiểm tra và bỏ qua token
  - `scan()`: Lấy token tiếp theo
- **Liên kết**: Sử dụng Scanner, Symbol Table, Semantics, Code Generation

---

### 8. **semantics.c / semantics.h**

- **Mục đích**: Phân tích ngữ nghĩa (Semantic Analyzer)
- **Chức năng**:
  - Kiểm tra ngữ nghĩa của mã (xem có hợp lý không)
  - Kiểm tra kiểu dữ liệu (type checking)
  - Kiểm tra phạm vi biến (scope checking)
  - Kiểm tra khai báo được xác định trước khi sử dụng
  - Xác nhận các hoạt động hợp lệ (ví dụ: không thể cộng chuỗi với số)
  - Tương tác với Symbol Table để quản lý symbols
- **Ví dụ kiểm tra**:
  - Biến được khai báo trước khi sử dụng
  - Phép toán hợp lệ với các kiểu
  - Số lượng tham số hợp lệ

---

### 9. **symtab.c / symtab.h**

- **Mục đích**: Quản lý bảng ký hiệu (Symbol Table)
- **Chức năng**:
  - Lưu trữ thông tin về tất cả các symbols (biến, hàm, hằng, kiểu)
  - Cung cấp các hàm:
    - `lookup()`: Tìm kiếm symbol
    - `insert()`: Thêm symbol mới
    - `removeSymbol()`: Xóa symbol
    - `enterScope()`: Vào phạm vi mới (cho hàm/procedure)
    - `exitScope()`: Thoát khỏi phạm vi
  - Hỗ trợ scoping (phạm vi - biến cục bộ, toàn cục)
  - Lưu loại, kích thước, vị trí bộ nhớ của mỗi symbol
- **Tầm quan trọng**: Không thể xác minh ngữ nghĩa không có bảng ký hiệu

---

### 10. **codegen.c / codegen.h**

- **Mục đích**: Sinh mã (Code Generator)
- **Chức năng**:
  - Tạo mã máy hoặc mã trung gian từ cây phân tích
  - Phát hành các hướng dẫn cho máy ảo KPL
  - Quản lý bộ nhớ (stack, heap)
  - Các hàm chính:
    - `genCode()`: Hàm sinh mã tổng quát
    - `genPushConst()`: Sinh lệnh đẩy hằng lên stack
    - `genAssign()`: Sinh lệnh gán
    - `genBinOp()`: Sinh lệnh toán tử nhị phân
    - `genFunctionCall()`: Sinh lệnh gọi hàm
    - Các hàm khác cho mỗi loại lệnh/biểu thức
- **Đầu ra**: Mã thực thi cho máy ảo KPL

---

### 11. **instructions.c / instructions.h**

- **Mục đích**: Định nghĩa các lệnh máy ảo KPL
- **Chức năng**:
  - Liệt kê tất cả các instruction/lệnh có sẵn cho máy ảo
  - Định nghĩa cấu trúc lệnh (opcode, operands)
  - Cung cấp các hàm để tạo instruction
  - Ví dụ instruction:
    - PUSH_CONST: Đẩy hằng lên stack
    - ASSIGN: Gán giá trị
    - ADD, SUB, MUL, DIV: Phép toán
    - CALL: Gọi hàm
    - RETURN: Trở về
    - READ, WRITE: I/O
- **Sử dụng bởi**: Code Generator

---

### 12. **debug.c / debug.h**

- **Mục đích**: Hỗ trợ gỡ lỗi (Debugging)
- **Chức năng**:
  - Cung cấp các hàm in thông tin debug
  - In thông tin về tokens, symbols, trạng thái phân tích
  - In cây phân tích
  - In bảng ký hiệu (symbol table)
  - In mã được sinh ra
- **Dùng với flag**: `-dump` khi chạy compiler

---

## Các File Ví Dụ

### **example1.kpl, example2.kpl, example3.kpl, ..., example7.kpl**

- **Mục đích**: Các chương trình KPL mẫu để test compiler
- **Nội dung**: Ví dụ về cách viết chương trình trong ngôn ngữ KPL
- **Ứng dụng**: Dùng để kiểm tra xem compiler có hoạt động đúng không

**Ví dụ example1.kpl:**

```kpl
Program Example1; (* Example 1 *)
Begin
    FOR (1,100)
        CALL WRITEC(' ');
End. (* Example 1 *)
```

---

## Các File Kết Quả

### **result2, result2.txt, example2**

- **Mục đích**: Kết quả output của quá trình biên dịch
- **Nội dung**: Mã máy hoặc mã trung gian được sinh ra từ các file .kpl
- **Dùng để**: Kiểm tra kết quả compilation

---

## Build System

### **Makefile, Makefile.win**

- **Mục đích**: Tự động hóa quá trình biên dịch C code
- **Chức năng**:
  - Định nghĩa cách biên dịch các file .c thành file .o
  - Liên kết các file .o thành executable `kplc`
  - Định nghĩa các target: `all`, `clean`
  - `Makefile`: Cho Linux/Unix/Mac
  - `Makefile.win`: Cho Windows
- **Sử dụng**:
  ```bash
  make          # Linux/Unix
  make -f Makefile.win  # Windows
  ```

---

## Luồng Xử Lý Của Compiler

```
File Input (*.kpl)
    ↓
[Reader] - Đọc file ký tự từng cái
    ↓
[Scanner] - Tạo tokens từ ký tự
    ↓
[Parser] - Kiểm tra cú pháp, tạo cây phân tích
    ↓
[Semantics] - Kiểm tra ngữ nghĩa, kiểm tra kiểu
    ↓
[Symbol Table] - Lưu giữ thông tin về các symbols
    ↓
[Code Generator] - Sinh mã máy
    ↓
[Instructions] - Tạo các lệnh thực thi
    ↓
File Output (executable)
```

---

## Cách Sử Dụng Compiler

```bash
# Biên dịch file KPL
./kplc input.kpl output_executable

# Biên dịch với chế độ debug (in mã sinh ra)
./kplc input.kpl output_executable -dump
```

---

## Tóm Tắt Chức Năng Các Module

| File           | Loại        | Chức Năng                 |
| -------------- | ----------- | ------------------------- |
| main.c         | Điểm nhập   | Điểm vào của chương trình |
| reader.c       | I/O         | Đọc file nguồn            |
| scanner.c      | Lexical     | Phân tích từ vựng         |
| parser.c       | Syntax      | Phân tích cú pháp         |
| semantics.c    | Semantic    | Phân tích ngữ nghĩa       |
| symtab.c       | Symbol      | Quản lý ký hiệu           |
| codegen.c      | Code Gen    | Sinh mã máy               |
| instructions.c | VM          | Định nghĩa lệnh máy ảo    |
| error.c        | Error       | Xử lý lỗi                 |
| debug.c        | Debug       | Hỗ trợ gỡ lỗi             |
| charcode.c     | Utility     | Xử lý mã ký tự            |
| token.c        | Data Struct | Cấu trúc token            |

---
