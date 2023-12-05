#pragma once
#include <string>
#include <vector>
#include <deque>
#include <map>
#include "lexer.h"
#include "astnodes.h"
#include "loaddata.h"
#include <memory>
#include <exception>

using std::unique_ptr;

extern TOKEN CurTok;
extern std::deque<TOKEN> tok_buffer;

// defined in mccomp.cpp
extern std::vector<std::string> nonterminals;
extern std::vector<production_options> rhslist;
extern std::vector<std::string> terminals;
extern std::map<std::string, bool> nullable;
extern std::map<std::string, std::vector<std::string>> first;
extern std::map<std::string, std::vector<std::string>> follow;

TOKEN getNextToken();
void putBackToken(TOKEN tok);
enum TOKEN_TYPE word_to_type(std::string word);
std::vector<int> terminals_to_int(std::vector<std::string> terminals);

unique_ptr<ProgramASTnode> parser();
unique_ptr<ProgramASTnode> parse_program();
void parse_extern_list(vector<unique_ptr<ExternASTnode>> &externlist);
void parse_decl_list(vector<unique_ptr<DeclASTnode>> &decllist);
unique_ptr<ExternASTnode> parse_extern();
void parse_extern_list1(vector<unique_ptr<ExternASTnode>> &externlist);
unique_ptr<DeclASTnode> parse_decl();
void parse_decl_list1(vector<unique_ptr<DeclASTnode>> &decllist);
unique_ptr<TypeSpecASTnode>parse_type_spec();
unique_ptr<ParamListASTnode> parse_params();
unique_ptr<VarDeclASTnode> parse_var_decl();
unique_ptr<FunDeclASTnode> parse_fun_decl();
unique_ptr<VarTypeASTnode> parse_var_type();
unique_ptr<BlockASTnode> parse_block();
unique_ptr<ParamListASTnode> parse_param_list();
unique_ptr<ParamASTnode> parse_param();
unique_ptr<ParamListASTnode> parse_param_list1();
void parse_local_decls(vector<unique_ptr<DeclASTnode>> &decllist);
unique_ptr<DeclASTnode> parse_local_decl();
void parse_stmt_list(vector<unique_ptr<StmtASTnode>> &stmtlist);
unique_ptr<StmtASTnode> parse_stmt();
unique_ptr<ExprASTnode> parse_expr_stmt();
unique_ptr<WhileASTnode> parse_while_stmt();
unique_ptr<IfASTnode> parse_if_stmt();
unique_ptr<BlockASTnode> parse_else_stmt();
unique_ptr<ReturnASTnode> parse_return_stmt();
unique_ptr<ExprASTnode> parse_expr();
unique_ptr<ExprASTnode> parse_rval();
unique_ptr<ExprASTnode> parse_rvalprime(unique_ptr<ExprASTnode> lhs);
unique_ptr<ExprASTnode> parse_rval1();
unique_ptr<ExprASTnode> parse_rval1prime(unique_ptr<ExprASTnode> lhs);
unique_ptr<ExprASTnode> parse_rval2();
unique_ptr<ExprASTnode> parse_rval2prime(unique_ptr<ExprASTnode> lhs);
unique_ptr<ExprASTnode> parse_rval3();
unique_ptr<ExprASTnode> parse_rval3prime(unique_ptr<ExprASTnode> lhs);
unique_ptr<ExprASTnode> parse_rval4();
unique_ptr<ExprASTnode> parse_rval4prime(unique_ptr<ExprASTnode> lhs);
unique_ptr<ExprASTnode> parse_rval5();
unique_ptr<ExprASTnode> parse_rval5prime(unique_ptr<ExprASTnode> lhs);

unique_ptr<ExprASTnode> parse_rval6();
unique_ptr<ExprASTnode> parse_rval7();
void parse_args(vector<unique_ptr<ExprASTnode>> &arglist);
void parse_arg_list(vector<unique_ptr<ExprASTnode>> &arglist);
void parse_arg_list1(vector<unique_ptr<ExprASTnode>> &arglist);

class ParseError : public std::exception {
  TOKEN tok;
  std::string message = "";
public:
  ParseError(TOKEN t, std::string comment) : tok(t) {
    this->message += ("parsing error with token " + tok.lexeme + " of type " + std::to_string(tok.type) + " on line " + std::to_string(tok.lineNo) + ", column " + std::to_string(tok.columnNo) + "\n" + comment);
  }
  const char *what() const noexcept override {
    return message.c_str();
  }
};

