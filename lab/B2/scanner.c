/* Scanner
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Than Cat Ngoc Lan
 * @version 1.0
 */

#include <stdio.h>
#include <stdlib.h>

#include "reader.h"
#include "charcode.h"
#include "token.h"
#include "error.h"


extern int lineNo;
extern int colNo;
extern int currentChar;

extern CharCode charCodes[];

/***************************************************************/

void skipBlank() {
  while (charCodes[currentChar] == CHAR_SPACE)
    readChar();
}

void skipComment() {
  while (1) {
    readChar();
    if (currentChar == EOF) {
      error(ERR_ENDOFCOMMENT, lineNo, colNo);
    }
    // Nếu gặp dấu *, kiểm tra ký tự tiếp theo
    if (charCodes[currentChar] == CHAR_TIMES) {
      readChar();
      if (currentChar == EOF) {
        error(ERR_ENDOFCOMMENT, lineNo, colNo);
      }
      // Nếu ký tự tiếp theo là ) thì kết thúc comment
      if (charCodes[currentChar] == CHAR_RPAR) {
        readChar();
        return;
      }
    }
  }
}

void skipLineComment() {
  while (currentChar != EOF && currentChar != '\n') {
    readChar();
  }
}

Token* readIdentKeyword(void) {
  Token *token = makeToken(TK_NONE, lineNo, colNo);
  int count = 1;

  token->string[0] = (char)currentChar;
  readChar();

  while ((currentChar != EOF) && 
         ((charCodes[currentChar] == CHAR_LETTER) || (charCodes[currentChar] == CHAR_DIGIT))) {
    if (count <= MAX_IDENT_LEN) {
      token->string[count] = (char)currentChar;
    }
    count++;
    readChar();
  }

  if (count > MAX_IDENT_LEN) {
    error(ERR_IDENTTOOLONG, token->lineNo, token->colNo);
    return token;
  }

  token->string[count] = '\0';
  token->tokenType = checkKeyword(token->string);

  if (token->tokenType == TK_NONE) {
    token->tokenType = TK_IDENT;
  }

  return token;
}

Token* readNumber(void) {
  Token *token = makeToken(TK_NUMBER, lineNo, colNo);
  int count = 0;

  while ((currentChar != EOF) && (charCodes[currentChar] == CHAR_DIGIT)) {
    token->string[count] = (char)currentChar;
    count++;
    readChar();
  }

  token->string[count] = '\0';
  token->value = atoi(token->string); // Chuyển chuỗi thành số
  return token;
}

Token* readConstChar(void) {
  Token *token = makeToken(TK_CHAR, lineNo, colNo);

  readChar();
  if (currentChar == EOF) {
    token->tokenType = TK_NONE;
    error(ERR_INVALIDCHARCONSTANT, token->lineNo, token->colNo);
    return token;
  }
    
  token->string[0] = currentChar;
  token->string[1] = '\0';

  readChar();
  if (currentChar == EOF) {
    token->tokenType = TK_NONE;
    error(ERR_INVALIDCHARCONSTANT, token->lineNo, token->colNo);
    return token;
  }

  if (charCodes[currentChar] == CHAR_SINGLEQUOTE) {
    readChar();
    return token;
  } else {
    token->tokenType = TK_NONE;
    error(ERR_INVALIDCHARCONSTANT, token->lineNo, token->colNo);
    return token;
  }
}

Token* readString(void) {
  Token *token = makeToken(TK_STRING, lineNo, colNo);
  int count = 0;

  readChar(); // Bỏ qua dấu " mở đầu
  while (currentChar != EOF && charCodes[currentChar] != CHAR_DOUBLEQUOTE) {
    if (count <= MAX_IDENT_LEN) {
       token->string[count] = (char)currentChar;
       count++;
    }
    readChar();
  }
  
  token->string[count] = '\0';

  if (currentChar == EOF) {
    token->tokenType = TK_NONE;
    error(ERR_INVALIDCHARCONSTANT, lineNo, colNo);
    return token;
  }
  
  readChar(); // Bỏ qua dấu " đóng
  return token;
}

Token* getToken(void) {
  Token *token;
  int ln, cn;

  if (currentChar == EOF) 
    return makeToken(TK_EOF, lineNo, colNo);

  switch (charCodes[currentChar]) {
  case CHAR_SPACE: skipBlank(); return getToken();
  case CHAR_LETTER: return readIdentKeyword();
  case CHAR_DIGIT: return readNumber();
  // Dấu cộng +
  case CHAR_PLUS: 
    token = makeToken(SB_PLUS, lineNo, colNo);
    readChar(); 
    return token;

  // Dấu trừ -
  case CHAR_MINUS:
    token = makeToken(SB_MINUS, lineNo, colNo);
    readChar(); 
    return token;

  // Dấu nhân *
  case CHAR_TIMES:
    ln = lineNo; cn = colNo;
    readChar();
    if (charCodes[currentChar] == CHAR_TIMES) { // Nếu gặp thêm dấu * nữa
      readChar();
      return makeToken(SB_POWER, ln, cn); // Trả về token lũy thừa
    } else {
      return makeToken(SB_TIMES, ln, cn); // Trả về phép nhân bình thường
    }

  // Dấu chia /
  case CHAR_SLASH:
    ln = lineNo; cn = colNo;
    readChar();
    if (charCodes[currentChar] == CHAR_SLASH) { // Nếu gặp thêm dấu / nữa
      skipLineComment(); // Bỏ qua đến hết dòng
      return getToken(); // Gọi đệ quy lấy token tiếp theo
    } else {
      return makeToken(SB_SLASH, ln, cn); // Phép chia bình thường
    }
  
  // phép lấy dư
    case CHAR_PERCENT:
    token = makeToken(SB_MOD, lineNo, colNo);
    readChar();
    return token;

  // string
    case CHAR_DOUBLEQUOTE:
    return readString();
  
  // Dấu nhỏ hơn < và nhỏ hơn hoặc bằng <=
  case CHAR_LT:
    ln = lineNo; cn = colNo;
    readChar();
    if ((currentChar != EOF) && (charCodes[currentChar] == CHAR_EQ)) {
      readChar();
      return makeToken(SB_LE, ln, cn);
    } else {
      return makeToken(SB_LT, ln, cn);
    }

  // Dấu lớn hơn > và lớn hơn hoặc bằng >=
  case CHAR_GT:
    ln = lineNo; cn = colNo;
    readChar();
    if ((currentChar != EOF) && (charCodes[currentChar] == CHAR_EQ)) {
      readChar();
      return makeToken(SB_GE, ln, cn);
    } else {
      return makeToken(SB_GT, ln, cn);
    }

  // Dấu bằng = 
  case CHAR_EQ: 
    token = makeToken(SB_EQ, lineNo, colNo);
    readChar(); 
    return token;

  // Dấu chấm than ! (dùng cho !=)
  case CHAR_EXCLAIMATION:
    ln = lineNo; cn = colNo;
    readChar();
    if ((currentChar != EOF) && (charCodes[currentChar] == CHAR_EQ)) {
      readChar();
      return makeToken(SB_NEQ, ln, cn);
    } else {
      error(ERR_INVALIDSYMBOL, ln, cn);
      return makeToken(TK_NONE, ln, cn);
    }

  // Dấu phẩy ,
  case CHAR_COMMA:
    token = makeToken(SB_COMMA, lineNo, colNo);
    readChar(); 
    return token;

  // Dấu chấm . và ký hiệu kết thúc mảng .)
  case CHAR_PERIOD:
    ln = lineNo; cn = colNo;
    readChar();
    if ((currentChar != EOF) && (charCodes[currentChar] == CHAR_RPAR)) {
      readChar();
      return makeToken(SB_RSEL, ln, cn);
    } else {
      return makeToken(SB_PERIOD, ln, cn);
    }

  // Dấu hai chấm : và phép gán :=
  case CHAR_COLON:
    ln = lineNo; cn = colNo;
    readChar();
    if ((currentChar != EOF) && (charCodes[currentChar] == CHAR_EQ)) {
      readChar();
      return makeToken(SB_ASSIGN, ln, cn);
    } else {
      return makeToken(SB_COLON, ln, cn);
    }

  // Dấu chấm phẩy ;
  case CHAR_SEMICOLON:
    token = makeToken(SB_SEMICOLON, lineNo, colNo);
    readChar(); 
    return token;

  // Dấu nháy đơn ' (đã được xử lý trong readConstChar nhưng gọi ở đây để chắc chắn)
  case CHAR_SINGLEQUOTE: 
    return readConstChar();

  // Dấu mở ngoặc ( và ký hiệu bắt đầu mảng (. và comment (*
  case CHAR_LPAR:
    ln = lineNo; cn = colNo;
    readChar();
    
    if (currentChar == EOF) 
      return makeToken(SB_LPAR, ln, cn);

    switch (charCodes[currentChar]) {
      case CHAR_PERIOD:
        readChar();
        return makeToken(SB_LSEL, ln, cn);
      case CHAR_TIMES: // Đây là comment (*
        readChar();
        skipComment();
        return getToken(); // Gọi đệ quy để lấy token tiếp theo sau comment
      default:
        return makeToken(SB_LPAR, ln, cn);
    }

  // Dấu đóng ngoặc )
  case CHAR_RPAR:
    token = makeToken(SB_RPAR, lineNo, colNo);
    readChar(); 
    return token;
  default:
    token = makeToken(TK_NONE, lineNo, colNo);
    error(ERR_INVALIDSYMBOL, lineNo, colNo);
    readChar(); 
    return token;
  }
}


/******************************************************************/

void printToken(Token *token) {

  printf("%d-%d:", token->lineNo, token->colNo);

  switch (token->tokenType) {
  case TK_NONE: printf("TK_NONE\n"); break;
  case TK_IDENT: printf("TK_IDENT(%s)\n", token->string); break;
  case TK_NUMBER: printf("TK_NUMBER(%d)\n", token->value); break;
  case TK_CHAR: printf("TK_CHAR(\'%s\')\n", token->string); break;
  case TK_EOF: printf("TK_EOF\n"); break;
  case TK_STRING: printf("TK_STRING(\"%s\")\n", token->string); break;

  case KW_PROGRAM: printf("KW_PROGRAM\n"); break;
  case KW_CONST: printf("KW_CONST\n"); break;
  case KW_TYPE: printf("KW_TYPE\n"); break;
  case KW_VAR: printf("KW_VAR\n"); break;
  case KW_INTEGER: printf("KW_INTEGER\n"); break;
  case KW_CHAR: printf("KW_CHAR\n"); break;
  case KW_ARRAY: printf("KW_ARRAY\n"); break;
  case KW_OF: printf("KW_OF\n"); break;
  case KW_FUNCTION: printf("KW_FUNCTION\n"); break;
  case KW_PROCEDURE: printf("KW_PROCEDURE\n"); break;
  case KW_BEGIN: printf("KW_BEGIN\n"); break;
  case KW_END: printf("KW_END\n"); break;
  case KW_CALL: printf("KW_CALL\n"); break;
  case KW_IF: printf("KW_IF\n"); break;
  case KW_THEN: printf("KW_THEN\n"); break;
  case KW_ELSE: printf("KW_ELSE\n"); break;
  case KW_WHILE: printf("KW_WHILE\n"); break;
  case KW_DO: printf("KW_DO\n"); break;
  case KW_FOR: printf("KW_FOR\n"); break;
  case KW_TO: printf("KW_TO\n"); break;
  case KW_STRING: printf("KW_STRING\n"); break;
  case KW_BYTES: printf("KW_BYTES\n"); break;
  case KW_REPEAT: printf("KW_REPEAT\n"); break;
  case KW_UNTIL: printf("KW_UNTIL\n"); break;

  case SB_SEMICOLON: printf("SB_SEMICOLON\n"); break;
  case SB_COLON: printf("SB_COLON\n"); break;
  case SB_PERIOD: printf("SB_PERIOD\n"); break;
  case SB_COMMA: printf("SB_COMMA\n"); break;
  case SB_ASSIGN: printf("SB_ASSIGN\n"); break;
  case SB_EQ: printf("SB_EQ\n"); break;
  case SB_NEQ: printf("SB_NEQ\n"); break;
  case SB_LT: printf("SB_LT\n"); break;
  case SB_LE: printf("SB_LE\n"); break;
  case SB_GT: printf("SB_GT\n"); break;
  case SB_GE: printf("SB_GE\n"); break;
  case SB_PLUS: printf("SB_PLUS\n"); break;
  case SB_MINUS: printf("SB_MINUS\n"); break;
  case SB_TIMES: printf("SB_TIMES\n"); break;
  case SB_SLASH: printf("SB_SLASH\n"); break;
  case SB_LPAR: printf("SB_LPAR\n"); break;
  case SB_RPAR: printf("SB_RPAR\n"); break;
  case SB_LSEL: printf("SB_LSEL\n"); break;
  case SB_RSEL: printf("SB_RSEL\n"); break;
  case SB_MOD: printf("SB_MOD\n"); break;
  case SB_POWER: printf("SB_POWER\n"); break;
  }
}

int scan(char *fileName) {
  Token *token;

  if (openInputStream(fileName) == IO_ERROR)
    return IO_ERROR;

  token = getToken();
  while (token->tokenType != TK_EOF) {
    printToken(token);
    free(token);
    token = getToken();
  }

  free(token);
  closeInputStream();
  return IO_SUCCESS;
}

/******************************************************************/

int main(int argc, char *argv[]) {
  if (argc <= 1) {
    printf("scanner: no input file.\n");
    return -1;
  }

  if (scan(argv[1]) == IO_ERROR) {
    printf("Can\'t read input file!\n");
    return -1;
  }
    
  return 0;
}



