#include <fstream>
#include <iostream>
#include <sstream>
#include <set>
#include <cctype>
#include <vector>
#include <filesystem>
#include "scanner.h"
#include "parser.h"
#include "node.h"
#include "statSem.h"
#include "codeGen.h"


int main(int argc, char* argv[]) {
   std::istream* input;
   std::ifstream inputFile;
   std::ofstream outputFile;
   std::string outputFileName;


   if (argc > 1) {
       inputFile.open(argv[1]);
       if (!inputFile) {
           std::cerr << "Error opening file " << argv[1] << std::endl;
           return 1;
       }
       input = &inputFile;


       std::string inputPath(argv[1]);
       std::size_t pos = inputPath.find_last_of("/\\");
       std::string baseName = inputPath.substr(pos + 1);
       pos = baseName.find_last_of(".");
       if (pos != std::string::npos) {
           baseName = baseName.substr(0, pos);
       }
       outputFileName = baseName + ".asm";
   } else {
       input = &std::cin;
       outputFileName = "kb.asm";
   }


   outputFile.open(outputFileName);
   if (!outputFile) {
       std::cerr << "Error opening output file " << outputFileName << std::endl;
       return 1;
   }


   std::string line;
   int lineNum = 0;
   Token token;
   std::vector<Token> tokens;
   while (true) {
       //lineNum++;
       token = scanner(*input, line, lineNum);
       tokens.push_back(token);
       if (input->eof()) break;
   }


   Node* root = parser(tokens);


   //printTree(root, 0);


   applyStaticSemantics(root);


   recursiveGenerator(root, outputFile);


   if (inputFile.is_open()) {
       inputFile.close();
   }


   return 0;
}
