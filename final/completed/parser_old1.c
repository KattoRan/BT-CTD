/* 
 * File parser.c:
 * - Recursive-descent parser cho KPL (giống Pascal mini)
 * - Vừa phân tích cú pháp (parse), vừa kiểm tra ngữ nghĩa (semantics),
 *   vừa sinh mã trung gian/bytecode (codegen).
 */

#include <stdio.h>
#include <stdlib.h>

#include "reader.h"     // đọc file nguồn
#include "scanner.h"    // tách token
#include "parser.h"     // khai báo các hàm compile...
#include "semantics.h"  // kiểm tra khai báo/kiểu
#include "error.h"      // báo lỗi cú pháp/ngữ nghĩa
#include "debug.h"      // in debug (nếu cần)
#include "codegen.h"    // sinh bytecode (genLC, genAD, genJ, ...)

/* currentToken: token vừa "ăn" xong
 * lookAhead: token kế tiếp để nhìn trước (1-token lookahead)
 */
Token *currentToken;
Token *lookAhead;

/* kiểu cơ bản + symbol table được khởi tạo ở symtab.c */
extern Type* intType;
extern Type* charType;
extern SymTab* symtab;

/* ============================================================
 * 1) TOKEN UTILITIES: scan() và eat()
 * ============================================================
 */

/* scan():
 * - dịch token: currentToken <- lookAhead
 * - đọc token mới vào lookAhead
 * - giải phóng token cũ (tmp) để tránh leak
 */
void scan(void) {
  Token* tmp = currentToken;
  currentToken = lookAhead;
  lookAhead = getValidToken();
  free(tmp);
}

/* eat(tokenType):
 * - nếu lookAhead đúng loại token cần -> scan() để "ăn" nó
 * - nếu không đúng -> báo lỗi thiếu token (missingToken)
 */
void eat(TokenType tokenType) {
  if (lookAhead->tokenType == tokenType) {
    // printToken(lookAhead); // bật nếu muốn debug token
    scan();
  } else missingToken(tokenType, lookAhead->lineNo, lookAhead->colNo);
}

/* ============================================================
 * 2) PROGRAM & BLOCK
 * ============================================================
 */

/* compileProgram():
 * parse cú pháp:
 *   PROGRAM ident ; block .
 * - tạo Program object trong symbol table
 * - vào scope của chương trình (enterBlock)
 * - compileBlock() cho phần thân
 * - kết thúc bằng '.' và sinh lệnh HL để dừng chương trình
 */
void compileProgram(void) {
  Object* program;

  eat(KW_PROGRAM);
  eat(TK_IDENT);

  program = createProgramObject(currentToken->string);
  program->progAttrs->codeAddress = getCurrentCodeAddress(); // địa chỉ bắt đầu code
  enterBlock(program->progAttrs->scope);                    // vào scope chương trình

  eat(SB_SEMICOLON);

  compileBlock();     // biên dịch khối chính
  eat(SB_PERIOD);     // kết thúc chương trình

  genHL();            // halt

  exitBlock();        // ra khỏi scope chương trình
}

/* compileBlock():
 * block gồm:
 *   const/type/var/subdecls
 *   BEGIN statements END
 *
 * trick quan trọng:
 * - genJ(...) trước để nhảy qua code của function/procedure khai báo phía trước
 * - sau khi compileSubDecls xong thì updateJ nhảy về đây
 * - genINT(frameSize) cấp phát stack frame cho biến cục bộ
 */
void compileBlock(void) {
  Instruction* jmp;
  
  jmp = genJ(DC_VALUE);  // nhảy tạm thời (sẽ update sau)

  compileConstDecls();   // CONST ...
  compileTypeDecls();    // TYPE ...
  compileVarDecls();     // VAR ...
  compileSubDecls();     // FUNCTION/PROCEDURE ...

  updateJ(jmp, getCurrentCodeAddress());           // quay về code chính sau khai báo con
  genINT(symtab->currentScope->frameSize);         // cấp stack frame cho scope hiện tại

  eat(KW_BEGIN);
  compileStatements();                              // chuỗi statement
  eat(KW_END);
}

/* ============================================================
 * 3) DECLARATIONS: CONST / TYPE / VAR
 * ============================================================
 */

/* compileConstDecls():
 * parse:
 *   CONST ident = constant ; { ident = constant ; }
 * - checkFreshIdent: không trùng tên trong scope hiện tại
 * - declareObject: đưa vào symbol table
 */
void compileConstDecls(void) {
  Object* constObj;
  ConstantValue* constValue;

  if (lookAhead->tokenType == KW_CONST) {
    eat(KW_CONST);
    do {
      eat(TK_IDENT);
      checkFreshIdent(currentToken->string);
      constObj = createConstantObject(currentToken->string);
      declareObject(constObj);
      
      eat(SB_EQ);
      constValue = compileConstant();              // đọc giá trị hằng
      constObj->constAttrs->value = constValue;
      
      eat(SB_SEMICOLON);
    } while (lookAhead->tokenType == TK_IDENT);
  }
}

/* compileTypeDecls():
 * parse:
 *   TYPE ident = type ; { ident = type ; }
 */
void compileTypeDecls(void) {
  Object* typeObj;
  Type* actualType;

  if (lookAhead->tokenType == KW_TYPE) {
    eat(KW_TYPE);
    do {
      eat(TK_IDENT);
      
      checkFreshIdent(currentToken->string);
      typeObj = createTypeObject(currentToken->string);
      declareObject(typeObj);
      
      eat(SB_EQ);
      actualType = compileType();                  // đọc mô tả kiểu
      typeObj->typeAttrs->actualType = actualType;
      
      eat(SB_SEMICOLON);
    } while (lookAhead->tokenType == TK_IDENT);
  } 
}

/* compileVarDecls():
 * parse:
 *   VAR ident : type ; { ident : type ; }
 * - mỗi biến sẽ được symtab gán offset trong stack frame
 */
void compileVarDecls(void) {
  Object* varObj;
  Type* varType;

  if (lookAhead->tokenType == KW_VAR) {
    eat(KW_VAR);
    do {
      eat(TK_IDENT);
      checkFreshIdent(currentToken->string);
      varObj = createVariableObject(currentToken->string);
      eat(SB_COLON);
      varType = compileType();
      varObj->varAttrs->type = varType;
      declareObject(varObj);      
      eat(SB_SEMICOLON);
    } while (lookAhead->tokenType == TK_IDENT);
  } 
}

/* ============================================================
 * 4) SUBPROGRAM: FUNCTION / PROCEDURE
 * ============================================================
 */

/* compileSubDecls(): lặp cho đến khi hết FUNCTION/PROCEDURE */
void compileSubDecls(void) {
  while ((lookAhead->tokenType == KW_FUNCTION) || (lookAhead->tokenType == KW_PROCEDURE)) {
    if (lookAhead->tokenType == KW_FUNCTION)
      compileFuncDecl();
    else compileProcDecl();
  }
}

/* compileFuncDecl():
 * parse:
 *   FUNCTION ident (params)? : basicType ; block ; 
 * - tạo function object, set codeAddress
 * - enterBlock(scope của function) để khai báo param/var cục bộ
 * - kết thúc bằng genEF() (exit function)
 */
void compileFuncDecl(void) {
  Object* funcObj;
  Type* returnType;

  eat(KW_FUNCTION);
  eat(TK_IDENT);

  checkFreshIdent(currentToken->string);
  funcObj = createFunctionObject(currentToken->string);
  funcObj->funcAttrs->codeAddress = getCurrentCodeAddress();
  declareObject(funcObj);

  enterBlock(funcObj->funcAttrs->scope);
  
  compileParams();

  eat(SB_COLON);
  returnType = compileBasicType();
  funcObj->funcAttrs->returnType = returnType;

  eat(SB_SEMICOLON);

  compileBlock();

  genEF();
  eat(SB_SEMICOLON);

  exitBlock();
}

/* compileProcDecl():
 * parse:
 *   PROCEDURE ident (params)? ; block ;
 * - kết thúc bằng genEP() (exit procedure)
 */
void compileProcDecl(void) {
  Object* procObj;

  eat(KW_PROCEDURE);
  eat(TK_IDENT);

  checkFreshIdent(currentToken->string);
  procObj = createProcedureObject(currentToken->string);
  procObj->procAttrs->codeAddress = getCurrentCodeAddress();
  declareObject(procObj);

  enterBlock(procObj->procAttrs->scope);

  compileParams();

  eat(SB_SEMICOLON);
  compileBlock();

  genEP();
  eat(SB_SEMICOLON);

  exitBlock();
}

/* ============================================================
 * 5) CONSTANTS & TYPES
 * ============================================================
 */

/* compileUnsignedConstant():
 * - constant không có dấu +/- phía trước
 * - nhận: number | ident(constant) | char
 */
ConstantValue* compileUnsignedConstant(void) {
  ConstantValue* constValue;
  Object* obj;

  switch (lookAhead->tokenType) {
  case TK_NUMBER:
    eat(TK_NUMBER);
    constValue = makeIntConstant(currentToken->value);
    break;
  case TK_IDENT:
    eat(TK_IDENT);
    obj = checkDeclaredConstant(currentToken->string);
    constValue = duplicateConstantValue(obj->constAttrs->value);
    break;
  case TK_CHAR:
    eat(TK_CHAR);
    constValue = makeCharConstant(currentToken->string[0]);
    break;
  default:
    error(ERR_INVALID_CONSTANT, lookAhead->lineNo, lookAhead->colNo);
    break;
  }
  return constValue;
}

/* compileConstant():
 * - cho phép +number, -number, hoặc char
 * - nếu '-' thì đổi dấu intValue
 */
ConstantValue* compileConstant(void) {
  ConstantValue* constValue;

  switch (lookAhead->tokenType) {
  case SB_PLUS:
    eat(SB_PLUS);
    constValue = compileConstant2();
    break;
  case SB_MINUS:
    eat(SB_MINUS);
    constValue = compileConstant2();
    constValue->intValue = - constValue->intValue;
    break;
  case TK_CHAR:
    eat(TK_CHAR);
    constValue = makeCharConstant(currentToken->string[0]);
    break;
  default:
    constValue = compileConstant2();
    break;
  }
  return constValue;
}

/* compileConstant2():
 * - phần lõi cho hằng số INT: number | ident(int-constant)
 */
ConstantValue* compileConstant2(void) {
  ConstantValue* constValue;
  Object* obj;

  switch (lookAhead->tokenType) {
  case TK_NUMBER:
    eat(TK_NUMBER);
    constValue = makeIntConstant(currentToken->value);
    break;
  case TK_IDENT:
    eat(TK_IDENT);
    obj = checkDeclaredConstant(currentToken->string);
    if (obj->constAttrs->value->type == TP_INT)
      constValue = duplicateConstantValue(obj->constAttrs->value);
    else
      error(ERR_UNDECLARED_INT_CONSTANT,currentToken->lineNo, currentToken->colNo);
    break;
  default:
    error(ERR_INVALID_CONSTANT, lookAhead->lineNo, lookAhead->colNo);
    break;
  }
  return constValue;
}

/* compileType():
 * parse type:
 *   INTEGER | CHAR | ARRAY (. number .) OF type | ident(typeName)
 */
Type* compileType(void) {
  Type* type;
  Type* elementType;
  int arraySize;
  Object* obj;

  switch (lookAhead->tokenType) {
  case KW_INTEGER: 
    eat(KW_INTEGER);
    type = makeIntType();
    break;
  case KW_CHAR: 
    eat(KW_CHAR); 
    type = makeCharType();
    break;
  case KW_STRING:              // THÊM
    eat(KW_STRING);
    type = makeStringType();   // bạn phải tự định nghĩa
    break;

  case KW_BYTES:               // THÊM
    eat(KW_BYTES);
    type = makeBytesType();    // bạn phải tự định nghĩa
    break;
  case KW_ARRAY:
    eat(KW_ARRAY);
    eat(SB_LSEL);
    eat(TK_NUMBER);
    arraySize = currentToken->value;
    eat(SB_RSEL);
    eat(KW_OF);
    elementType = compileType();
    type = makeArrayType(arraySize, elementType);
    break;
  case TK_IDENT:
    eat(TK_IDENT);
    obj = checkDeclaredType(currentToken->string);
    type = duplicateType(obj->typeAttrs->actualType);
    break;
  default:
    error(ERR_INVALID_TYPE, lookAhead->lineNo, lookAhead->colNo);
    break;
  }
  return type;
}

/* compileBasicType(): chỉ cho phép INTEGER hoặc CHAR (dùng cho return type/param) */
Type* compileBasicType(void) {
  Type* type;

  switch (lookAhead->tokenType) {
  case KW_INTEGER: 
    eat(KW_INTEGER); 
    type = makeIntType();
    break;
  case KW_CHAR: 
    eat(KW_CHAR); 
    type = makeCharType();
    break;
  default:
    error(ERR_INVALID_BASICTYPE, lookAhead->lineNo, lookAhead->colNo);
    break;
  }
  return type;
}

/* ============================================================
 * 6) PARAMETERS
 * ============================================================
 */

/* compileParams():
 * parse:
 *   '(' param {';' param} ')'
 * nếu không có '(' thì không có tham số
 */
void compileParams(void) {
  if (lookAhead->tokenType == SB_LPAR) {
    eat(SB_LPAR);
    compileParam();
    while (lookAhead->tokenType == SB_SEMICOLON) {
      eat(SB_SEMICOLON);
      compileParam();
    }
    eat(SB_RPAR);
  }
}

/* compileParam():
 * parse:
 *   [VAR] ident : basicType
 * - VAR -> truyền tham chiếu (PARAM_REFERENCE)
 * - không VAR -> truyền giá trị (PARAM_VALUE)
 */
void compileParam(void) {
  Object* param;
  Type* type;
  enum ParamKind paramKind = PARAM_VALUE;

  if (lookAhead->tokenType == KW_VAR) {
    paramKind = PARAM_REFERENCE;
    eat(KW_VAR);
  }

  eat(TK_IDENT);
  checkFreshIdent(currentToken->string);
  param = createParameterObject(currentToken->string, paramKind);
  eat(SB_COLON);
  type = compileBasicType();
  param->paramAttrs->type = type;
  declareObject(param); // symtab sẽ gán offset + add vào paramList của owner
}

/* ============================================================
 * 7) STATEMENTS
 * ============================================================
 */

/* compileStatements():
 * parse:
 *   statement {';' statement}
 */
void compileStatements(void) {
  compileStatement();
  while (lookAhead->tokenType == SB_SEMICOLON) {
    eat(SB_SEMICOLON);
    compileStatement();
  }
}

/* compileStatement(): chọn loại statement theo token đầu */
void compileStatement(void) {
  switch (lookAhead->tokenType) {
  case TK_IDENT:  compileAssignSt(); break; // gán: x := ...
  case KW_CALL:   compileCallSt();   break; // call proc
  case KW_BEGIN:  compileGroupSt();  break; // begin ... end
  case KW_IF:     compileIfSt();     break; // if ... then ... else ...
  case KW_WHILE:  compileWhileSt();  break; // while ... do ...
  case KW_FOR:    compileForSt();    break; // for i := ... to ... do ...
  case SB_SEMICOLON:
  case KW_END:
  case KW_ELSE:
    /* empty statement (FOLLOW set) */
    break;
  default:
    error(ERR_INVALID_STATEMENT, lookAhead->lineNo, lookAhead->colNo);
    break;
  }
}

/* ============================================================
 * 8) LVALUE + ASSIGN
 * ============================================================
 */

/* compileLValue():
 * - parse ident [indexes]
 * - sinh code để lấy "địa chỉ" vùng nhớ của LValue
 * - trả về Type của LValue để check kiểu khi gán
 */
Type* compileLValue(void) {
  Object* var;
  Type* varType;

  eat(TK_IDENT);
  var = checkDeclaredLValueIdent(currentToken->string);

  switch (var->kind) {
  case OBJ_VARIABLE:
    genVariableAddress(var); // push địa chỉ biến lên stack
    if (var->varAttrs->type->typeClass == TP_ARRAY)
      varType = compileIndexes(var->varAttrs->type); // tính địa chỉ phần tử mảng
    else
      varType = var->varAttrs->type;
    break;

  case OBJ_PARAMETER:
    /* param truyền giá trị -> cần lấy địa chỉ ô param
       param VAR -> param chứa địa chỉ -> lấy "giá trị" (địa chỉ thật) */
    if (var->paramAttrs->kind == PARAM_VALUE)
      genParameterAddress(var);
    else
      genParameterValue(var);
    varType = var->paramAttrs->type;
    break;

  case OBJ_FUNCTION:
    /* trong Pascal: gán vào tên hàm để set return value */
    genReturnValueAddress(var);
    varType = var->funcAttrs->returnType;
    break;

  default:
    error(ERR_INVALID_LVALUE,currentToken->lineNo, currentToken->colNo);
  }

  return varType;
}

/* compileAssignSt():
 * parse: lvalue := expression
 * - checkTypeEquality để đảm bảo kiểu khớp
 * - genST để store (lưu) giá trị vào địa chỉ LValue
 */
// void compileAssignSt(void) {
//   Type* varType;
//   Type* expType;

//   varType = compileLValue();
//   eat(SB_ASSIGN);

//   expType = compileExpression();
//   checkTypeEquality(varType, expType);

//   genST();
// }

void compileAssignSt(void) {
  // offset base của vùng tạm nằm ngay sau frameSize hiện tại
  int base = symtab->currentScope->frameSize;

  int k = 0;                 // số biến bên trái
  int cap = 8;
  Type** lhsTypes = (Type**)malloc(cap * sizeof(Type*));

  /* ====== 1) Đọc danh sách LHS: lvalue (',' lvalue)* ======
     Với mỗi LHS:
     - INT 1: xin 1 ô tạm
     - LA 0, base+i: địa chỉ ô tạm
     - compileLValue(): đẩy địa chỉ biến thật
     - ST: lưu địa chỉ biến vào ô tạm
  */
  while (1) {
    if (k >= cap) {
      cap *= 2;
      lhsTypes = (Type**)realloc(lhsTypes, cap * sizeof(Type*));
    }

    genINT(1);           // cấp 1 ô temp cho địa chỉ LHS[k]
    genLA(0, base + k);  // push address(temp_slot)
    lhsTypes[k] = compileLValue();  // push address(real_lvalue)
    genST();             // temp_slot := real_lvalue_address
    k++;

    if (lookAhead->tokenType == SB_COMMA) {
      eat(SB_COMMA);
      continue;
    }
    break;
  }

  /* Dạng cũ (1 biến) vẫn chạy vì k có thể = 1 */
  eat(SB_ASSIGN);

  /* ====== 2) Đọc danh sách RHS: expr (',' expr)* với đúng k expr ======
     Mỗi RHS:
     - INT 1: xin 1 ô temp cho value
     - LA 0, base+k+i
     - compileExpression()
     - checkTypeEquality(LHS[i], RHS[i])
     - ST
  */
  for (int i = 0; i < k; i++) {
    if (i > 0) eat(SB_COMMA);

    genINT(1);                 // cấp 1 ô temp cho value[i]
    genLA(0, base + k + i);    // push address(temp_value_slot)

    Type* rhsType = compileExpression();
    checkTypeEquality(lhsTypes[i], rhsType);

    genST();                   // temp_value_slot := rhs_value
  }

  // Nếu còn dấu ',' nữa => RHS nhiều hơn LHS => báo lỗi
  if (lookAhead->tokenType == SB_COMMA) {
    error(ERR_INVALID_STATEMENT, lookAhead->lineNo, lookAhead->colNo);
  }

  /* ====== 3) Gán thật: *(addr[i]) = value[i] ======
     - LV 0, base+i      -> lấy địa chỉ LHS[i]
     - LV 0, base+k+i    -> lấy value[i]
     - ST
  */
  for (int i = k - 1; i >= 0; i--) {
    genLV(0, base + i);
    genLV(0, base + k + i);
    genST();
  }

  // bỏ toàn bộ vùng tạm (k ô addr + k ô value)
  genDCT(2 * k);

  free(lhsTypes);
}


/* ============================================================
 * 9) CALL + ARGUMENTS
 * ============================================================
 */

/* compileCallSt():
 * parse: CALL ident (args)?
 * - kiểm tra procedure tồn tại
 * - nếu predefined -> gọi codegen riêng
 * - nếu user-defined -> chuẩn bị stack, truyền args, gọi proc
 */
void compileCallSt(void) {
  Object* proc;

  eat(KW_CALL);
  eat(TK_IDENT);

  proc = checkDeclaredProcedure(currentToken->string);

  /* NOTE: đoạn "if (proc == NULL)" thực tế ít xảy ra vì checkDeclaredProcedure đã báo lỗi.
     Ý tưởng: nếu là predefined function/proc thì xử lý đặc biệt. */
  if (proc == NULL) {
    if (isPredefinedFunction(proc)) {
      compileArguments(proc->funcAttrs->paramList);
      genPredefinedFunctionCall(proc);
    }
  } else {
    if (isPredefinedProcedure(proc)) {
      compileArguments(proc->procAttrs->paramList);
      genPredefinedProcedureCall(proc);
    } else {
      genINT(RESERVED_WORDS); // cấp vùng reserved cho activation record
      compileArguments(proc->procAttrs->paramList);
      genDCT(RESERVED_WORDS + proc->procAttrs->paramCount); // dọn args
      genProcedureCall(proc);
    }
  }
}

/* begin ... end statement */
void compileGroupSt(void) {
  eat(KW_BEGIN);
  compileStatements();
  eat(KW_END);
}

/* ============================================================
 * 10) IF / WHILE / FOR (CONTROL FLOW)
 * ============================================================
 */

/* compileIfSt():
 * - sinh nhảy có điều kiện:
 *   FJ addrFalse (tạm)
 *   [then-st]
 *   nếu có else: J addrEnd (tạm), patch FJ -> else, patch J -> end
 */
void compileIfSt(void) {
  Instruction* fjInstruction;
  Instruction* jInstruction;

  eat(KW_IF);
  compileCondition();
  eat(KW_THEN);

  fjInstruction = genFJ(DC_VALUE);
  compileStatement();

  if (lookAhead->tokenType == KW_ELSE) {
    jInstruction = genJ(DC_VALUE);
    updateFJ(fjInstruction, getCurrentCodeAddress());
    eat(KW_ELSE);
    compileStatement();
    updateJ(jInstruction, getCurrentCodeAddress());
  } else {
    updateFJ(fjInstruction, getCurrentCodeAddress());
  }
}

/* compileWhileSt():
 * begin:
 *   condition
 *   FJ end
 *   body
 *   J begin
 * end:
 */
void compileWhileSt(void) {
  CodeAddress beginWhile;
  Instruction* fjInstruction;

  beginWhile = getCurrentCodeAddress();
  eat(KW_WHILE);
  compileCondition();
  fjInstruction = genFJ(DC_VALUE);

  eat(KW_DO);
  compileStatement();

  genJ(beginWhile);
  updateFJ(fjInstruction, getCurrentCodeAddress());
}

/* compileForSt(): for i := exp1 to exp2 do st
 * - gán i = exp1
 * - so sánh i <= exp2, nếu false -> thoát
 * - body
 * - i = i + 1
 * - nhảy lại
 */
void compileForSt(void) {
  CodeAddress beginLoop;
  Instruction* fjInstruction;
  Type* varType;
  Type* type;

  eat(KW_FOR);

  varType = compileLValue();
  eat(SB_ASSIGN);

  genCV(); // copy địa chỉ LValue để dùng nhiều lần
  type = compileExpression();
  checkTypeEquality(varType, type);
  genST();

  genCV();
  genLI();              // load i
  beginLoop = getCurrentCodeAddress();

  eat(KW_TO);
  type = compileExpression(); // exp2
  checkTypeEquality(varType, type);

  genLE();                     // i <= exp2 ?
  fjInstruction = genFJ(DC_VALUE);

  eat(KW_DO);
  compileStatement();

  // i := i + 1
  genCV();
  genCV();
  genLI();
  genLC(1);
  genAD();
  genST();

  genCV();
  genLI();

  genJ(beginLoop);
  updateFJ(fjInstruction, getCurrentCodeAddress());
  genDCT(1); // dọn stack
}

/* ============================================================
 * 11) ARGUMENT LIST (gọi hàm/thủ tục)
 * ============================================================
 */

/* compileArgument():
 * - nếu param truyền value -> compileExpression
 * - nếu param VAR -> compileLValue (lấy địa chỉ)
 */
void compileArgument(Object* param) {
  Type* type;

  if (param->paramAttrs->kind == PARAM_VALUE) {
    type = compileExpression();
    checkTypeEquality(type, param->paramAttrs->type);
  } else {
    type = compileLValue();
    checkTypeEquality(type, param->paramAttrs->type);
  }
}

/* compileArguments(paramList):
 * parse:
 *   '(' arg {',' arg} ')'
 * - kiểm tra số lượng tham số và kiểu có khớp paramList không
 * - nếu không có '(' thì chấp nhận FOLLOW set (không có args)
 */
void compileArguments(ObjectNode* paramList) {
  ObjectNode* node = paramList;

  switch (lookAhead->tokenType) {
  case SB_LPAR:
    eat(SB_LPAR);

    if (node == NULL)
      error(ERR_PARAMETERS_ARGUMENTS_INCONSISTENCY, currentToken->lineNo, currentToken->colNo);

    compileArgument(node->object);
    node = node->next;

    while (lookAhead->tokenType == SB_COMMA) {
      eat(SB_COMMA);
      if (node == NULL)
        error(ERR_PARAMETERS_ARGUMENTS_INCONSISTENCY, currentToken->lineNo, currentToken->colNo);
      compileArgument(node->object);
      node = node->next;
    }

    if (node != NULL)
      error(ERR_PARAMETERS_ARGUMENTS_INCONSISTENCY, currentToken->lineNo, currentToken->colNo);

    eat(SB_RPAR);
    break;

  /* FOLLOW set: token cho biết arguments không xuất hiện ở đây */
  case SB_TIMES: case SB_SLASH: case SB_PLUS: case SB_MINUS:
  case KW_TO: case KW_DO:
  case SB_RPAR: case SB_COMMA:
  case SB_EQ: case SB_NEQ: case SB_LE: case SB_LT: case SB_GE: case SB_GT:
  case SB_RSEL:
  case SB_SEMICOLON:
  case KW_END: case KW_ELSE: case KW_THEN:
    break;

  default:
    error(ERR_INVALID_ARGUMENTS, lookAhead->lineNo, lookAhead->colNo);
  }
}

/* ============================================================
 * 12) CONDITION + EXPRESSIONS
 * ============================================================
 */

/* compileCondition():
 * parse: expression (==,!=,<,<=,>,>=) expression
 * - checkBasicType: chỉ cho int/char so sánh
 * - sinh opcode so sánh tương ứng (genEQ/genLT/...)
 */
void compileCondition(void) {
  Type* type1;
  Type* type2;
  TokenType op;

  type1 = compileExpression();
  checkBasicType(type1);

  op = lookAhead->tokenType;
  switch (op) {
  case SB_EQ:  eat(SB_EQ);  break;
  case SB_NEQ: eat(SB_NEQ); break;
  case SB_LE:  eat(SB_LE);  break;
  case SB_LT:  eat(SB_LT);  break;
  case SB_GE:  eat(SB_GE);  break;
  case SB_GT:  eat(SB_GT);  break;
  default:
    error(ERR_INVALID_COMPARATOR, lookAhead->lineNo, lookAhead->colNo);
  }

  type2 = compileExpression();
  checkTypeEquality(type1, type2);

  switch (op) {
  case SB_EQ:  genEQ(); break;
  case SB_NEQ: genNE(); break;
  case SB_LE:  genLE(); break;
  case SB_LT:  genLT(); break;
  case SB_GE:  genGE(); break;
  case SB_GT:  genGT(); break;
  default: break;
  }
}

/* compileExpression(): xử lý:
 * - if <Condition> return <Expression> else return <Expression>
 * - unary + / -
 */
Type* compileExpression(void) {
  Type* type;
  /* ---- NEW: if-expression ----
   * <Expression> ::= if <Condition> return <Expression> else return <Expression>
   * Ý nghĩa: nếu Condition đúng -> giá trị là expr sau return đầu
   *          sai -> expr sau else return
   */
  if (lookAhead->tokenType == KW_IF) {
    Type* typeTrue;
    Type* typeFalse;
    Instruction* fjInst;
    Instruction* jInst;

    eat(KW_IF);
    compileCondition();
    eat(KW_RETURN);
    fjInst = genFJ(DC_VALUE);

    typeTrue = compileExpression();
    jInst = genJ(DC_VALUE);

    updateFJ(fjInst, getCurrentCodeAddress());

    eat(KW_ELSE);
    eat(KW_RETURN);
    typeFalse = compileExpression();
    checkTypeEquality(typeTrue, typeFalse);

    updateJ(jInst, getCurrentCodeAddress());
    return typeTrue;
  }
  /* ---- OLD: unary + / - ---- */
  switch (lookAhead->tokenType) {
  case SB_PLUS:
    eat(SB_PLUS);
    type = compileExpression2();
    checkIntType(type);
    break;
  case SB_MINUS:
    eat(SB_MINUS);
    type = compileExpression2();
    checkIntType(type);
    genNEG(); // đổi dấu
    break;
  default:
    type = compileExpression2();
  }
  return type;
}

/* compileExpression2(): term rồi xử lý tiếp chuỗi + - */
Type* compileExpression2(void) {
  Type* type = compileTerm();
  return compileExpression3(type);
}

/* compileExpression3(type1): xử lý + / - dạng trái kết hợp (left-assoc) */
Type* compileExpression3(Type* argType1) {
  Type* argType2;
  Type* resultType;

  switch (lookAhead->tokenType) {
  case SB_PLUS:
    eat(SB_PLUS);
    checkIntType(argType1);
    argType2 = compileTerm();
    checkIntType(argType2);
    genAD();
    resultType = compileExpression3(argType1);
    break;

  case SB_MINUS:
    eat(SB_MINUS);
    checkIntType(argType1);
    argType2 = compileTerm();
    checkIntType(argType2);
    genSB();
    resultType = compileExpression3(argType1);
    break;

  /* FOLLOW set -> kết thúc expression */
  case KW_TO: case KW_DO:
  case SB_RPAR: case SB_COMMA:
  case SB_EQ: case SB_NEQ: case SB_LE: case SB_LT: case SB_GE: case SB_GT:
  case SB_RSEL:
  case SB_SEMICOLON:
  case KW_END: case KW_ELSE: case KW_THEN:
  case KW_RETURN:          /* <-- thêm dòng này */
    resultType = argType1;
    break;

  default:
    error(ERR_INVALID_EXPRESSION, lookAhead->lineNo, lookAhead->colNo);
  }
  return resultType;
}

/* compileTerm(): factor rồi xử lý tiếp chuỗi * / */
Type* compileTerm(void) {
  Type* type = compileFactor();
  return compileTerm2(type);
}

/* compileTerm2(): xử lý * và / */
Type* compileTerm2(Type* argType1) {
  Type* argType2;
  Type* resultType;

  switch (lookAhead->tokenType) {
  case SB_TIMES:
    eat(SB_TIMES);
    checkIntType(argType1);
    argType2 = compileFactor();
    checkIntType(argType2);
    genML();
    resultType = compileTerm2(argType1);
    break;

  case SB_SLASH:
    eat(SB_SLASH);
    checkIntType(argType1);
    argType2 = compileFactor();
    checkIntType(argType2);
    genDV();
    resultType = compileTerm2(argType1);
    break;

  /* FOLLOW set -> kết thúc term */
  case SB_PLUS: case SB_MINUS:
  case KW_TO: case KW_DO:
  case SB_RPAR: case SB_COMMA:
  case SB_EQ: case SB_NEQ: case SB_LE: case SB_LT: case SB_GE: case SB_GT:
  case SB_RSEL:
  case SB_SEMICOLON:
  case KW_END: case KW_ELSE: case KW_THEN:
  case KW_RETURN:
    resultType = argType1;
    break;

  default:
    error(ERR_INVALID_TERM, lookAhead->lineNo, lookAhead->colNo);
  }
  return resultType;
}

/* compileFactor(): đơn vị nhỏ nhất của biểu thức
 * - number/char: genLC
 * - ident: có thể là constant/variable/parameter/function -> sinh code tương ứng
 * - '(' expression ')'
 */
Type* compileFactor(void) {
  Type* type;
  Object* obj;

  switch (lookAhead->tokenType) {
  case TK_NUMBER:
    eat(TK_NUMBER);
    type = intType;
    genLC(currentToken->value);
    break;

  case TK_CHAR:
    eat(TK_CHAR);
    type = charType;
    genLC(currentToken->value);
    break;

  case TK_STRING:
    eat(TK_STRING);
    type = makeStringType();     // hoặc char array / string type bạn định nghĩa
    // genLCString(currentToken->string); // sinh hằng chuỗi
  break;


  case TK_IDENT:
    eat(TK_IDENT);
    obj = checkDeclaredIdent(currentToken->string);

    switch (obj->kind) {
    case OBJ_CONSTANT:
      if (obj->constAttrs->value->type == TP_INT) {
        type = intType;
        genLC(obj->constAttrs->value->intValue);
      } else {
        type = charType;
        genLC(obj->constAttrs->value->charValue);
      }
      break;

    case OBJ_VARIABLE:
      if (obj->varAttrs->type->typeClass == TP_ARRAY) {
        genVariableAddress(obj);
        type = compileIndexes(obj->varAttrs->type);
        genLI(); // load giá trị tại địa chỉ phần tử
      } else {
        type = obj->varAttrs->type;
        genVariableValue(obj);
      }
      break;

    case OBJ_PARAMETER:
      type = obj->paramAttrs->type;
      genParameterValue(obj);
      if (obj->paramAttrs->kind == PARAM_REFERENCE)
        genLI(); // param VAR: giá trị là địa chỉ -> cần LI để lấy giá trị thật
      break;

    case OBJ_FUNCTION:
      if (isPredefinedFunction(obj)) {
        compileArguments(obj->funcAttrs->paramList);
        genPredefinedFunctionCall(obj);
      } else {
        genINT(4); // reserved words cho activation record của function
        compileArguments(obj->funcAttrs->paramList);
        genDCT(4 + obj->funcAttrs->paramCount);
        genFunctionCall(obj);
      }
      type = obj->funcAttrs->returnType;
      break;

    default:
      error(ERR_INVALID_FACTOR, currentToken->lineNo, currentToken->colNo);
      break;
    }
    break;

  case SB_LPAR:
    eat(SB_LPAR);
    type = compileExpression();
    eat(SB_RPAR);
    break;

  default:
    error(ERR_INVALID_FACTOR, lookAhead->lineNo, lookAhead->colNo);
  }

  return type;
}

/* compileIndexes(arrayType):
 * - xử lý chuỗi chỉ số mảng: (.exp.) (.exp.) ...
 * - mỗi chỉ số: tính offset = index * size(elementType) rồi cộng vào base address
 * - trả về kiểu phần tử cuối cùng (phải là basicType)
 */
Type* compileIndexes(Type* arrayType) {
  Type* type;

  while (lookAhead->tokenType == SB_LSEL) {
    eat(SB_LSEL);

    type = compileExpression();
    checkIntType(type);
    checkArrayType(arrayType);

    genLC(sizeOfType(arrayType->elementType));
    genML();
    genAD();

    arrayType = arrayType->elementType;
    eat(SB_RSEL);
  }

  checkBasicType(arrayType);
  return arrayType;
}

/* ============================================================
 * 13) ENTRY: compile(fileName)
 * ============================================================
 */

/* compile(fileName):
 * - mở file nguồn
 * - lấy token đầu tiên vào lookAhead
 * - init symbol table (predefined READ/WRITE + types)
 * - compileProgram()
 * - dọn dẹp token/symtab và đóng file
 */
int compile(char *fileName) {
  if (openInputStream(fileName) == IO_ERROR)
    return IO_ERROR;

  currentToken = NULL;
  lookAhead = getValidToken();

  initSymTab();
  compileProgram();

  cleanSymTab();
  free(currentToken);
  free(lookAhead);
  closeInputStream();
  return IO_SUCCESS;
}
