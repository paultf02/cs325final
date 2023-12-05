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
#include "lexer.h"
#include "astnodes.h"
#include "loaddata.h"
#include "parser.h"
#include <algorithm>
#include <cassert>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <map>
#include <memory>
#include <queue>
#include <set>
#include <sstream>
#include <stack>
#include <stdexcept>
#include <string.h>
#include <string>
#include <system_error>
#include <utility>
#include <vector>

using std::string;
using std::vector;
using std::unique_ptr;
using std::make_unique;
using std::map;

using namespace llvm;

FILE *pFile;

string IdentifierStr; // Filled in if IDENT
int IntVal;                // Filled in if INT_LIT
bool BoolVal;              // Filled in if BOOL_LIT
float FloatVal;            // Filled in if FLOAT_LIT
string StringVal;     // Filled in if String Literal
int lineNo, columnNo; // not static because used in mccomp.cpp
TOKEN CurTok;
std::deque<TOKEN> tok_buffer;

string pathprefix = "./";
string grammarversion = "11";
string csvfilename = pathprefix + "firstfollow/firstfollowg"+grammarversion+"sep.csv";
string grammarfilename = pathprefix + "grammars/transformedgrammar"+grammarversion+".txt";
string terminalfilename = pathprefix + "grammars/terminals2.txt";
vector<string> nonterminals; // this is the lhs of the grammar, does not include epsilon
vector<production_options> rhslist;
vector<string> terminals;
std::map<string, bool> nullable; //dictionaries for ifnullable, first and follow sets
std::map<string, vector<string>> first;
std::map<string, vector<string>> follow;

// AST Printer
string br = "|-";
string sp = " ";
string nl = "\n";
inline llvm::raw_ostream &operator<<(llvm::raw_ostream &os, const ASTnode &ast) {
  os << ast.to_string("");
  return os;
}

// Code Generation
unique_ptr<LLVMContext> TheContext;
unique_ptr<Module> TheModule;
unique_ptr<IRBuilder<>> Builder;
vector<unique_ptr<map<string, AllocaInst*>>> NamedValuesVector; // local var table(s)
map<string, GlobalVariable*> GlobalNamedValues; //global var table
Value *llvmnull;

// Main driver code.
int main(int argc, char **argv) {
  if (argc == 2) {
    pFile = fopen(argv[1], "r");
    if (pFile == NULL)
      perror("Error opening file");
  } else {
    std::cout << "Usage: ./code InputFile\n";
    return 1;
  }

  string fname = argv[1];
  lineNo = 1; // initialize line number and column numbers to zero
  columnNo = 1;
  load_data(); // load first and follow set from file, load list of terminals, load list of production lhs and rhs respectively
  getNextToken(); // get the first token
  unique_ptr<ProgramASTnode> programrootnode = parser(); // Run the parser now.
  fprintf(stderr, "Lexing and Parsing Finished\n");
  // llvm::outs() << *programrootnode << "\n";
  string ASTstring = programrootnode->to_string("");
  std::ofstream out("AST.txt");
  out << ASTstring;
  out.close();
  fprintf(stderr, "Printing AST Finished\n");
  // Start printing final IR
  // Print out all of the generated code into a file called output.ll or to the terminal
  
  auto Filename = "output.ll";
  std::error_code EC;
  llvm::raw_fd_ostream dest(Filename, EC, llvm::sys::fs::OF_None);
  if (EC) {
    llvm::errs() << "Could not open file: " << EC.message();
    return 2;
  }
  
  // Make the module, which holds all the code.
  TheContext = make_unique<LLVMContext>();
  TheModule = make_unique<llvm::Module>(fname, *TheContext);
  Builder = make_unique<llvm::IRBuilder<>>(*TheContext);
  llvmnull = Constant::getNullValue(Type::getInt1Ty(*TheContext));

  programrootnode->codegen();
  fprintf(stderr, "Codegen Finished\n");
  
  TheModule->llvm::Module::print(dest, nullptr); // print IR to file output.ll
  // TheModule->print(llvm::errs(), nullptr); // print IR to terminal
  // End printing final IR
  fclose(pFile); // close the file that contains the code that was parsed
  return 0;
}
