#include "loaddata.h"
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <algorithm>

// make a queue of tokens


bool str_to_bool(std::string word){
  if (word == "True"){
    return true;
  } else if (word == "False"){
    return false;
  } else {
    throw std::runtime_error("cannot convert string to bool");
  }
}

int nonterminal_index(std::string term){
  auto it = std::find(nonterminals.begin(), nonterminals.end(), term);
  if (it == nonterminals.end()){
    // name not in vector
    throw std::runtime_error("this term is not in the terminal list");
  } else {
    auto index = std::distance(nonterminals.begin(), it);
    return index;
  }
}

// The below is ChatGPT3.5 code with the prompt
// "write cpp code that takes a string and splits it delimited by commas. make sure to escape out commas in single quotes."
// there were some minor errors with the generated code so i changed some parts
std::vector<std::string> splitString(const std::string& input) {
    std::vector<std::string> substrings;
    std::string currentSubstring;

    bool insideSingleQuotes = false;

    for (char c : input) {
        if (c == '\'') {
            insideSingleQuotes = !insideSingleQuotes;
            currentSubstring += c;
        } else if (c == ',' && !insideSingleQuotes) {
            // If it's a comma and not inside single quotes, start a new substring
            substrings.push_back(currentSubstring);
            currentSubstring.clear();
        } else {
            // Otherwise, append the character to the current substring
            currentSubstring += c;
        }
    }

    // Add the last substring after the loop
    substrings.push_back(currentSubstring);

    return substrings;
}

// The below is ChatGPT3.5 code with the prompt
// "write cpp code to split a string based on a string delimiter"
std::vector<std::string> splitString_strdelim(const std::string& input, const std::string& delimiter) {
    std::vector<std::string> tokens;
    size_t start = 0, end = 0;

    while ((end = input.find(delimiter, start)) != std::string::npos) {
        tokens.push_back(input.substr(start, end - start));
        start = end + delimiter.length();
    }

    // Add the last token (or the only token if there is no delimiter)
    tokens.push_back(input.substr(start));

    return tokens;
}

//

void load_data(){

  std::fstream csvfile, terminalfile, grammarfile;
  std::string line, part;
  std::string name, isnullable, firstset, followset, lhs, rhs, or_sequence;
  char sep = '?';

  csvfile.open(csvfilename, std::ios::in);
  if (!csvfile) {                        
    std::cout<<"File doesn’t exist.";
    throw std::runtime_error("could not open file");         
  }
  // first line
  std::getline(csvfile, line); 
  // Read data, line by line
  while (std::getline(csvfile, line)){
    std::stringstream ss(line);
    std::getline(ss, name, sep);
    std::getline(ss, isnullable, sep);
    std::getline(ss, firstset, sep);
    std::getline(ss, followset, sep);

    firstset = firstset.substr(1, firstset.length()-2);
    followset = followset.substr(1, followset.length()-2);
    first.insert({name, splitString(firstset)});
    follow.insert({name, splitString(followset)});
    if (isnullable == "True"){
      nullable.insert({name, true});
    } else if (isnullable == "False"){
      nullable.insert({name, false});
    }
  }
  csvfile.close();

  terminalfile.open(terminalfilename, std::ios::in);
  if (!terminalfile) {                        
    std::cout<<"File doesn’t exist.";
    throw std::runtime_error("could not open file");         
  }
  while (std::getline(terminalfile, line)){
    terminals.push_back(line);
  }
  terminalfile.close();

  grammarfile.open(grammarfilename, std::ios::in);
  if (!grammarfile) {                        
    std::cout<<"File doesn’t exist.";
    throw std::runtime_error("could not open grammar file");         
  }
  while (std::getline(grammarfile, line)){

    std::vector<std::string> splitline = splitString_strdelim(line, " -> ");
    std::string lhs = splitline[0];
    std::string rhs = splitline[1];

    nonterminals.push_back(lhs);
    std::vector<std::string> splitor = splitString_strdelim(rhs, " | ");

    production_options rhstyped;
    sentence thissentence;
    for (auto &elem: splitor){
      thissentence = splitString_strdelim(elem, " ");
      rhstyped.push_back(thissentence);

    }
    rhslist.push_back(rhstyped);


  }
  grammarfile.close();

  // name = "expr";
  // std::cout << name <<" first:\n";
  // for (auto &elem : first[name]){
  //   std::cout << elem << '\n';
  // }
  // std::cout << name <<" follow:\n";
  // for (auto &elem : follow[name]){
  //   std::cout << elem << '\n';
  // }

  // std::cout << "iterating through terminals: " << '\n';
  // for (auto &elem : terminals){
  //   std::cout << elem << '\n';
  // }
}