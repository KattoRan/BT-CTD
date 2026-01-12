/* 
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#include <stdio.h>
#include "reader.h"
#include "codegen.h"  

#define CODE_SIZE 10000
extern SymTab* symtab;

extern Object* readiFunction;
extern Object* readcFunction;
extern Object* writeiProcedure;
extern Object* writecProcedure;
extern Object* writelnProcedure;

CodeBlock* codeBlock;
// tính toán mức lồng nhau của một phạm vi
int computeNestedLevel(Scope* scope) {
  int level = 0;
  Scope* tmp = symtab->currentScope;
  while (tmp != scope) {
    tmp = tmp->outer;
    level ++;
  }
  return level;
}
// sinh mã để lấy địa chỉ của một biến
void genVariableAddress(Object* var) {
  int level = computeNestedLevel(VARIABLE_SCOPE(var));
  int offset = VARIABLE_OFFSET(var);
  genLA(level, offset);
}
// sinh mã để lấy giá trị của một biến
void genVariableValue(Object* var) {
  int level = computeNestedLevel(VARIABLE_SCOPE(var));
  int offset = VARIABLE_OFFSET(var);
  genLV(level, offset);
}
// sinh mã để lấy địa chỉ của một hằng số
void genParameterAddress(Object* param) {
  int level = computeNestedLevel(PARAMETER_SCOPE(param));
  int offset = PARAMETER_OFFSET(param);
  genLA(level, offset);
}
// sinh mã để lấy giá trị của một hằng số
void genParameterValue(Object* param) {
  int level = computeNestedLevel(PARAMETER_SCOPE(param));
  int offset = PARAMETER_OFFSET(param);
  genLV(level, offset);
}
// sinh mã để lấy địa chỉ của giá trị trả về của một hàm
void genReturnValueAddress(Object* func) {
  int level = computeNestedLevel(FUNCTION_SCOPE(func));
  int offset = RETURN_VALUE_OFFSET;
  genLA(level, offset);
}
// sinh mã để lấy giá trị trả về của một hàm
void genReturnValueValue(Object* func) {
  int level = computeNestedLevel(FUNCTION_SCOPE(func));
  int offset = RETURN_VALUE_OFFSET;
  genLV(level, offset);
}
// sinh mã để gọi một thủ tục đã được định nghĩa sẵn
void genPredefinedProcedureCall(Object* proc) {
  if (proc == writeiProcedure)
    genWRI();
  else if (proc == writecProcedure)
    genWRC();
  else if (proc == writelnProcedure)
    genWLN();
}
// sinh mã để gọi một thủ tục đã được định nghĩa
void genProcedureCall(Object* proc) {
  int level = computeNestedLevel(proc->procAttrs->scope->outer);
  genCALL(level, proc->procAttrs->codeAddress);
}
// sinh mã để gọi một hàm đã được định nghĩa sẵn
void genPredefinedFunctionCall(Object* func) {
  if (func == readiFunction)
    genRI();
  else if (func == readcFunction)
    genRC();
}
// sinh mã để gọi một hàm đã được định nghĩa
void genFunctionCall(Object* func) {
  int level = computeNestedLevel(func->funcAttrs->scope->outer);
  genCALL(level, func->funcAttrs->codeAddress);
}
// sinh mã để tải địa chỉ của một biến hoặc tham số
void genLA(int level, int offset) {
  emitLA(codeBlock, level, offset);
}
// sinh mã để tải giá trị của một biến hoặc tham số
void genLV(int level, int offset) {
  emitLV(codeBlock, level, offset);
}
// sinh mã để tải một hằng số nguyên
void genLC(WORD constant) {
  emitLC(codeBlock, constant);
}
// sinh mã để tải một hằng số ký tự
void genLI(void) {
  emitLI(codeBlock);
}
// sinh mã để tăng con trỏ ngăn xếp
void genINT(int delta) {
  emitINT(codeBlock,delta);
}
// sinh mã để giảm con trỏ ngăn xếp
void genDCT(int delta) {
  emitDCT(codeBlock,delta);
}
// sinh mã nhảy không điều kiện
Instruction* genJ(CodeAddress label) {
  Instruction* inst = codeBlock->code + codeBlock->codeSize;
  emitJ(codeBlock,label);
  return inst;
}
// sinh mã nhảy có điều kiện
Instruction* genFJ(CodeAddress label) {
  Instruction* inst = codeBlock->code + codeBlock->codeSize;
  emitFJ(codeBlock, label);
  return inst;
}
// sinh mã kết thúc chương trình
void genHL(void) {
  emitHL(codeBlock);
}
// sinh mã lưu giá trị từ ngăn xếp vào biến hoặc tham số
void genST(void) {
  emitST(codeBlock);
}
// sinh mã gọi một hàm hoặc thủ tục
void genCALL(int level, CodeAddress label) {
  emitCALL(codeBlock, level, label);
}
// sinh mã kết thúc một thủ tục
void genEP(void) {
  emitEP(codeBlock);
}
// sinh mã kết thúc một hàm
void genEF(void) {
  emitEF(codeBlock);
}
// sinh mã đọc một ký tự
void genRC(void) {
  emitRC(codeBlock);
}
// sinh mã đọc một số nguyên
void genRI(void) {
  emitRI(codeBlock);
}
// sinh mã ghi một ký tự
void genWRC(void) {
  emitWRC(codeBlock);
}
// sinh mã ghi một số nguyên
void genWRI(void) {
  emitWRI(codeBlock);
}
// sinh mã ghi xuống dòng
void genWLN(void) {
  emitWLN(codeBlock);
}
// sinh mã cộng hai số nguyên
void genAD(void) {
  emitAD(codeBlock);
}
// sinh mã trừ hai số nguyên
void genSB(void) {
  emitSB(codeBlock);
}
// sinh mã nhân hai số nguyên
void genML(void) {
  emitML(codeBlock);
}
// sinh mã chia hai số nguyên
void genDV(void) {
  emitDV(codeBlock);
}
// sinh mã phủ định một số nguyên
void genNEG(void) {
  emitNEG(codeBlock);
}
// sinh mã sao chép giá trị trên đỉnh ngăn xếp
void genCV(void) {
  emitCV(codeBlock);
}
// sinh mã so sánh hai số nguyên bằng nhau
void genEQ(void) {
  emitEQ(codeBlock);
}
// sinh mã so sánh hai số nguyên không bằng nhau
void genNE(void) {
  emitNE(codeBlock);
}
// sinh mã so sánh hai số nguyên lớn hơn
void genGT(void) {
  emitGT(codeBlock);
}
// sinh mã so sánh hai số nguyên lớn hơn hoặc bằng
void genGE(void) {
  emitGE(codeBlock);
}
// sinh mã so sánh hai số nguyên nhỏ hơn
void genLT(void) {
  emitLT(codeBlock);
}
// sinh mã so sánh hai số nguyên nhỏ hơn hoặc bằng
void genLE(void) {
  emitLE(codeBlock);
}
// cập nhật địa chỉ nhảy cho lệnh nhảy không điều kiện
void updateJ(Instruction* jmp, CodeAddress label) {
  jmp->q = label;
}
// cập nhật địa chỉ nhảy cho lệnh nhảy có điều kiện
void updateFJ(Instruction* jmp, CodeAddress label) {
  jmp->q = label;
}
// lấy địa chỉ mã lệnh hiện tại
CodeAddress getCurrentCodeAddress(void) {
  return codeBlock->codeSize;
}
// kiểm tra xem một hàm đã được khai báo chưa
int isPredefinedFunction(Object* func) {
  return ((func == readiFunction) || (func == readcFunction));
}
// kiểm tra xem một thủ tục đã được khai báo chưa
int isPredefinedProcedure(Object* proc) {
  return ((proc == writeiProcedure) || (proc == writecProcedure) || (proc == writelnProcedure));
}
// khởi tạo bộ đệm mã
void initCodeBuffer(void) {
  codeBlock = createCodeBlock(CODE_SIZE);
}
// in mã lệnh
void printCodeBuffer(void) {
  printCodeBlock(codeBlock);
}
// giải phóng bộ đệm mã
void cleanCodeBuffer(void) {
  freeCodeBlock(codeBlock);
}
// lưu mã lệnh vào file
int serialize(char* fileName) {
  FILE* f;

  f = fopen(fileName, "wb");
  if (f == NULL) return IO_ERROR;
  saveCode(codeBlock, f);
  fclose(f);
  return IO_SUCCESS;
}
