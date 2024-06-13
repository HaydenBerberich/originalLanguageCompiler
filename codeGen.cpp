#include <iostream>
#include <vector>
#include <fstream>
#include <cstring>
#include "codeGen.h"
#include "node.h"


static int LabelCntr=0; /* counting unique labels generated */
static int VarCntr=0; /* counting unique temporaries generated */
typedef enum {VAR, LABEL} nameType;
static char Name[20]; /* for creation of unique names */


static char *newName(nameType what)
{ if (what==VAR) // creating new temporary
   sprintf(Name,"T%d",VarCntr++); /* generate a new label as T0, T1, etc */
 else // creating new Label
   sprintf(Name,"L%d",LabelCntr++); /* new lables as L0, L1, etc */
 return(Name);
}


std::vector<std::string> vars;


void recursiveGenerator(Node* node, std::ofstream& outputFile) {
   char label1[20];
   char label2[20];
   char label3[20];
   char label4[20];
   char argR1[20];
   char argR2[20];


   if (node == nullptr) return;


   if (node->label == "Program") { //<program> -> <vars> tape <func> <block> | <vars> tape <block>
       recursiveGenerator(node->child1, outputFile);
       recursiveGenerator(node->child2, outputFile);
       recursiveGenerator(node->child3, outputFile);
       recursiveGenerator(node->child4, outputFile);


       outputFile << "STOP\n";


       for (const auto& var : vars) {
           outputFile << var << " 0\n";
       }


   } else if (node->label == "Func") { //<func> -> func Identifier <block>
       outputFile << "BR " << node->token2.tokenInstance << "End\n";
       outputFile << node->token2.tokenInstance << ": NOOP\n";
       recursiveGenerator(node->child1, outputFile);
       recursiveGenerator(node->child2, outputFile);
       recursiveGenerator(node->child3, outputFile);
       recursiveGenerator(node->child4, outputFile);
       outputFile << "BR " << node->token2.tokenInstance << "Return\n";
       outputFile << node->token2.tokenInstance << "End: NOOP\n";
   } else if (node->label == "Block") { //<block> -> { <vars> <stats> }
       recursiveGenerator(node->child1, outputFile);
       recursiveGenerator(node->child2, outputFile);
       recursiveGenerator(node->child3, outputFile);
       recursiveGenerator(node->child4, outputFile);
   } else if (node->label == "Vars") { //<vars> -> empty | create Identifier <vars2>
       if (node->token1.tokenInstance == "create") {
           vars.push_back(node->token2.tokenInstance);
       }


       recursiveGenerator(node->child1, outputFile);
       recursiveGenerator(node->child2, outputFile);
       recursiveGenerator(node->child3, outputFile);
       recursiveGenerator(node->child4, outputFile);
   } else if (node->label == "Vars2") { //<vars2> -> ; | := Integer ; <vars>
       if (node->token1.tokenInstance == ";") {
           outputFile << "LOAD -7\n";
           outputFile << "STORE " << vars.back() << "\n";
       } else if (node->token1.tokenInstance == ":=") {
           outputFile << "LOAD " << node->token2.tokenInstance << "\n";
           outputFile << "STORE " << vars.back() << "\n";
       }


       recursiveGenerator(node->child1, outputFile);
       recursiveGenerator(node->child2, outputFile);
       recursiveGenerator(node->child3, outputFile);
       recursiveGenerator(node->child4, outputFile);
   } else if (node->label == "Expr") { //<expr> -> <N> <expr'>
       recursiveGenerator(node->child1, outputFile);
       recursiveGenerator(node->child2, outputFile);
       recursiveGenerator(node->child3, outputFile);
       recursiveGenerator(node->child4, outputFile);
   } else if (node->label == "Expr2") { //<expr'> -> - <expr> | Îµ
       if (node->token1.tokenInstance == "-") {
           strcpy(argR1, newName(VAR));
           vars.push_back(argR1);
           outputFile << "STORE " << argR1 << "\n";
           recursiveGenerator(node->child1, outputFile);
           strcpy(argR2, newName(VAR));
           vars.push_back(argR2);
           outputFile << "STORE " << argR2 << "\n";
           outputFile << "LOAD " << argR1 << "\n";
           outputFile << "SUB " << argR2 << "\n";
       } else {
           recursiveGenerator(node->child1, outputFile);
           recursiveGenerator(node->child2, outputFile);
           recursiveGenerator(node->child3, outputFile);
           recursiveGenerator(node->child4, outputFile);
       }
   } else if (node->label == "N") { //<N> -> <A> <N'>
       recursiveGenerator(node->child1, outputFile);
       recursiveGenerator(node->child2, outputFile);
       recursiveGenerator(node->child3, outputFile);
       recursiveGenerator(node->child4, outputFile);
   } else if (node->label == "N2") { //<N'> -> / <A> <N'> | + <A> <N'> | Îµ
       if (node->token1.tokenInstance == "+") {
           strcpy(argR1, newName(VAR));
           vars.push_back(argR1);
           outputFile << "STORE " << argR1 << "\n";
           recursiveGenerator(node->child1, outputFile);
           strcpy(argR2, newName(VAR));
           vars.push_back(argR2);
           outputFile << "STORE " << argR2 << "\n";
           outputFile << "LOAD " << argR1 << "\n";
           outputFile << "ADD " << argR2 << "\n";
       } else if (node->token1.tokenInstance == "/") {
           strcpy(argR1, newName(VAR));
           vars.push_back(argR1);
           outputFile << "STORE " << argR1 << "\n";
           recursiveGenerator(node->child1, outputFile);
           strcpy(argR2, newName(VAR));
           vars.push_back(argR2);
           outputFile << "STORE " << argR2 << "\n";
           outputFile << "LOAD " << argR1 << "\n";
           outputFile << "DIV " << argR2 << "\n";
       } else {
           recursiveGenerator(node->child1, outputFile);
           recursiveGenerator(node->child2, outputFile);
           recursiveGenerator(node->child3, outputFile);
           recursiveGenerator(node->child4, outputFile);
       }
   } else if (node->label == "A") { //<A> -> <M> <A'>
       recursiveGenerator(node->child1, outputFile);
       recursiveGenerator(node->child2, outputFile);
       recursiveGenerator(node->child3, outputFile);
       recursiveGenerator(node->child4, outputFile);
   } else if (node->label == "A2") { //<A'> -> * <A> | Îµ
       if (node->token1.tokenInstance == "*") {
           strcpy(argR1, newName(VAR));
           vars.push_back(argR1);
           outputFile << "STORE " << argR1 << "\n";
           recursiveGenerator(node->child1, outputFile);
           strcpy(argR2, newName(VAR));
           vars.push_back(argR2);
           outputFile << "STORE " << argR2 << "\n";
           outputFile << "LOAD " << argR1 << "\n";
           outputFile << "MULT " << argR2 << "\n";
       } else {
           recursiveGenerator(node->child1, outputFile);
           recursiveGenerator(node->child2, outputFile);
           recursiveGenerator(node->child3, outputFile);
           recursiveGenerator(node->child4, outputFile);
       }
   } else if (node->label == "M") { //<M> -> ^ <M> | <R>
       if (node->token1.tokenInstance == "^") {
           strcpy(label1, newName(LABEL));
           strcpy(label2, newName(LABEL));
           strcpy(label3, newName(LABEL));
           strcpy(label4, newName(LABEL));
           recursiveGenerator(node->child1, outputFile);
           outputFile << "BRZERO " << label2 << "\n";
           outputFile << "SUB 1\n";
           outputFile << "BRZERO " << label3 << "\n";
           outputFile << "ADD 1\n";
           outputFile << "BR " << label4 << "\n";
           outputFile << label2 << ": NOOP\n";
           outputFile << "LOAD 1\n";
           outputFile << "BR " << label1 << "\n";
           outputFile << label3 << ": NOOP\n";
           outputFile << "LOAD 0\n";
           outputFile << "BR " << label1 << "\n";
           outputFile << label4 << ": NOOP\n";
           outputFile << "MULT -1\n";
           outputFile << label1 << ": NOOP\n";
       } else {
           recursiveGenerator(node->child1, outputFile);
           recursiveGenerator(node->child2, outputFile);
           recursiveGenerator(node->child3, outputFile);
           recursiveGenerator(node->child4, outputFile);
       }
   } else if (node->label == "R") { //<R> -> ( <expr> ) | Identifier | Integer
       if (node->token1.tokenInstance != "(") {
           outputFile << "LOAD " << node->token1.tokenInstance << "\n";
       }
       recursiveGenerator(node->child1, outputFile);
       recursiveGenerator(node->child2, outputFile);
       recursiveGenerator(node->child3, outputFile);
       recursiveGenerator(node->child4, outputFile);
   } else if (node->label == "Stats") { //<stats> -> <stat> <mStat>
       recursiveGenerator(node->child1, outputFile);
       recursiveGenerator(node->child2, outputFile);
       recursiveGenerator(node->child3, outputFile);
       recursiveGenerator(node->child4, outputFile);
   } else if (node->label == "MStat") { //<mStat> -> empty | <stat> <mStat>
       recursiveGenerator(node->child1, outputFile);
       recursiveGenerator(node->child2, outputFile);
       recursiveGenerator(node->child3, outputFile);
       recursiveGenerator(node->child4, outputFile);
   } else if (node->label == "Stat") { //<stat> -> <in> ; | <out> ; | <block> | <if> ; | <loop1> ; | <loop2> ; | <assign> ; | <goto> ; | <label> ; | <pick> ;
       recursiveGenerator(node->child1, outputFile);
       recursiveGenerator(node->child2, outputFile);
       recursiveGenerator(node->child3, outputFile);
       recursiveGenerator(node->child4, outputFile);
   } else if (node->label == "In") { //<in> -> cin Identifier
       outputFile << "READ " << node->token2.tokenInstance << "\n";
       recursiveGenerator(node->child1, outputFile);
       recursiveGenerator(node->child2, outputFile);
       recursiveGenerator(node->child3, outputFile);
       recursiveGenerator(node->child4, outputFile);
   } else if (node->label == "Out") { //<out> -> cout <expr>
       recursiveGenerator(node->child1, outputFile);
       recursiveGenerator(node->child2, outputFile);
       recursiveGenerator(node->child3, outputFile);
       recursiveGenerator(node->child4, outputFile);
       strcpy(argR1, newName(VAR));
       vars.push_back(argR1);
       outputFile << "STORE " << argR1 << "\n";
       outputFile << "WRITE " << argR1 << "\n";
   } else if (node->label == "If") { //<if> -> if [ <expr> <RO> <expr> ] then <stat>
       recursiveGenerator(node->child3, outputFile);
       strcpy(argR1, newName(VAR));
       vars.push_back(argR1);
       outputFile << "STORE " << argR1 << "\n";
       recursiveGenerator(node->child1, outputFile);
       strcpy(argR2, newName(VAR));
       vars.push_back(argR2);
       outputFile << "STORE " << argR2 << "\n";
       outputFile << "SUB " << argR1 << "\n";
       strcpy(label1, newName(LABEL));
       if (node->child2->token1.tokenInstance == "<") {
           outputFile << "BRPOS " << label1 << "\n";
       } else if (node->child2->token1.tokenInstance == ">") {
           outputFile << "BRNEG " << label1 << "\n";
       } else if (node->child2->token1.tokenInstance == "==") {
           outputFile << "BRPOS " << label1 << "\n";
           outputFile << "BRNEG " << label1 << "\n";
       } else if (node->child2->token1.tokenInstance == "=!=") {
           outputFile << "BRZERO " << label1 << "\n";
       } else if (node->child2->token1.tokenInstance == ".") {
           outputFile << "LOAD " << argR2 << "\n";
           outputFile << "DIV 2\n";
           outputFile << "MULT 2\n";
           outputFile << "SUB " << argR2 << "\n";
           outputFile << "STORE " << argR2 << "\n";
           outputFile << "LOAD " << argR1 << "\n";
           outputFile << "DIV 2\n";
           outputFile << "MULT 2\n";
           outputFile << "SUB " << argR1 << "\n";
           outputFile << "STORE " << argR1 << "\n";
           outputFile << "SUB " << argR2 << "\n";
           outputFile << "BRPOS " << label1 << "\n";
           outputFile << "BRNEG " << label1 << "\n";
       }
       recursiveGenerator(node->child4, outputFile);
       outputFile << label1 << ": NOOP\n";
   } else if (node->label == "Pick") { //<pick> -> pick <expr> <pickbody>
       recursiveGenerator(node->child1, outputFile);
       strcpy(argR1, newName(VAR));
       vars.push_back(argR1);
       strcpy(label1, newName(LABEL));
       strcpy(label2, newName(LABEL));
       outputFile << "STORE " << argR1 << "\n";
       outputFile << "BRZERO " << label1 << "\n";
       recursiveGenerator(node->child2->child1, outputFile);
       outputFile << "BR " << label2 << "\n";
       outputFile << label1 << ": NOOP\n";
       recursiveGenerator(node->child2->child2, outputFile);
       outputFile << label2 << ": NOOP\n";
   } else if (node->label == "PickBody") { //<pickbody> -> <stat> : <stat>
       std::cerr << "Error: " << node->label << " node encountered.\n";
   } else if (node->label == "Loop1") { //<loop1> -> while [ <expr> <RO> <expr> ] <stat>
       strcpy(label1, newName(LABEL));
       outputFile << label1 << ": NOOP\n";
       strcpy(label2, newName(LABEL));
       recursiveGenerator(node->child3, outputFile);
       strcpy(argR1, newName(VAR));
       vars.push_back(argR1);
       outputFile << "STORE " << argR1 << "\n";
       recursiveGenerator(node->child1, outputFile);
       strcpy(argR2, newName(VAR));
       vars.push_back(argR2);
       outputFile << "STORE " << argR2 << "\n";
       outputFile << "SUB " << argR1 << "\n";
       if (node->child2->token1.tokenInstance == "<") {
           outputFile << "BRZPOS " << label2 << "\n";
       } else if (node->child2->token1.tokenInstance == ">") {
           outputFile << "BRZNEG " << label2 << "\n";
       } else if (node->child2->token1.tokenInstance == "==") {
           outputFile << "BRPOS " << label2 << "\n";
           outputFile << "BRNEG " << label2 << "\n";
       } else if (node->child2->token1.tokenInstance == "=!=") {
           outputFile << "BRZERO " << label2 << "\n";
       } else if (node->child2->token1.tokenInstance == ".") {
           outputFile << "LOAD " << argR2 << "\n";
           outputFile << "DIV 2\n";
           outputFile << "MULT 2\n";
           outputFile << "SUB " << argR2 << "\n";
           outputFile << "STORE " << argR2 << "\n";
           outputFile << "LOAD " << argR1 << "\n";
           outputFile << "DIV 2\n";
           outputFile << "MULT 2\n";
           outputFile << "SUB " << argR1 << "\n";
           outputFile << "STORE " << argR1 << "\n";
           outputFile << "SUB " << argR2 << "\n";
           outputFile << "BRPOS " << label2 << "\n";
           outputFile << "BRNEG " << label2 << "\n";
       }
       recursiveGenerator(node->child4, outputFile);
       outputFile << "BR " << label1 << "\n";
       outputFile << label2 << ": NOOP\n";
   } else if (node->label == "Loop2") { //<loop2> -> repeat <stat> until [ <expr> <RO> <expr> ]
       strcpy(label1, newName(LABEL));
       outputFile << label1 << ": NOOP\n";
       recursiveGenerator(node->child1, outputFile);
       recursiveGenerator(node->child4, outputFile);
       strcpy(argR1, newName(VAR));
       vars.push_back(argR1);
       outputFile << "STORE " << argR1 << "\n";
       recursiveGenerator(node->child2, outputFile);
       strcpy(argR2, newName(VAR));
       vars.push_back(argR2);
       outputFile << "STORE " << argR2 << "\n";
       outputFile << "SUB " << argR1 << "\n";
       if (node->child3->token1.tokenInstance == "<") {
           outputFile << "BRPOS " << label1 << "\n";
       } else if (node->child3->token1.tokenInstance == ">") {
           outputFile << "BRNEG " << label1 << "\n";
       } else if (node->child3->token1.tokenInstance == "==") {
           outputFile << "BRPOS " << label1 << "\n";
           outputFile << "BRNEG " << label1 << "\n";
       } else if (node->child3->token1.tokenInstance == "=!=") {
           outputFile << "BRZERO " << label1 << "\n";
       } else if (node->child3->token1.tokenInstance == ".") {
           outputFile << "LOAD " << argR2 << "\n";
           outputFile << "DIV 2\n";
           outputFile << "MULT 2\n";
           outputFile << "SUB " << argR2 << "\n";
           outputFile << "STORE " << argR2 << "\n";
           outputFile << "LOAD " << argR1 << "\n";
           outputFile << "DIV 2\n";
           outputFile << "MULT 2\n";
           outputFile << "SUB " << argR1 << "\n";
           outputFile << "STORE " << argR1 << "\n";
           outputFile << "SUB " << argR2 << "\n";
           outputFile << "BRPOS " << label1 << "\n";
           outputFile << "BRNEG " << label1 << "\n";
       }
   } else if (node->label == "Assign") { //<assign> -> set Identifier = <expr> | Identifier = <expr>
       recursiveGenerator(node->child1, outputFile);
       if (node->token2.tokenInstance != "") {
           outputFile << "STORE " << node->token2.tokenInstance << "\n";
       }
       recursiveGenerator(node->child2, outputFile);
       recursiveGenerator(node->child3, outputFile);
       recursiveGenerator(node->child4, outputFile);
   } else if (node->label == "R0") { //<RO> -> < | > | == | ... (three tokens here) | =!=
       std::cerr << "Error: " << node->label << " node encountered.\n";
   } else if (node->label == "Label") { //<label> -> label Identifier
       strcpy(label1, node->token2.tokenInstance.c_str());
       outputFile << label1 << ": NOOP\n";
   } else if (node->label == "Goto") { //<goto> -> jump Identifier
       strcpy(label1, node->token2.tokenInstance.c_str());
       outputFile << "BR " << label1 << "\n";
       outputFile << node->token2.tokenInstance << "Return: NOOP\n";
   }
}
