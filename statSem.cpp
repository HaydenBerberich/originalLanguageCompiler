#include <iostream>
#include <vector>
#include <string>
#include "node.h"


enum TokenType {
   KW_tk,
   ID_tk,
   INT_tk,
   OP_tk,
   EOF_tk,
   ERROR,
};


std::vector<std::string> symbolTable;
int varNum;


void insert(std::string symbol) {
   symbolTable.push_back(symbol);
}


bool verify(std::string symbol) {
   for (std::string s : symbolTable) {
       if (s == symbol) {
           return true;
       }
   }
   return false;
}


void printSymbolTable() {
   std::cout << "Symbol Table: ";
   for (std::string s : symbolTable) {
       std::cout << s << " ";
   }
   std::cout << std::endl;
}


void applyStaticSemantics(Node* node) {
   if (node == nullptr) {
       return;
   }


   if (node->label == "Vars" || node->label == "Func") {
       if (node->token2.tokenInstance != "") {
           if (verify(node->token2.tokenInstance)) {
               std::cerr << "SEMANTICS ERROR: Identifier " << node->token2.tokenInstance << " is already defined.\n";
               exit(1);
           } else {
               insert(node->token2.tokenInstance);
               ++varNum;
               //printSymbolTable();
           }
       }
   } else {
       if (node->token1.tokenID == ID_tk && !verify(node->token1.tokenInstance)) {
           std::cerr << "SEMANTICS ERROR: " << node->token1.tokenInstance << " is not declared" << std::endl;
           std::cerr << "Line: " << node->token1.lineNum << " Char: " << node->token1.charNum << std::endl;
           exit(1);
       }
       if (node->token2.tokenID == ID_tk && !verify(node->token2.tokenInstance)) {
           std::cerr << "SEMANTICS ERROR: " << node->token2.tokenInstance << " is not declared" << std::endl;
           std::cerr << "Line: " << node->token2.lineNum << " Char: " << node->token2.charNum << std::endl;
           exit(1);
       }
       if (node->token3.tokenID == ID_tk && !verify(node->token3.tokenInstance)) {
           std::cerr << "SEMANTICS ERROR: " << node->token3.tokenInstance << " is not declared" << std::endl;
           std::cerr << "Line: " << node->token3.lineNum << " Char: " << node->token3.charNum << std::endl;
           exit(1);
       }
       if (node->token4.tokenID == ID_tk && !verify(node->token4.tokenInstance)) {
           std::cerr << "SEMANTICS ERROR: " << node->token4.tokenInstance << " is not declared" << std::endl;
           std::cerr << "Line: " << node->token4.lineNum << " Char: " << node->token4.charNum << std::endl;
           exit(1);
       }
       if (node->token5.tokenID == ID_tk && !verify(node->token5.tokenInstance)) {
           std::cerr << "SEMANTICS ERROR: " << node->token5.tokenInstance << " is not declared" << std::endl;
           std::cerr << "Line: " << node->token5.lineNum << " Char: " << node->token5.charNum << std::endl;
           exit(1);
       }
   }


   if (node->label == "Block") {
       varNum = 0;
   }


   applyStaticSemantics(node->child1);
   applyStaticSemantics(node->child2);
   applyStaticSemantics(node->child3);
   applyStaticSemantics(node->child4);


   if (node->label == "Block") {
       if (symbolTable.size() >= static_cast<std::vector<std::string>::size_type>(varNum)) {
           symbolTable.resize(symbolTable.size() - varNum);
       }
       //printSymbolTable();
   }
}
