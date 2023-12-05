#pragma once
#include "lexer.h"
#include "llvm/IR/Value.h"
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
#include <memory>
#include <utility>
#include <iostream>
#include <map>
#include <exception>

using namespace llvm;
using std::unique_ptr;
using std::vector;
using std::string;

extern string br;
extern string sp;
extern string nl;

extern unique_ptr<LLVMContext> TheContext;
extern unique_ptr<llvm::Module> TheModule;
extern unique_ptr<llvm::IRBuilder<>> Builder;
// extern std::map<std::string, AllocaInst*> NamedValues; // local var table(s)
extern vector<unique_ptr<std::map<string, AllocaInst*>>> NamedValuesVector;
extern std::map<std::string, GlobalVariable*> GlobalNamedValues; //global var table
//===----------------------------------------------------------------------===//
// AST nodes
//===----------------------------------------------------------------------===//

class BlockASTnode;
class StmtASTnode;
class ExprASTnode;
class AssignASTnode;
class DeclASTnode;

// ASTnode - Base class for all AST nodes.
class ASTnode {
public:
  virtual ~ASTnode() {};
  // virtual llvm::Value *codegen();
  // virtual llvm::Value *codegen() {return nullptr;};
  virtual string to_string(string pre) const {return pre + "not done" + nl;};
};

class BinOpASTnode : public ASTnode{
public:
  TOKEN_TYPE binop;
  TOKEN tok;
  unique_ptr<ExprASTnode> lhs;
  unique_ptr<ExprASTnode> rhs;
  BinOpASTnode(TOKEN_TYPE o,
               unique_ptr<ExprASTnode> l,
               unique_ptr<ExprASTnode> r,
               TOKEN t
               ) : binop(o), tok(t), lhs(std::move(l)), rhs(std::move(r)){};
  virtual llvm::Value *codegen();
  virtual string to_string(string pre) const override;
};

class UnOpASTnode : public ASTnode{
public:
  TOKEN_TYPE unop;
  TOKEN tok;
  unique_ptr<ExprASTnode> expr;
  UnOpASTnode(TOKEN_TYPE o,
              unique_ptr<ExprASTnode> e,
              TOKEN t
              ) : unop(o), tok(t), expr(std::move(e)){};
  virtual llvm::Value *codegen();
  virtual string to_string(string pre) const override;
};

class IntASTnode : public ASTnode {
public:
  /// IntASTnode - Class for integer literals like 1, 2, 10,
  int val;
  TOKEN tok;
  string name;
  IntASTnode(TOKEN t) : val(std::stoi(t.lexeme)), tok(t){};
  virtual llvm::Value *codegen();
  virtual string to_string(string pre) const override;
};

class FloatASTnode : public ASTnode{
public:
  float val;
  TOKEN tok;
  string name;
  FloatASTnode(TOKEN t) : val(std::stof(t.lexeme)), tok(t){};
  virtual llvm::Value *codegen();
  virtual string to_string(string pre) const override;
};

class BoolASTnode : public ASTnode{
public:
  bool val;
  TOKEN tok;
  string name;
  BoolASTnode(TOKEN t) : val(noerr_str_to_bool(t.lexeme)), tok(t) {};
  virtual llvm::Value *codegen();
  virtual string to_string(string pre) const override;
};

class IdentASTnode : public ASTnode{
public:
  TOKEN tok;
  string name;
  IdentASTnode(TOKEN t) : tok(t), name(t.lexeme){};
  virtual llvm::Value *codegen();
  virtual string to_string(string pre) const override;
};

class FunCallASTnode : public ASTnode{
public:
  unique_ptr<IdentASTnode> ident;
  vector<unique_ptr<ExprASTnode>> arglist;
  FunCallASTnode(unique_ptr<IdentASTnode> i,
                 vector<unique_ptr<ExprASTnode>> &al){
    ident = std::move(i);
    for (auto &elem : al){
      arglist.push_back(std::move(elem));
    }
  }

  virtual llvm::Value *codegen();
  virtual string to_string(string pre) const override;
};

class VarTypeASTnode : public ASTnode{
public:
  string vartype; // "int", "float", "bool"
  VarTypeASTnode(string vt) : vartype(vt) {};
  virtual Type *codegen();
  virtual string to_string(string pre) const override;
  Type *getType(){
    if (vartype=="int"){
      return Type::getInt32Ty(*TheContext);
    } else if (vartype=="float"){
      return Type::getFloatTy(*TheContext);
    } else if (vartype=="bool"){
      return Type::getInt1Ty(*TheContext);
    } else {
      throw std::runtime_error("VarTypeASTnode has invalid vartype");
    }
  }
};

class VarDeclASTnode : public ASTnode{
public:
  bool isGlobal = true;
  unique_ptr<VarTypeASTnode> vartype;
  unique_ptr<IdentASTnode> ident;
  // string name;
  VarDeclASTnode(unique_ptr<VarTypeASTnode> vt,
                 unique_ptr<IdentASTnode> id,
                 bool isGlbl = true
                 ) : isGlobal(isGlbl), vartype(std::move(vt)), ident(std::move(id)){};
  virtual llvm::Value *codegen();
  virtual string to_string(string pre) const override;
};

class ExprASTnode : public ASTnode{
public:
  string type; // assign, binop, unop, ident, funcall, intlit, floatlit, boollit
  unique_ptr<AssignASTnode> assign;
  unique_ptr<BinOpASTnode> binop;
  unique_ptr<UnOpASTnode> unop;
  unique_ptr<IdentASTnode> ident;
  unique_ptr<FunCallASTnode> funcall;
  unique_ptr<IntASTnode> intlit;
  unique_ptr<FloatASTnode> floatlit;
  unique_ptr<BoolASTnode> boollit;
  ExprASTnode(unique_ptr<AssignASTnode> a) : type("assign"), assign(std::move(a)){};
  ExprASTnode(unique_ptr<BinOpASTnode> b) : type("binop"), binop(std::move(b)){};
  ExprASTnode(unique_ptr<UnOpASTnode> u) : type("unop"), unop(std::move(u)){};
  ExprASTnode(unique_ptr<IdentASTnode> i) : type("ident"), ident(std::move(i)){};
  ExprASTnode(unique_ptr<FunCallASTnode> f) : type("funcall"), funcall(std::move(f)){};
  ExprASTnode(unique_ptr<IntASTnode> in) : type("intlit"), intlit(std::move(in)){};
  ExprASTnode(unique_ptr<FloatASTnode> fl) : type("floatlit"), floatlit(std::move(fl)){};
  ExprASTnode(unique_ptr<BoolASTnode> bo) : type("boollit"), boollit(std::move(bo)){};
  virtual llvm::Value *codegen();
  virtual string to_string(string pre) const override;
  TOKEN get_first_tok() const;
};

class AssignASTnode : public ASTnode{
public:
  unique_ptr<IdentASTnode> ident;
  unique_ptr<ExprASTnode> rhs;
  AssignASTnode(unique_ptr<IdentASTnode> i,
                unique_ptr<ExprASTnode> r
                ) : ident(std::move(i)) , rhs(std::move(r)){};
  virtual llvm::Value *codegen();;
  virtual string to_string(string pre) const override;
};

class WhileASTnode : public ASTnode{
public:
  unique_ptr<ExprASTnode> expr;
  unique_ptr<StmtASTnode> stmt;
  WhileASTnode(unique_ptr<ExprASTnode> e,
               unique_ptr<StmtASTnode> s) : expr(std::move(e)) , stmt(std::move(s)){}
  virtual llvm::Value *codegen();
  virtual string to_string(string pre) const override;
};

class ReturnASTnode : public ASTnode{
public:
  bool isVoid;
  unique_ptr<ExprASTnode> expr;
  ReturnASTnode(unique_ptr<ExprASTnode> e) : isVoid(false), expr(std::move(e)){};
  ReturnASTnode() : isVoid(true){};
  virtual llvm::Value *codegen();
  virtual string to_string(string pre) const override;
};

class IfASTnode : public ASTnode{
public:
  unique_ptr<ExprASTnode> expr;
  unique_ptr<BlockASTnode> block;
  unique_ptr<BlockASTnode> else_stmt; //nullptr if no else
  IfASTnode(unique_ptr<ExprASTnode> e,
            unique_ptr<BlockASTnode> b,
            unique_ptr<BlockASTnode> e_stmt
            ) : expr(std::move(e)) , block(std::move(b)), else_stmt(std::move(e_stmt)){};
  virtual llvm::Value *codegen();
  virtual string to_string(string pre) const override;
};

class StmtASTnode : public ASTnode{
public:
  string whichtype; // "expr_stmt" "block" "if_stmt" "while_stmt" "return_stmt"
  unique_ptr<ExprASTnode> expr_stmt;
  unique_ptr<BlockASTnode> block;
  unique_ptr<IfASTnode> if_stmt;
  unique_ptr<WhileASTnode> while_stmt;
  unique_ptr<ReturnASTnode> return_stmt;
  StmtASTnode(unique_ptr<ExprASTnode> e) : whichtype("expr_stmt"), expr_stmt(std::move(e)) {};
  StmtASTnode(unique_ptr<BlockASTnode> b) : whichtype("block"), block(std::move(b)) {}; 
  StmtASTnode(unique_ptr<IfASTnode> i) : whichtype("if_stmt"), if_stmt(std::move(i)) {}; 
  StmtASTnode(unique_ptr<WhileASTnode> w) : whichtype("while_stmt"), while_stmt(std::move(w)) {}; 
  StmtASTnode(unique_ptr<ReturnASTnode> r) : whichtype("return_stmt"), return_stmt(std::move(r)) {};  
  virtual llvm::Value *codegen();
  virtual string to_string(string pre) const override;
};

class BlockASTnode : public ASTnode{
public:
  vector<unique_ptr<DeclASTnode>> localdecls;
  vector<unique_ptr<StmtASTnode>> stmtlist;

  BlockASTnode(vector<unique_ptr<DeclASTnode>> &ld,
               vector<unique_ptr<StmtASTnode>> &sl){
    for (auto &elem : ld){
      localdecls.push_back(std::move(elem));
    }
    for (auto &elem : sl){
      stmtlist.push_back(std::move(elem));
    }
  }
  
  virtual llvm::Value *codegen();
  virtual string to_string(string pre) const override;
};

class ParamASTnode : public ASTnode{
public:
  unique_ptr<VarTypeASTnode> vartype;
  unique_ptr<IdentASTnode> ident;
  ParamASTnode(unique_ptr<VarTypeASTnode> vt,
              unique_ptr<IdentASTnode> id
              ) : vartype(std::move(vt)), ident(std::move(id)){};
  virtual Type *codegen();
  virtual string to_string(string pre) const override;
};

class ParamListASTnode : public ASTnode{
public:
  vector<unique_ptr<ParamASTnode>> paramlist;
  ParamListASTnode(vector<unique_ptr<ParamASTnode>> &pl){
    for(int i=0; i<pl.size(); i++){
      paramlist.push_back(std::move(pl.at(i)));
    }
  };
  virtual vector<Type *> codegen();
  virtual string to_string(string pre) const override;
};

class TypeSpecASTnode : public ASTnode{
public:
  bool isVoid = false;
  unique_ptr<VarTypeASTnode> vartype;
  TypeSpecASTnode(string v) : isVoid(true){};
  TypeSpecASTnode(unique_ptr<VarTypeASTnode> vt) : vartype(std::move(vt)){};
  virtual Type *codegen();
  virtual string to_string(string pre) const override;
  string get_type() const{
    string ans;
    if (isVoid){
      ans = "void";
    } else {
      ans = vartype->vartype;
    }
    return ans;
  }
};

class FunProtoASTnode : public ASTnode{
public:
  unique_ptr<TypeSpecASTnode> typespec;
  unique_ptr<IdentASTnode> ident;
  unique_ptr<ParamListASTnode> params;
  FunProtoASTnode(unique_ptr<TypeSpecASTnode> ts,
                unique_ptr<IdentASTnode> id,
                unique_ptr<ParamListASTnode> ps
                ) : typespec(std::move(ts)), ident(std::move(id)), params(std::move(ps)){};
  virtual Function *codegen();
  virtual string to_string(string pre) const override;
};

class FunBodyASTnode : public ASTnode{
public:
  unique_ptr<BlockASTnode> body;
  FunBodyASTnode(unique_ptr<BlockASTnode> b){
    body = std::move(b);
  }
  virtual llvm::Value *codegen();
  virtual string to_string(string pre) const override;
};

class FunDeclASTnode : public ASTnode{
public:
  unique_ptr<FunProtoASTnode> funproto;
  unique_ptr<FunBodyASTnode> funbody;
  FunDeclASTnode( unique_ptr<TypeSpecASTnode> ts,
                  unique_ptr<IdentASTnode> id,
                  unique_ptr<ParamListASTnode> ps,
                  unique_ptr<BlockASTnode> b){
  funproto = std::make_unique<FunProtoASTnode>(std::move(ts), std::move(id), std::move(ps));
  funbody = std::make_unique<FunBodyASTnode>(std::move(b));
  };
  virtual llvm::Value *codegen();
  virtual string to_string(string pre) const override;
};

class ExternASTnode : public ASTnode{
public:
  unique_ptr<FunProtoASTnode> funproto;
  ExternASTnode(unique_ptr<TypeSpecASTnode> ts,
                unique_ptr<IdentASTnode> id,
                unique_ptr<ParamListASTnode> ps){
  funproto = std::make_unique<FunProtoASTnode>(std::move(ts), std::move(id), std::move(ps));
  };
  virtual llvm::Value *codegen();
  virtual string to_string(string pre) const override;
};

class DeclASTnode : public ASTnode{
public:
  bool isVar;
  unique_ptr<VarDeclASTnode> vardecl;
  unique_ptr<FunDeclASTnode> fundecl;
  DeclASTnode(unique_ptr<VarDeclASTnode> vd
             ) : isVar(true), vardecl(std::move(vd)){}
  DeclASTnode(unique_ptr<FunDeclASTnode> fd
             ) : isVar(false), fundecl(std::move(fd)){}
  virtual llvm::Value *codegen();;
  virtual string to_string(string pre) const override;
};

class ProgramASTnode : public ASTnode{
public:
  vector<unique_ptr<ExternASTnode>> externs;
  vector<unique_ptr<DeclASTnode>> decls;
  ProgramASTnode(vector<unique_ptr<ExternASTnode>> &el,
                 vector<unique_ptr<DeclASTnode>> &dl){
    for (auto &elem : el){
      externs.push_back(std::move(elem));
    }
    for (auto &elem : dl){
      decls.push_back(std::move(elem));
    }
  }

  virtual llvm::Value *codegen();
  virtual string to_string(string pre) const override;
};

class CompileError : public std::exception{
  TOKEN tok;
  std::string message = "";
public:
  CompileError(TOKEN t, std::string comment) : tok(t) {
    string identification = "Compiling Error with token: " + tok.lexeme + " of type " + std::to_string(tok.type) + " on line " + std::to_string(tok.lineNo) + ", column " + std::to_string(tok.columnNo) + "\n";
    this->message += identification;
    this->message += comment;
  };
  CompileError(std::string comment) {
    this->message += comment;
  }
  const char *what() const noexcept override {
    return message.c_str();
  }
};

AllocaInst* CreateEntryBlockAlloca(Function *TheFunction, const std::string &VarName, Type *argtype);

AllocaInst* find_local(string funcname);

string typetostring(Type* t);

Type *tok_to_llvm_type(TOKEN_TYPE tok);

Value *widening_cast_or_err(Value* inputval, Type* goaltype, TOKEN tok);

Value *bool_cast(Value* val);

Value *int_cast(Value* val);

Value *force_cast(Value* inputval, Type* goaltype, TOKEN tok);

Type* widest_type(Value* v1, Value* v2);