/* 
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#include <stdio.h>
#include <stdlib.h>
#include "error.h"

#define NUM_OF_ERRORS 29

struct ErrorMessage {
  ErrorCode errorCode;
  char *message;
};

struct ErrorMessage errors[29] = {
  {ERR_END_OF_COMMENT, "End of comment expected."}, // kết thúc comment bị thiếu
  {ERR_IDENT_TOO_LONG, "Identifier too long."}, // tên định danh quá dài
  {ERR_INVALID_CONSTANT_CHAR, "Invalid char constant."}, // hằng ký tự không hợp lệ
  {ERR_INVALID_SYMBOL, "Invalid symbol."}, // ký hiệu không hợp lệ
  {ERR_INVALID_IDENT, "An identifier expected."}, // định danh không hợp lệ
  {ERR_INVALID_CONSTANT, "A constant expected."}, // hằng không hợp lệ
  {ERR_INVALID_TYPE, "A type expected."}, // kiểu không hợp lệ
  {ERR_INVALID_BASICTYPE, "A basic type expected."}, // kiểu cơ bản không hợp lệ
  {ERR_INVALID_VARIABLE, "A variable expected."}, // biến không hợp lệ
  {ERR_INVALID_FUNCTION, "A function identifier expected."}, // hàm không hợp lệ
  {ERR_INVALID_PROCEDURE, "A procedure identifier expected."}, // thủ tục không hợp lệ
  {ERR_INVALID_PARAMETER, "A parameter expected."}, // tham số không hợp lệ
  {ERR_INVALID_STATEMENT, "Invalid statement."}, // câu lệnh không hợp lệ
  {ERR_INVALID_COMPARATOR, "A comparator expected."}, // toán tử so sánh không hợp lệ
  {ERR_INVALID_EXPRESSION, "Invalid expression."}, // biểu thức không hợp lệ
  {ERR_INVALID_TERM, "Invalid term."}, // hạng tử không hợp lệ
  {ERR_INVALID_FACTOR, "Invalid factor."}, // hệ số không hợp lệ
  {ERR_INVALID_LVALUE, "Invalid lvalue in assignment."}, // vế trái của phép gán không hợp lệ
  {ERR_INVALID_ARGUMENTS, "Wrong arguments."}, // đối số không hợp lệ
  {ERR_UNDECLARED_IDENT, "Undeclared identifier."}, // định danh chưa được khai báo
  {ERR_UNDECLARED_CONSTANT, "Undeclared constant."}, // hằng chưa được khai báo
  {ERR_UNDECLARED_INT_CONSTANT, "Undeclared integer constant."}, // hằng số nguyên chưa được khai báo
  {ERR_UNDECLARED_TYPE, "Undeclared type."}, // kiểu chưa được khai báo
  {ERR_UNDECLARED_VARIABLE, "Undeclared variable."}, // biến chưa được khai báo
  {ERR_UNDECLARED_FUNCTION, "Undeclared function."}, // hàm chưa được khai báo
  {ERR_UNDECLARED_PROCEDURE, "Undeclared procedure."}, // thủ tục chưa được khai báo
  {ERR_DUPLICATE_IDENT, "Duplicate identifier."}, // định danh bị trùng
  {ERR_TYPE_INCONSISTENCY, "Type inconsistency"}, // không nhất quán kiểu dữ liệu
  {ERR_PARAMETERS_ARGUMENTS_INCONSISTENCY, "The number of arguments and the number of parameters are inconsistent."} // số lượng tham số và đối số không nhất quán
};
// in ra lỗi tương ứng với mã lỗi
void error(ErrorCode err, int lineNo, int colNo) {
  int i;
  for (i = 0 ; i < NUM_OF_ERRORS; i ++) 
    if (errors[i].errorCode == err) {
      printf("%d-%d:%s\n", lineNo, colNo, errors[i].message);
      exit(0);
    }
}
// in ra lỗi thiếu token
void missingToken(TokenType tokenType, int lineNo, int colNo) {
  printf("%d-%d:Missing %s\n", lineNo, colNo, tokenToString(tokenType));
  exit(0);
}
// in ra lỗi khẳng định
void assert(char *msg) {
  printf("%s\n", msg);
}
