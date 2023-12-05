#pragma once
#include <string>
#include <vector>
#include <map>
#include <deque>
#include "lexer.h"

// each element of rhslist is rhs which is a list of sentences (each sentence for each or)
// std::vector<string> rhs;
// rhs is the list of atoms in the sentence
typedef std::vector<std::string> sentence;
typedef std::vector<sentence> production_options;

extern std::string csvfilename;
extern std::string terminalfilename;
extern std::string grammarfilename;

extern std::vector<std::string> nonterminals; // this is the lhs of the grammar, does not include epsilon
extern std::vector<production_options> rhslist;
extern std::vector<std::string> terminals;

//below are our dictionaries for first and follow sets
extern std::map<std::string, bool> nullable;
extern std::map<std::string, std::vector<std::string>> first;
extern std::map<std::string, std::vector<std::string>> follow;

bool str_to_bool(std::string word);
int nonterminal_index(std::string term);
std::vector<std::string> splitString(const std::string& input);
std::vector<std::string> splitString_strdelim(const std::string& input, const std::string& delimiter);
void load_data();