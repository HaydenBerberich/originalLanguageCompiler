#include <fstream>
#include <iostream>
#include <sstream>
#include <set>
#include <cctype>
#include "scanner.h"
#include "token.h"


//Define keywords and operators
std::set<std::string> keywords = {"start", "stop", "while", "repeat", "until", "label", "return", "cin", "cout", "tape", "jump", "if", "then", "pick", "create", "set", "func"};
std::set<std::string> operators = {"=", "<", ">", "==", "=!=", ":", ":=", "+", "-", "*", "/", "^", ".", "(", ")", ",", "{", "}", ";", "[", "]", "||", "&&"};


enum TokenType {
   KW_tk,
   ID_tk,
   INT_tk,
   OP_tk,
   EOF_tk,
   ERROR,
};


//checks for keyword
bool isKeyword(const std::string& str) {
   return keywords.find(str) != keywords.end();
}


//checks for identifier
bool isIdentifier(const std::string& str) {
   if (!isalpha(str[0])) return false;
   if (str.size() > 8) return false;
   for (char c : str) {
       if (!isalnum(c) && c != '_') return false;
   }
   return true;
}


//checks for integer
bool isInteger(const std::string& str) {
   if (str.size() > 8) return false;
   for (char c : str) {
       if (!isdigit(c)) return false;
   }
   return true;
}


//checks for operator
bool isOperator(const std::string& str) {
   return operators.find(str) != operators.end();
}


Token scanner(std::istream& input, std::string& line, int& lineNum) {
   static std::istringstream iss(line);
   static std::string token;


   while (iss.peek() == ' ' || iss.peek() == '\t') {
       iss.get();
   }


   //reads next line
   if (iss.eof()) {
       do {
           if (!std::getline(input, line)) {
               return {EOF_tk, "", lineNum, 0};
           }
           lineNum++;
           iss.str(line);
           iss.clear();


           while (iss.peek() == ' ' || iss.peek() == '\t') {
               iss.get();
           }
       } while (line.empty() || line[0] == '/');
   }


   int charPos = static_cast<int>(iss.tellg()) + 1;


   //returns eof token
   if (!(iss >> token)) {
       return {EOF_tk, "", lineNum, charPos};
   }


   if (token[0] == '/' && token[1] == '/') { //comment
       std::string restOfLine;
       std::getline(iss, restOfLine);
   } else if (isKeyword(token)) { //keyword
       return {KW_tk, token, lineNum, charPos};
   } else if (isIdentifier(token)) { //identifier
       return {ID_tk, token, lineNum, charPos};
   } else if (isInteger(token)) { //integer
       return {INT_tk, token, lineNum, charPos};
   } else if (isOperator(token)) { //operator
       return {OP_tk, token, lineNum, charPos};
   } else { //error
       std::cerr << "SCANNER ERROR: line " << lineNum << " char " << charPos << " at " << token << std::endl;
       return {ERROR, token, lineNum, charPos};
   }


   return scanner(input, line, lineNum);
}


//returns token type as string
std::string tokenTypeToString(int tokenValue) {
   switch (tokenValue) {
       case KW_tk:
           return "KW_tk";
       case ID_tk:
           return "ID_tk";
       case INT_tk:
           return "INT_tk";
       case OP_tk:
           return "OP_tk";
       case EOF_tk:
           return "EOF_tk";
       case ERROR:
           return "ERROR";
       default:
           return "Unknown TokenType";
   }
}


//test scanner and output information
void testScanner(std::istream& input, std::string& line, int& lineNum) {
   Token token;
   do {
       token = scanner(input, line, lineNum);
       std::cout << "ID: " << tokenTypeToString(token.tokenID) << " Instance: " << token.tokenInstance << " Line: " << token.lineNum << " Character: " << token.charNum << std::endl;
   } while (token.tokenID != EOF_tk);
}
