#include "astnodes.h"
#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/TargetParser/Host.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"
#include <string>
#include <map>

using std::string;
using std::make_unique;
using std::unique_ptr;
using std::map;
using std::cout;
using namespace llvm;

extern string br;
extern string sp;
extern string nl;
extern Value *llvmnull;

string StmtASTnode::to_string(string pre) const {
  int i = 1;
  string npre = pre;
  string ans = "this should not be seen";
  if (whichtype=="expr_stmt"){
      ans = expr_stmt->to_string(npre);
  } else if (whichtype=="block"){
      ans = block->to_string(npre);
  } else if (whichtype=="if_stmt"){
      ans = if_stmt->to_string(npre);
  } else if (whichtype=="while_stmt"){
      ans = while_stmt->to_string(npre);
  } else if (whichtype=="return_stmt"){
      ans = return_stmt->to_string(npre);
  };
  return ans;
};

string BlockASTnode::to_string(string pre) const {
  string npre = pre + sp;
  string ans = pre + "BlockASTnode:" + nl;
  for (auto &ld : localdecls){
    ans += ld->to_string(npre);
  }
  for (auto &stmt : stmtlist){
    ans += stmt->to_string(npre);
  }    
  return ans;
};

string ExprASTnode::to_string(string pre) const {
  string ans;
  ans = pre + "ExprASTnode:" + nl;
  string npre = pre + sp;
  if (type == "assign"){
    ans += assign->to_string(npre);
  } else if (type == "binop"){
    ans += binop->to_string(npre);
  } else if (type == "unop"){
    ans += unop->to_string(npre);
  } else if (type == "ident"){
    ans += ident->to_string(npre);
  } else if (type == "funcall"){
    ans += funcall->to_string(npre);
  } else if (type == "intlit"){
    ans += intlit->to_string(npre);
  } else if (type == "floatlit"){
    ans += floatlit->to_string(npre);
  } else if (type == "boollit"){
    ans += boollit->to_string(npre);
  }
  return ans;
};

string FunCallASTnode::to_string(string pre) const {
  string ans;
  ans = pre + "FunCallASTnode:" + nl;
  string npre = pre + sp;
  ans += ident->to_string(npre);
  ans += npre + "Args:" + nl;
  string nnpre = npre + sp;
  for (auto &elem : arglist){
    ans += elem->to_string(nnpre);
  }
  return ans;
};

string BinOpASTnode::to_string(string pre) const {
  string ans = pre + "BinOpASTnode:" + nl;
  string npre = pre + sp;
  string nnpre = npre + sp;
  ans += npre + "Binary Operation: " + tok.lexeme + nl;
  ans += npre + "LHS:" + nl;
  ans += lhs->to_string(nnpre);
  ans += npre + "RHS:" + nl;
  ans += rhs->to_string(nnpre);
  return ans;
};

string UnOpASTnode::to_string(string pre) const {
  string ans = pre + "UnOpASTnode:" + nl;
  string npre = pre + sp;
  ans += npre + "Unary Operation: " + tok.lexeme + nl;
  ans += expr->to_string(npre);
  return ans;
}

string AssignASTnode::to_string(string pre) const {
  // std::cout << "In AssignASTnode\n";
  string ans = pre + "AssignASTnode:" + nl;
  string npre = pre + sp;
  ans += ident->to_string(npre);
  ans += rhs->to_string(npre);
  return ans;
}

string IfASTnode::to_string(string pre) const {
  string ans = pre + "IfASTnode:" + nl;
  string npre = pre + sp;
  string nnpre = npre + sp;
  ans += expr->to_string(npre);
  ans += block->to_string(npre);
  if (else_stmt){
    ans += npre + "ElseStmt:" + nl;
    ans += else_stmt->to_string(nnpre);
  }
  return ans;
};

string WhileASTnode::to_string(string pre) const {
  string ans = pre + "WhileASTnode:" + nl;
  string npre = pre + sp;
  ans += expr->to_string(npre);
  ans += stmt->to_string(npre);
  return ans;
}

string IntASTnode::to_string(string pre) const {
  string ans = pre + "IntASTnode: " + tok.lexeme + " <line: " + std::to_string(tok.lineNo) + ", column: " + std::to_string(tok.columnNo) + ">" + nl;
  return ans;
};

string FloatASTnode::to_string(string pre) const {
  string ans = pre + "FloatASTnode: " + tok.lexeme + " <line: " + std::to_string(tok.lineNo) + ", column: " + std::to_string(tok.columnNo) + ">" + nl;
  return ans;
};

string BoolASTnode::to_string(string pre) const {
  string ans = pre + "BoolASTnode: " + tok.lexeme + " <line: " + std::to_string(tok.lineNo) + ", column: " + std::to_string(tok.columnNo) + ">" + nl;
  return ans;
};

string IdentASTnode::to_string(string pre) const {
  string ans = pre + "IdentASTnode: " + tok.lexeme + " <line: " + std::to_string(tok.lineNo) + ", column: " + std::to_string(tok.columnNo) + ">" + nl;
  return ans;
};

string VarTypeASTnode::to_string(string pre) const {
  string ans = pre + "VarTypeASTnode: " + vartype + nl;
  return ans;
}

string VarDeclASTnode::to_string(string pre) const {
  string npre = pre + sp;
  string ans = "";
  ans += pre + "VarDeclASTnode: " + vartype->vartype + sp + ident->name;
  ans += + " <line: " + std::to_string(ident->tok.lineNo) + ", column: " + std::to_string(ident->tok.columnNo) + ">" + nl;
  auto x = ident->tok.lexeme;
  auto y = std::to_string(ident->tok.lineNo);
  return ans;
};

string ReturnASTnode::to_string(string pre) const {
  string ans = pre + "ReturnASTnode:" + nl;
  string npre = pre + sp;
  if (isVoid){
    ans += npre + "void" + nl;
  } else {
    ans += expr->to_string(npre);
  }
  return ans;
};

string ParamASTnode::to_string(string pre) const {
  string npre = pre + sp;
  string ans = pre + "ParamASTnode " + vartype->vartype + sp + ident->name;
      
  return ans;
};

string ParamListASTnode::to_string(string pre) const {
  string npre = pre + sp;
  string ans = pre + "ParamListASTnode" + nl;
  for (auto &p : paramlist){
    ans += pre + p->to_string(npre) + nl;
  }
  ans.pop_back();
  return ans;
};

string TypeSpecASTnode::to_string(string pre) const {
  string ans = pre + "TypeSpecASTnode: " + get_type() + nl;
  return ans;
}

string FunProtoASTnode::to_string(string pre) const {
  string npre = pre + sp;
  string ans = "";
  ans += pre + "FunProtoASTnode: ";
  ans += typespec->get_type() + sp + ident->name;
  ans += "(";
  bool commas = false;
  for (auto &elem : params->paramlist){
    ans += elem->vartype->vartype + ", ";
    commas = true;
  }
  if (commas){
    ans.pop_back();
    ans.pop_back();
  }
  ans += ")";
  ans += " <line: " + std::to_string(ident->tok.lineNo) + ", column: " + std::to_string(ident->tok.columnNo) + ">" + nl;
  return ans;
};

string FunBodyASTnode::to_string(string pre) const{
  string npre = pre + sp;
  string ans = "";
  ans += pre + "FunBodyASTnode:" + nl;
  ans += body->to_string(npre);
  return ans;
};

string FunDeclASTnode::to_string(string pre) const {
  string npre = pre + sp;
  string ans = "";
  ans += pre + "FunDeclASTnode:" + nl;
  ans += funproto->to_string(npre);
  ans += funbody->to_string(npre);
  return ans;
};

string ExternASTnode::to_string(string pre) const {
  string npre = pre + sp;
  string ans = "";
  ans += pre + "ExternASTnode: " + nl;
  ans += funproto->to_string(npre);
  return ans;
};

string DeclASTnode::to_string(string pre) const {
  string npre = pre;
  string ans = "";
  if (isVar){
    ans = vardecl->to_string(npre);
  } else {
    ans = fundecl->to_string(npre);
  }
  return ans;
};

string ProgramASTnode::to_string(string pre) const {
  string npre = pre + sp;
  string ans = pre + "ProgramASTnode:" + nl;
  for (auto &ext : externs){
    ans += ext->to_string(npre);
  }
  for (auto &dec : decls){
    ans += dec->to_string(npre);
  }

  ans.pop_back(); // remove last newline
  return ans;
};

Value* BinOpASTnode::codegen(){
  Value *val, *l, *r;
  if (binop==AND) { // this implements short circuiting
    Function *TheFunction = Builder->GetInsertBlock()->getParent();
    BasicBlock *original = Builder->GetInsertBlock();
    BasicBlock *checkrhs = BasicBlock::Create(*TheContext, "checkrhs", TheFunction);
    BasicBlock *end = BasicBlock::Create(*TheContext, "endshortcircuit", TheFunction);
    l = bool_cast(lhs->codegen());
    Builder->CreateCondBr(l, checkrhs, end);
    Builder->SetInsertPoint(checkrhs);
    r = bool_cast(rhs->codegen());
    Builder->CreateBr(end);
    Builder->SetInsertPoint(end);
    PHINode *phi = Builder->CreatePHI(tok_to_llvm_type(BOOL_TOK), 2, "mergeandshortcircuit");
    Value *llvmfalse = ConstantInt::get(tok_to_llvm_type(BOOL_TOK), 0);
    phi->addIncoming(llvmfalse, original);
    phi->addIncoming(r, checkrhs);
    return phi;
  } else if (binop==OR) { // this implements short circuiting
    Function *TheFunction = Builder->GetInsertBlock()->getParent();
    BasicBlock *original = Builder->GetInsertBlock();
    BasicBlock *checkrhs = BasicBlock::Create(*TheContext, "checkrhs", TheFunction);
    BasicBlock *end = BasicBlock::Create(*TheContext, "endshortcircuit", TheFunction);
    l = bool_cast(lhs->codegen());
    Builder->CreateCondBr(l, end, checkrhs);
    Builder->SetInsertPoint(checkrhs);
    r = bool_cast(rhs->codegen());
    Builder->CreateBr(end);
    Builder->SetInsertPoint(end);
    PHINode *phi = Builder->CreatePHI(tok_to_llvm_type(BOOL_TOK), 2, "mergeandshortcircuit");
    Value *llvmtrue = ConstantInt::get(tok_to_llvm_type(BOOL_TOK), 1);
    phi->addIncoming(llvmtrue, original);
    phi->addIncoming(r, checkrhs);
    return phi;

  } else {
  l = lhs->codegen();
  r = rhs->codegen();
  Type* widest = widest_type(l, r);
  Value *lcast = widening_cast_or_err(l, widest, tok);
  Value *rcast = widening_cast_or_err(r, widest, tok);
  Type* floattype = tok_to_llvm_type(FLOAT_TOK);
  Type* inttype = tok_to_llvm_type(INT_TOK);
  switch(binop){
  case AND: // normal non shortcircuiting
    val = Builder->CreateAnd(bool_cast(l), bool_cast(r), "and");
    break;
  case OR: // normal non shortcircuiting 
    val = Builder->CreateOr(bool_cast(l), bool_cast(r), "or");
    break;
  case PLUS:
    if (widest==floattype){
      val = Builder->CreateFAdd(lcast, rcast, "float_add");
    } else {
      val = Builder->CreateAdd(int_cast(l), int_cast(r), "int_add");
    }
    break;
  case MINUS:
    if (widest==floattype){
      val = Builder->CreateFSub(lcast, rcast, "float_sub");
    } else {
      // cast bool to int if needed
      val = Builder->CreateSub(int_cast(l), int_cast(r), "int_sub");
    }
    break;
  case ASTERIX:
    if (widest==floattype){
      val = Builder->CreateFMul(lcast, rcast, "float_mul");
    } else if (widest==inttype){
      val = Builder->CreateMul(int_cast(l), int_cast(r), "int_mul");
    } else {
      val = Builder->CreateMul(l, r, "bool_mul");
    }
    break;
  case DIV:
    if (widest==floattype){
        val = Builder->CreateFDiv(lcast, rcast, "float_div");
      } else if (widest==inttype){
        val = Builder->CreateSDiv(int_cast(l), int_cast(r), "int_sdiv");
      } else {
        val = Builder->CreateUDiv(l, r, "bool_udiv");
      }
    break;
  case MOD:
    if (widest==floattype){
        throw CompileError(tok, "Neither operand of MOD can be a float");
      } else if (widest==inttype){
        val = Builder->CreateSRem(int_cast(l), int_cast(r), "int_srem");
      } else {
        val = Builder->CreateURem(l, r, "bool_urem");
      }
    break;
  case EQ:
    if (widest==floattype){
      val = Builder->CreateFCmpOEQ(lcast, rcast, "float_oeq");
    } else if (widest==inttype){
      val = Builder->CreateICmpEQ(int_cast(l), int_cast(r), "int_eq");
    } else {
      val = Builder->CreateICmpEQ(l, r, "bool_eq");
    }
    break;
  case NE:
    if (widest==floattype){
      // nan is not equal to anything (including itself)
      val = Builder->CreateFCmpUNE(lcast, rcast, "float_une");
    } else if (widest==inttype){
      val = Builder->CreateICmpNE(int_cast(l), int_cast(r), "int_ne");
    } else {
      val = Builder->CreateICmpNE(l, r, "bool_ne");
    }
    break;
  case LE:
    if (widest==floattype){
      // nan is not le anything (including itself)
      val = Builder->CreateFCmpOLE(lcast, rcast, "float_ole");
    } else if (widest==inttype){
      val = Builder->CreateICmpSLE(int_cast(l), int_cast(r), "int_sle");
    } else {
      val = Builder->CreateICmpULE(l, r, "bool_ule");
    }
    break;
  case LT:
    if (widest==floattype){
      // nan is not lt anything (including itself)
      val = Builder->CreateFCmpOLT(lcast, rcast, "float_olt");
    } else if (widest==inttype){
      val = Builder->CreateICmpSLT(int_cast(l), int_cast(r), "int_slt");
    } else {
      val = Builder->CreateICmpULT(l, r, "bool_ult");
    }
    break;
  case GE:
    if (widest==floattype){
      // nan is not ge anything (including itself)
      val = Builder->CreateFCmpOGE(lcast, rcast, "float_oge");
    } else if (widest==inttype){
      val = Builder->CreateICmpSGE(int_cast(l), int_cast(r), "int_sge");
    } else {
      val = Builder->CreateICmpUGE(l, r, "bool_uge");
    }
    break;
  case GT:
    if (widest==floattype){
      // nan is not gt anything (including itself)
      val = Builder->CreateFCmpOGT(lcast, rcast, "float_ogt");
    } else if (widest==inttype){
      val = Builder->CreateICmpSGT(int_cast(l), int_cast(r), "int_sgt");
    } else {
      val = Builder->CreateICmpUGT(l, r, "bool_ugt");
    }
    break;
  default:
    throw CompileError(tok, "This is not a valid binary operation token");
  }
  }
  return val;
};

Value* UnOpASTnode::codegen(){
  Value *val = expr->codegen();
  Type *t = val->getType();
  switch(unop){
  case MINUS:
    Value *ans;
    if (t == tok_to_llvm_type(BOOL_TOK)) {
      ans = Builder->CreateNeg(int_cast(val));
    } else if (t == tok_to_llvm_type(INT_TOK)) {
      ans = Builder->CreateNeg(val);
    } else {
      ans = Builder->CreateFNeg(val);
    }
    return ans;
  case PLUS:
    return val;
  case NOT:
    return Builder->CreateICmpNE(bool_cast(val), ConstantInt::get(tok_to_llvm_type(BOOL_TOK), 1), "not");
  default:
    throw CompileError(tok, "Token is not a valid unary operator");
  }
};

Value* IntASTnode::codegen(){
  return ConstantInt::get(tok_to_llvm_type(INT_TOK), val);
};

Value* FloatASTnode::codegen(){
  return ConstantFP::get(tok_to_llvm_type(FLOAT_TOK), val);
};

Value* BoolASTnode::codegen(){
  int v = val ? 1 : 0;
  return ConstantInt::get(tok_to_llvm_type(BOOL_TOK), v);
};

Value* IdentASTnode::codegen(){
  AllocaInst* alloca = find_local(name);
  if (!alloca){ // not found in any local variable symbol table
    if (GlobalNamedValues.find(name) == GlobalNamedValues.end()){ // not found in global variable symbol table
      throw CompileError(tok, "variable needs to be declared before being used");
    } else { // we are referring to a already declared global variable
      GlobalVariable* g = GlobalNamedValues.at(name);
      if (g){ // this is not a nullptr so global variable has been defined once before
        Value* v = Builder->CreateLoad(g->getValueType(), g, name);
        return v;
      } else { // nullptr which means global variable has been declared but not defined
          // we will return the null value of the global
          // throw CompileError(tok, "global variable has been declared but not defined yet");
          GlobalVariable* g = TheModule->getGlobalVariable(name);
          Value* v = Builder->CreateLoad(g->getValueType(), g, name);
          return v;
      }
    }
  } else { // load the existing local variable
    Value* v = Builder->CreateLoad(alloca->getAllocatedType(), alloca, name);
    return v;
  }
  
};

Value* FunCallASTnode::codegen(){
  // Look up the name in the global module table.
  Function *CalleeF = TheModule->getFunction(ident->name);
  if (!CalleeF){
    throw CompileError(ident->tok, "Unknown function referenced");
  }
  // If argument mismatch error.
  if (CalleeF->arg_size() != arglist.size()){
    throw CompileError(ident->tok, "Incorrect # arguments passed");
  }

  
  std::vector<Value *> argvals;
  for (int i = 0; i<arglist.size(); i++) {
    Value *thisarg = arglist[i]->codegen();
    TOKEN tok = arglist[i]->get_first_tok();
    Value *casted = widening_cast_or_err(thisarg, CalleeF->getArg(i)->getType(), tok);
    argvals.push_back(casted);
    if (!argvals.back()){
      throw std::runtime_error("argvals.back() is null in FunCallASTnode");
    }
  }
  CallInst *callinst = Builder->CreateCall(CalleeF, argvals, "calltmp");
  return callinst;

};

Type* VarTypeASTnode::codegen(){
  if (vartype == "bool"){
    return tok_to_llvm_type(BOOL_TOK);
  } else if (vartype == "float"){
    return tok_to_llvm_type(FLOAT_TOK);
  } else {
    return tok_to_llvm_type(INT_TOK);
  }
};

Value* VarDeclASTnode::codegen(){
  if (isGlobal){
    if (GlobalNamedValues.find(ident->name) != GlobalNamedValues.end()){
      throw CompileError(ident->tok, "Global variable cannot be declared more than once");
    } 
    Type *t = vartype->codegen();
    GlobalVariable *g = new GlobalVariable(*TheModule, t, false, GlobalValue::CommonLinkage, Constant::getNullValue(t), ident->name);
    GlobalNamedValues.insert({ident->name, nullptr});
    return g;
  } else {
    throw std::runtime_error("local decl not implemented here, it is implemented in block");
  }
};

Value* AssignASTnode::codegen(){
  Value* rhsvalue = rhs->codegen();
  AllocaInst* alloca = find_local(ident->name);
  if (!alloca){ // not found in any local variable symbol table
    if (GlobalNamedValues.find(ident->name) == GlobalNamedValues.end()){ // not found in global variable symbol table
      throw CompileError(ident->tok, "variable needs to be declared before being defined");
    } else { // we are referring to a already declared global variable
      GlobalVariable* g = GlobalNamedValues.at(ident->name);

////////// comment out exactly one of the below two

/////////////////////////begining of global variables can only be defined once 
      // if (g){ // this is not a nullptr so global variable has been defined once beforee
      //   throw CompileError(ident->tok, "global variable cannot be defined more than once");
      // } else { // this is the first definition of the global variable
      //   GlobalVariable *gnonnull = TheModule->getGlobalVariable(ident->name);
      //   Type *lhstype = gnonnull->getValueType();
      //   Value *tostore = force_cast(rhsvalue, lhstype, rhs->get_first_tok());
      //   Builder->CreateStore(tostore, gnonnull);
      //   GlobalNamedValues.at(ident->name) = gnonnull;
      //   return rhsvalue;
      // }
////////////////////////////////////////////// end of global variables can only be defined once

///////////////////////////////// beginning of global variables can be defined many times

        GlobalVariable *gnonnull = TheModule->getGlobalVariable(ident->name);
        Type *lhstype = gnonnull->getValueType();
        Value *tostore = force_cast(rhsvalue, lhstype, rhs->get_first_tok());
        Builder->CreateStore(tostore, gnonnull);
        GlobalNamedValues.at(ident->name) = gnonnull;
        return rhsvalue;

////////////////////////////////////// end of global variables can be defined many times.


    }
    
  } else { // we are referring to already declared local variable
    Type *lhstype = alloca->getAllocatedType();
    Value *tostore = force_cast(rhsvalue, lhstype, rhs->get_first_tok());
    Builder->CreateStore(tostore, alloca);
    return rhsvalue;
  }

  // below is to get rid of compiler warnings and debugging (but has not been needed)
  throw CompileError(ident->tok, "We should not be reaching this part of the function");
};

TOKEN ExprASTnode::get_first_tok() const{
    TOKEN tok;
    if (type == "assign"){
      tok =  assign->ident->tok;
    } else if (type == "binop"){
      tok =  binop->lhs->get_first_tok();
    } else if (type == "unop"){
      tok =  unop->tok;
    } else if (type == "ident"){
      tok =  ident->tok;
    } else if (type == "funcall"){
      tok =  funcall->ident->tok;
    } else if (type == "intlit"){
      tok =  intlit->tok;
    } else if (type == "floatlit"){
      tok =  floatlit->tok;
    } else if (type == "boollit"){
      tok =  boollit->tok;
    };
    return tok;
  };

Value* ExprASTnode::codegen(){
  Value* val;
  if (type == "assign"){
    val = assign->codegen();
  } else if (type == "binop"){
    val = binop->codegen();
  } else if (type == "unop"){
    val = unop->codegen();
  } else if (type == "ident"){
    val = ident->codegen();
  } else if (type == "funcall"){
    val = funcall->codegen();
  } else if (type == "intlit"){
    val = intlit->codegen();
  } else if (type == "floatlit"){
    val = floatlit->codegen();
  } else if (type == "boollit"){
    val = boollit->codegen();
  };
  if (!val){
    throw std::runtime_error("in ExprASTnode, val is a nullptr which it should not be");
  }
  return val;
};

Value* WhileASTnode::codegen(){
  Function *TheFunction = Builder->GetInsertBlock()->getParent();
  BasicBlock *whilecond_ = BasicBlock::Create(*TheContext, "whilecondition", TheFunction);
  BasicBlock *whilebody_ = BasicBlock::Create(*TheContext, "whilebody", TheFunction);
  BasicBlock *end_ = BasicBlock::Create(*TheContext, "endwhile", TheFunction);
  
  Builder->CreateBr(whilecond_);
  Builder->SetInsertPoint(whilecond_);
  Value *cond = expr->codegen();
  Value *comp = Builder->CreateICmpNE(bool_cast(cond), ConstantInt::get(tok_to_llvm_type(BOOL_TOK), 0), "whilecond");
  Builder->CreateCondBr(comp, whilebody_, end_);
  Builder->SetInsertPoint(whilebody_);
  stmt->codegen();
  Builder->CreateBr(whilecond_);
  Builder->SetInsertPoint(end_);
  return llvmnull;
};

Value* ReturnASTnode::codegen(){
  if (isVoid){
    return Builder->CreateRetVoid();
  } else {
    // type check the return value.
    Type* rettype = Builder->getCurrentFunctionReturnType();
    Value* retval = expr->codegen();
    Value* casted = widening_cast_or_err(retval, rettype, expr->get_first_tok());
    return Builder->CreateRet(casted);
  }
};

Value* IfASTnode::codegen(){
  Function *TheFunction = Builder->GetInsertBlock()->getParent();
  BasicBlock *then_ = BasicBlock::Create(*TheContext, "then", TheFunction);
  BasicBlock *else_;
  BasicBlock *end_ = BasicBlock::Create(*TheContext, "endif");
  
  Value *cond = expr->codegen();
  Value *comp = Builder->CreateICmpNE(bool_cast(cond), ConstantInt::get(tok_to_llvm_type(BOOL_TOK), 0), "ifcond");

  Value *llvmfalse = ConstantInt::get(*TheContext, APInt(32, 0, true));
  
  if (else_stmt){
    else_ = BasicBlock::Create(*TheContext, "else");
    Builder->CreateCondBr(comp, then_, else_);
    Builder->SetInsertPoint(then_);
    block->codegen();
    Builder->CreateBr(end_);
    TheFunction->insert(TheFunction->end(), else_);
    Builder->SetInsertPoint(else_);
    else_stmt->codegen();
    Builder->CreateBr(end_);
    TheFunction->insert(TheFunction->end(), end_);
    Builder->SetInsertPoint(end_);
  } else {
    Builder->CreateCondBr(comp, then_, end_);
    Builder->SetInsertPoint(then_);
    block->codegen();
    Builder->CreateBr(end_);
    TheFunction->insert(TheFunction->end(), end_);
    Builder->SetInsertPoint(end_);
  }
  
  return llvmnull;
};

Value* StmtASTnode::codegen(){
  if (whichtype=="expr_stmt"){
    expr_stmt->codegen();
  } else if (whichtype=="block"){
    block->codegen();
  } else if (whichtype=="if_stmt"){
    if_stmt->codegen();
  } else if (whichtype=="while_stmt"){
    while_stmt->codegen();
  } else if (whichtype=="return_stmt"){
    return_stmt->codegen();
  }
  return llvmnull;
};

Value* BlockASTnode::codegen(){
  //We assume that the Builder.SetInsertPoint is already done.
  
  // we need to create a new NamedValues table for each ldecl and
  // push back onto NamedValuesVector
  auto NamedValuesPtr = make_unique<map<string, AllocaInst*>>();
  // bool createdNewMap = false;
  for (auto &ldecl : localdecls){
    if (NamedValuesPtr->find(ldecl->vardecl->ident->name) != NamedValuesPtr->end()){
      throw CompileError(ldecl->vardecl->ident->tok, "Variable cannot be declared more than once in the given scope");
    }
    Type *t = ldecl->vardecl->vartype->getType();
    AllocaInst *alloca = Builder->CreateAlloca(t, 0, ldecl->vardecl->ident->name);
    NamedValuesPtr->insert({ldecl->vardecl->ident->name, alloca});
  }
  NamedValuesVector.push_back(std::move(NamedValuesPtr));
  
  for (auto &stmt : stmtlist){
    stmt->codegen();
    if (stmt->whichtype == "return_stmt"){
      break; // we do not codegen after the last return
    }
  }

  NamedValuesVector.pop_back(); // delete the top most symbol table after we exit the block
  return llvmnull;
};

Type *ParamASTnode::codegen(){
  return vartype->codegen();
};

vector<Type *> ParamListASTnode::codegen(){
  vector<Type *> ans;
  vector<string> argnames;
  for (auto &param : paramlist){
    if (find(argnames.begin(), argnames.end(), param->ident->name) != argnames.end()){
      throw CompileError(param->ident->tok, "Cannot have repeated argument names in function prototype");
    }
    ans.push_back(param->codegen());
  }
  return ans;
};

Type* TypeSpecASTnode::codegen(){
  if (isVoid){
    return Type::getVoidTy(*TheContext);
  } else {
    return vartype->codegen();
  }
};

Function* FunProtoASTnode::codegen(){
  vector<Type *> argtypes = params->codegen();
  Type *outputtype = typespec->codegen();
  FunctionType *ft = FunctionType::get(outputtype, argtypes, false);
  Function *F = Function::Create(ft, Function::ExternalLinkage, ident->name, TheModule.get());
  // Set names for all arguments.
  unsigned idx = 0;
  for (auto &prm : F->args()){
    prm.setName(params->paramlist.at(idx)->ident->name);
    idx++;
  }
  return F;
};

Value* FunBodyASTnode::codegen(){
  return body->codegen();
}

Value* FunDeclASTnode::codegen(){
  Function *TheFunction = TheModule->getFunction(funproto->ident->name);
  if (TheFunction){
    throw CompileError(funproto->ident->tok, "Function already declared, cannot be declared twice");
  }
  if (!TheFunction){
    TheFunction = funproto->codegen();
  }
  // if (!TheFunction){
  //   throw CompileError("in FunDeclASTnode idk why this is returning null");
  //   // return nullptr;
  // }

  BasicBlock *BB = BasicBlock::Create(*TheContext, "entry", TheFunction);
  Builder->SetInsertPoint(BB);

  NamedValuesVector.clear();
  auto NamedValuesPtr = make_unique<map<string, AllocaInst*>>();

  AllocaInst *alloca;
  for (auto &arg : TheFunction->args()) {
    Type *argtype = arg.getType();
    alloca = CreateEntryBlockAlloca(TheFunction, string(arg.getName()), argtype);
    Builder->CreateStore(&arg, alloca);
    NamedValuesPtr->insert({string(arg.getName()), alloca});
  }

  NamedValuesVector.push_back(std::move(NamedValuesPtr));

  funbody->codegen();

  // below is a bit of a hack so that we dont have empty basic blocks
  for (BasicBlock &bb: *TheFunction){
    if (bb.size() == 0){
      Type *returntype = TheFunction->getReturnType();
      Value* funczero = Constant::getNullValue(returntype);
      Builder->SetInsertPoint(&bb);
      Builder->CreateRet(funczero);
    }
  }
  
  verifyFunction(*TheFunction);
  return TheFunction;
};

Value* ExternASTnode::codegen(){
  return funproto->codegen();;
};

Value* DeclASTnode::codegen(){
  if (isVar){
    return vardecl->codegen();
  } else {
    return fundecl->codegen();
  }
};

Value* ProgramASTnode::codegen() {
  for (auto &elem : externs){
    elem->codegen();
  }
  for (auto &elem : decls){
    elem->codegen();
  }
  return llvmnull; 
};

AllocaInst* CreateEntryBlockAlloca(Function *TheFunction, const std::string &VarName, Type *argtype) {
  IRBuilder<> TmpB(&TheFunction->getEntryBlock(), TheFunction->getEntryBlock().begin());
  return TmpB.CreateAlloca(argtype, 0, VarName);
}

AllocaInst* find_local(string funcname){
  // start looking from the end of NamedValuesVector (most recent block)
  for (int i = NamedValuesVector.size()-1; i>=0; i--){
    if (NamedValuesVector[i]->find(funcname) != NamedValuesVector[i]->end()){
      return NamedValuesVector[i]->at(funcname);
    }
  }
  return nullptr;
};

string typetostring(Type* t){
  std::string type_str;
  llvm::raw_string_ostream rso(type_str);
  t->print(rso);
  return rso.str();
}

Type *tok_to_llvm_type(TOKEN_TYPE tok){
  switch(tok){
  case BOOL_TOK:
    return Type::getInt1Ty(*TheContext);
  case INT_TOK:
    return Type::getInt32Ty(*TheContext);
  case FLOAT_TOK:
    return Type::getFloatTy(*TheContext);
  default:
    throw CompileError("This TOKEN_TYPE is not a valid expression type: " + std::to_string((int) tok));
  }
}

Value *widening_cast_or_err(Value* inputval, Type* goaltype, TOKEN tok){
  Type * booltype = tok_to_llvm_type(BOOL_TOK);
  Type * inttype = tok_to_llvm_type(INT_TOK);
  Type * floattype = tok_to_llvm_type(FLOAT_TOK);

  Type *inputtype = inputval->getType();
  string errmsg = "cannot perform implicit narrowing cast when casting from " + typetostring(inputtype) + " to " + typetostring(goaltype) + "\n";

  // if identical and not void then no cast needed
  if (inputtype == goaltype && !inputtype->isVoidTy()){
    return inputval;
  }

  // Handling if inputtype or goaltype is void
  if (inputtype->isVoidTy()){
    errmsg += "Cannot cast from void\n";
  }
  if (goaltype->isVoidTy()){
    errmsg += "Cannot cast to void\n";
  }

  if (inputtype==booltype && goaltype==inttype) {
    // bool to int
    return Builder->CreateIntCast(inputval, goaltype, false, "bool_int_cast");
  } else if (inputtype==booltype && goaltype==floattype){
    // bool to float
    Value* booltoint = Builder->CreateIntCast(inputval, goaltype, false, "bool_int_cast");
    return Builder->CreateSIToFP(booltoint, tok_to_llvm_type(TOKEN_TYPE::FLOAT_TOK), "bool_float_cast");
  } else if (inputtype==inttype && goaltype==floattype){
    // int to float
    return Builder->CreateSIToFP(inputval, tok_to_llvm_type(TOKEN_TYPE::FLOAT_TOK), "int_float_cast");
  }
  
  throw CompileError(tok, errmsg);
};

Value *bool_cast(Value *val){
  Type *inputtype = val->getType();
  // std::cout << typetostring(inputtype) << "\n";
  if (inputtype==tok_to_llvm_type(BOOL_TOK)){
    return val;
  } else if (inputtype==tok_to_llvm_type(INT_TOK)){
    Value *cmpresult = Builder->CreateICmpNE(val, ConstantInt::get(tok_to_llvm_type(INT_TOK), 0), "int_to_bool_one");
    return Builder->CreateZExt(cmpresult, tok_to_llvm_type(BOOL_TOK), "int_to_bool_two");
  } else if (inputtype==tok_to_llvm_type(FLOAT_TOK)){
    Value *cmpresult = Builder->CreateFCmpUNE(val, ConstantFP::get(tok_to_llvm_type(FLOAT_TOK), 0.0), "float_to_bool_one");
    return Builder->CreateZExt(cmpresult, tok_to_llvm_type(BOOL_TOK), "float_to_bool_two");
  } else {
    throw std::runtime_error("bool_cast exception because input value type is not a supported type");
  }
}

Value *int_cast(Value* val){
  Type *inputtype = val->getType();
  if (inputtype==tok_to_llvm_type(BOOL_TOK)){
    return Builder->CreateIntCast(val, tok_to_llvm_type(INT_TOK), false, "bool_int_cast");
  } else if (inputtype==tok_to_llvm_type(INT_TOK)){
    return val;
  } else if (inputtype==tok_to_llvm_type(FLOAT_TOK)){
    return Builder->CreateFPToSI(val, tok_to_llvm_type(INT_TOK), "float_int_cast");
    throw std::runtime_error("bool_cast exception because input value type is not a supported type");
  } else {
    throw std::runtime_error("bool_cast exception because input value type is not a supported type");
  }
}

Value *force_cast(Value* inputval, Type* goaltype, TOKEN tok){
  if (goaltype==tok_to_llvm_type(FLOAT_TOK)){
    return widening_cast_or_err(inputval, goaltype, tok);
  } else if (goaltype==tok_to_llvm_type(INT_TOK)){
    return int_cast(inputval);
  } else if (goaltype==tok_to_llvm_type(BOOL_TOK)){
    return bool_cast(inputval);
  } else {
    string errmsg = "cannot perform implicit narrowing cast when casting from " + typetostring(inputval->getType()) + " to " + typetostring(goaltype) + "\n";
    throw CompileError(tok, errmsg);
  }

}

Type* widest_type(Value* v1, Value* v2){
  Type* t1 = v1->getType();
  Type* t2 = v2->getType();
  if (t1 == tok_to_llvm_type(FLOAT_TOK) || t2 == tok_to_llvm_type(FLOAT_TOK)){
    return tok_to_llvm_type(FLOAT_TOK);
  } else if (t1 == tok_to_llvm_type(INT_TOK) || t2 == tok_to_llvm_type(INT_TOK)){
    return tok_to_llvm_type(INT_TOK);
  } else {
    return tok_to_llvm_type(BOOL_TOK);
  }
}