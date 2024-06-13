#include <iostream>
#include <vector>
#include <cstdlib>
#include "parser.h"
#include "token.h"
#include <node.h>


enum TokenType {
   KW_tk,
   ID_tk,
   INT_tk,
   OP_tk,
   EOF_tk,
   ERROR,
};


Token currentToken;
Token lookaheadToken;
size_t tokenIndex = 0;
std::vector<Token> tokens;


void getNextToken() {
   if (tokenIndex < tokens.size()) {
       currentToken = tokens[tokenIndex];
       tokenIndex++;
   }
}


Node* program() { //<program> -> <vars> tape <func> <block> | <vars> tape <block>
   Node* node = new Node("Program");
   node->child1 = vars();
   if (currentToken.tokenInstance == "tape") {
       node->token1 = currentToken; // Store 'tape' token
       getNextToken();
       if (currentToken.tokenInstance == "func") {
           node->child2 = func();
           node->child3 = block();
       } else {
           node->child2 = block();
       }
   } else {
       std::cerr << "Error: Expected tape\n";
       printError();
       exit(1);
   }
   return node;
}


Node* func() { //<func> -> func Identifier <block>
   Node* node = new Node("Func");
   if (currentToken.tokenInstance == "func") {
       node->token1 = currentToken; // Store 'func' token
       getNextToken();
       if (currentToken.tokenID == ID_tk) {
           node->token2 = currentToken; // Store Identifier token
           getNextToken();
           node->child1 = block();
       } else {
           std::cerr << "Error: Expected Identifier\n";
           printError();
           exit(1);
       }
   } else {
       std::cerr << "Error: Expected 'func'\n";
       printError();
       exit(1);
   }
   return node;
}


Node* block() { //<block> -> { <vars> <stats> }
   Node* node = new Node("Block");
   if (currentToken.tokenInstance == "{") {
       node->token1 = currentToken; // Store '{' token
       getNextToken();
       node->child1 = vars();
       node->child2 = stats();
       if (currentToken.tokenInstance == "}") {
           node->token2 = currentToken; // Store '}' token
           getNextToken();
       } else {
           std::cerr << "Error: Expected '}'\n";
           printError();
           exit(1);
       }
   } else {
       std::cerr << "Error: Expected '{'\n";
       printError();
       exit(1);
   }
   return node;
}


Node* vars() { //<vars> -> empty | create Identifier <vars2>
   Node* node = new Node("Vars");
   if (currentToken.tokenInstance == "create") {
       node->token1 = currentToken; // Store 'create' token
       getNextToken();
       if (currentToken.tokenID == ID_tk) {
           node->token2 = currentToken; // Store Identifier token
           getNextToken();
           node->child1 = vars2();
       } else {
           std::cerr << "Error: Expected identifier\n";
           printError();
           exit(1);
       }
       return node;
   } else if (currentToken.tokenInstance == "tape") {
       // Set tokens to empty state
       node->token1 = Token();
       node->token2 = Token();
       return node;
   }
   node->token1 = Token();
   node->token2 = Token();
   return node;
}


Node* vars2() { //<vars2> -> ; | := Integer ; <vars>
   Node* node = new Node("Vars2");
   if (currentToken.tokenInstance == ";") {
       node->token1 = currentToken; // Store ';' token
       getNextToken();
       // Set other tokens to empty state
       node->token2 = Token();
       node->token3 = Token();
   } else if (currentToken.tokenInstance == ":=") {
       node->token1 = currentToken; // Store ':=' token
       getNextToken();
       if (currentToken.tokenID == INT_tk) {
           node->token2 = currentToken; // Store Integer token
           getNextToken();
           if (currentToken.tokenInstance == ";") {
               node->token3 = currentToken; // Store ';' token
               getNextToken();
               node->child1 = vars();
           } else {
               std::cerr << "Error: Expected ;\n";
           printError();
               exit(1);
           }
       } else {
           std::cerr << "Error: Expected integer\n";
           printError();
           exit(1);
       }
   } else {
       std::cerr << "Error: Expected ; or :=\n";
       printError();
       exit(1);
   }
   return node;
}


Node* expr() { //<expr> -> <N> <expr'>
   Node* node = new Node("Expr");
   node->child1 = N();
   node->child2 = expr2();
   return node;
}


Node* expr2() { //<expr'> -> - <expr> | Îµ
   Node* node = new Node("Expr2");
   if (currentToken.tokenInstance == "-") {
       node->token1 = currentToken; // Store '-' token
       getNextToken();
       node->child1 = expr();
   } else {
       // Set token to empty state
       node->token1 = Token();
   }
   return node;
}


Node* N() { //<N> -> <A> <N'>
   Node* node = new Node("N");
   node->child1 = A();
   node->child2 = N2();
   return node;
}


Node* N2() { //<N'> -> / <A> <N'> | + <A> <N'> | Îµ
   Node* node = new Node("N2");
   if (currentToken.tokenInstance == "/" || currentToken.tokenInstance == "+") {
       node->token1 = currentToken; // Store '/' or '+' token
       getNextToken();
       node->child1 = A();
       node->child2 = N2();
   } else {
       // Set token to empty state
       node->token1 = Token();
   }
   return node;
}


Node* A() { //<A> -> <M> <A'>
   Node* node = new Node("A");
   node->child1 = M();
   node->child2 = A2();
   return node;
}


Node* A2() { //<A'> -> * <A> | Îµ
   Node* node = new Node("A2");
   if (currentToken.tokenInstance == "*") {
       node->token1 = currentToken; // Store '*' token
       getNextToken();
       node->child1 = A();
   } else {
       // Set token to empty state
       node->token1 = Token();
   }
   return node;
}


Node* M() { //<M> -> ^ <M> | <R>
   Node* node = new Node("M");
   if (currentToken.tokenInstance == "^") {
       node->token1 = currentToken; // Store '^' token
       getNextToken();
       node->child1 = M();
   } else {
       // Set token to empty state
       node->token1 = Token();
       node->child1 = R();
   }
   return node;
}


Node* R() { //<R> -> ( <expr> ) | Identifier | Integer
   Node* node = new Node("R");
   if (currentToken.tokenInstance == "(") {
       node->token1 = currentToken; // Store '(' token
       getNextToken();
       node->child1 = expr();
       if (currentToken.tokenInstance == ")") {
           node->token2 = currentToken; // Store ')' token
           getNextToken();
       } else {
           std::cerr << "Error: Expected ')'\n";
           printError();
           exit(1);
       }
   } else if (currentToken.tokenID == ID_tk || currentToken.tokenID == INT_tk) {
       node->token1 = currentToken; // Store Identifier or Integer token
       getNextToken();
   } else {
       std::cerr << "Error: Expected '(', Identifier, or Integer\n";
       printError();
       exit(1);
   }
   return node;
}


Node* stats() { //<stats> -> <stat> <mStat>
   Node* node = new Node("Stats");
   node->child1 = stat();
   node->child2 = mStat();
   return node;
}


Node* mStat() { //<mStat> -> empty | <stat> <mStat>
   Node* node = new Node("MStat");
   if (currentToken.tokenInstance == "cin" || currentToken.tokenInstance == "cout"
       || currentToken.tokenInstance == "{" || currentToken.tokenInstance == "if"
       || currentToken.tokenInstance == "while" || currentToken.tokenInstance == "repeat"
       || currentToken.tokenInstance == "set" || currentToken.tokenID == ID_tk
       || currentToken.tokenInstance == "jump" || currentToken.tokenInstance == "label"
       || currentToken.tokenInstance == "pick") {
       node->child1 = stat();
       node->child2 = mStat();
   } else {
       // Set token to empty state
       node->token1 = Token();
   }
   return node;
}


Node* stat() { //<stat> -> <in> ; | <out> ; | <block> | <if> ; | <loop1> ; | <loop2> ; | <assign> ; | <goto> ; | <label> ; | <pick> ;
   Node* node = new Node("Stat");
   if (currentToken.tokenInstance == "cin") {
       node->child1 = in();
       if (currentToken.tokenInstance == ";") {
           node->token1 = currentToken; // Store ';' token
           getNextToken();
       } else {
           std::cerr << "Error: Expected ;\n";
           printError();
           exit(1);
       }
   } else if (currentToken.tokenInstance == "cout") {
       node->child1 = out();
       if (currentToken.tokenInstance == ";") {
           node->token1 = currentToken; // Store ';' token
           getNextToken();
       } else {
           std::cerr << "Error: Expected ;\n";
           printError();
           exit(1);
       }
   } else if (currentToken.tokenInstance == "{") {
       node->child1 = block();
       // Set token to empty state
       node->token1 = Token();
   } else if (currentToken.tokenInstance == "if") {
       node->child1 = if_stat();
       if (currentToken.tokenInstance == ";") {
           node->token1 = currentToken; // Store ';' token
           getNextToken();
       } else {
           std::cerr << "Error: Expected ;\n";
           printError();
           exit(1);
       }
   } else if (currentToken.tokenInstance == "while") {
       node->child1 = loop1();
       if (currentToken.tokenInstance == ";") {
           node->token1 = currentToken; // Store ';' token
           getNextToken();
       } else {
           std::cerr << "Error: Expected ;\n";
           printError();
           exit(1);
       }
   } else if (currentToken.tokenInstance == "repeat") {
       node->child1 = loop2();
       if (currentToken.tokenInstance == ";") {
           node->token1 = currentToken; // Store ';' token
           getNextToken();
       } else {
           std::cerr << "Error: Expected ;\n";
           printError();
           exit(1);
       }
   } else if (currentToken.tokenInstance == "set" || currentToken.tokenID == ID_tk) {
       node->child1 = assign();
       if (currentToken.tokenInstance == ";") {
           node->token1 = currentToken; // Store ';' token
           getNextToken();
       } else {
           std::cerr << "Error: Expected ;\n";
           printError();
           exit(1);
       }
   } else if (currentToken.tokenInstance == "jump") {
       node->child1 = goto_stat();
       if (currentToken.tokenInstance == ";") {
           node->token1 = currentToken; // Store ';' token
           getNextToken();
       } else {
           std::cerr << "Error: Expected ;\n";
           printError();
           exit(1);
       }
   } else if (currentToken.tokenInstance == "label") {
       node->child1 = label();
       if (currentToken.tokenInstance == ";") {
           node->token1 = currentToken; // Store ';' token
           getNextToken();
       } else {
           std::cerr << "Error: Expected ;\n";
           printError();
           exit(1);
       }
   } else if (currentToken.tokenInstance == "pick") {
       node->child1 = pick();
       if (currentToken.tokenInstance == ";") {
           node->token1 = currentToken; // Store ';' token
           getNextToken();
       } else {
           std::cerr << "Error: Expected ;\n";
           printError();
           exit(1);
       }
   } else {
       std::cerr << "Error: Expected cin, cout, {, while, repeat, set, jump, label, or pick\n";
       printError();
       exit(1);
   }
   return node;
}


Node* in() { //<in> -> cin Identifier
   Node* node = new Node("In");
   if (currentToken.tokenInstance == "cin") {
       node->token1 = currentToken; // Store 'cin' token
       getNextToken();
       if (currentToken.tokenID == ID_tk) {
           node->token2 = currentToken; // Store 'Identifier' token
           getNextToken();
       } else {
           std::cerr << "Error: Expected identifier\n";
           printError();
           exit(1);
       }
   } else {
       std::cerr << "Error: Expected 'cin'\n";
           printError();
       exit(1);
   }
   return node;
}


Node* out() { //<out> -> cout <expr>
   Node* node = new Node("Out");
   if (currentToken.tokenInstance == "cout") {
       node->token1 = currentToken; // Store 'cout' token
       getNextToken();
       node->child1 = expr();
   } else {
       std::cerr << "Error: Expected 'cout'\n";
       printError();
       exit(1);
   }
   return node;
}


Node* if_stat() { //<if> -> if [ <expr> <RO> <expr> ] then <stat>
   Node* node = new Node("If");
   if (currentToken.tokenInstance == "if") {
       node->token1 = currentToken; // Store 'if' token
       getNextToken();
       if (currentToken.tokenInstance == "[") {
           node->token2 = currentToken; // Store '[' token
           getNextToken();
           node->child1 = expr();
           node->child2 = RO();
           node->child3 = expr();
           if (currentToken.tokenInstance == "]") {
               node->token3 = currentToken; // Store ']' token
               getNextToken();
               if (currentToken.tokenInstance == "then") {
                   node->token4 = currentToken; // Store 'then' token
                   getNextToken();
                   node->child4 = stat();
               } else {
                   std::cerr << "Error: Expected 'then'\n";
                   printError();
                   exit(1);
               }
           } else {
               std::cerr << "Error: Expected ']'\n";
               printError();
               exit(1);
           }
       } else {
           std::cerr << "Error: Expected '['\n";
           printError();
           exit(1);
       }
   } else {
       std::cerr << "Error: Expected 'if'\n";
       printError();
       exit(1);
   }
   return node;
}


Node* pick() { //<pick> -> pick <expr> <pickbody>
   Node* node = new Node("Pick");
   if (currentToken.tokenInstance == "pick") {
       node->token1 = currentToken; // Store 'pick' token
       getNextToken();
       node->child1 = expr();
       node->child2 = pickbody();
   } else {
       std::cerr << "Error: Expected 'pick'\n";
       printError();
       exit(1);
   }
   return node;
}


Node* pickbody() { //<pickbody> -> <stat> : <stat>
   Node* node = new Node("PickBody");
   node->child1 = stat();
   if (currentToken.tokenInstance == ":") {
       node->token1 = currentToken; // Store ':' token
       getNextToken();
       node->child2 = stat();
   } else {
       std::cerr << "Error: Expected ':'\n";
       printError();
       exit(1);
   }
   return node;
}


Node* loop1() { //<loop1> -> while [ <expr> <RO> <expr> ] <stat>
   Node* node = new Node("Loop1");
   if (currentToken.tokenInstance == "while") {
       node->token1 = currentToken; // Store 'while' token
       getNextToken();
       if (currentToken.tokenInstance == "[") {
           node->token2 = currentToken; // Store '[' token
           getNextToken();
           node->child1 = expr();
           node->child2 = RO();
           node->child3 = expr();
           if (currentToken.tokenInstance == "]") {
               node->token3 = currentToken; // Store ']' token
               getNextToken();
               node->child4 = stat();
           } else {
               std::cerr << "Error: Expected ']'\n";
               printError();
               exit(1);
           }
       } else {
           std::cerr << "Error: Expected '['\n";
           printError();
           exit(1);
       }
   } else {
       std::cerr << "Error: Expected 'while'\n";
       printError();
       exit(1);
   }
   return node;
}


Node* loop2() { //<loop2> -> repeat <stat> until [ <expr> <RO> <expr> ]
   Node* node = new Node("Loop2");
   if (currentToken.tokenInstance == "repeat") {
       node->token1 = currentToken; // Store 'repeat' token
       getNextToken();
       node->child1 = stat();
       if (currentToken.tokenInstance == "until") {
           node->token2 = currentToken; // Store 'until' token
           getNextToken();
           if (currentToken.tokenInstance == "[") {
               node->token3 = currentToken; // Store '[' token
               getNextToken();
               node->child2 = expr();
               node->child3 = RO();
               node->child4 = expr();
               if (currentToken.tokenInstance == "]") {
                   node->token4 = currentToken; // Store ']' token
                   getNextToken();
               } else {
                   std::cerr << "Error: Expected ']'\n";
                   printError();
                   exit(1);
               }
           } else {
               std::cerr << "Error: Expected '['\n";
               printError();
               exit(1);
           }
       } else {
           std::cerr << "Error: Expected 'until'\n";
           printError();
           exit(1);
       }
   } else {
       std::cerr << "Error: Expected 'repeat'\n";
       printError();
       exit(1);
   }
   return node;
}


Node* assign() { //<assign> -> set Identifier = <expr> | Identifier = <expr>
   Node* node = new Node("Assign");
   if (currentToken.tokenInstance == "set" || currentToken.tokenID == ID_tk) {
       if (currentToken.tokenInstance == "set") {
           node->token1 = currentToken; // Store 'set' token
           getNextToken();
       }
       if (currentToken.tokenID == ID_tk) {
           node->token2 = currentToken; // Store 'Identifier' token
           getNextToken();
           if (currentToken.tokenInstance == "=") {
               node->token3 = currentToken; // Store '=' token
               getNextToken();
               node->child1 = expr();
           } else {
               std::cerr << "Error: Expected '='\n";
               printError();
               exit(1);
           }
       } else {
           std::cerr << "Error: Expected Identifier\n";
           printError();
           exit(1);
       }
   } else {
       std::cerr << "Error: Expected 'set' or Identifier\n";
       printError();
       exit(1);
   }
   return node;
}


Node* RO() { //<RO> -> < | > | == | ... (three tokens here) | =!=
   Node* node = new Node("RO");
   if (currentToken.tokenInstance == "<" || currentToken.tokenInstance == ">" ||
       currentToken.tokenInstance == "==" || currentToken.tokenInstance == "=!=") {
       node->token1 = currentToken; // Store '<', '>', '==', or '=!=' token
       getNextToken();
   } else if (currentToken.tokenInstance == ".") {
       node->token1 = currentToken; // Store first '.' token
       getNextToken();
       if (currentToken.tokenInstance == ".") {
           node->token2 = currentToken; // Store second '.' token
           getNextToken();
           if (currentToken.tokenInstance == ".") {
               node->token3 = currentToken; // Store third '.' token
               getNextToken();
           } else {
               std::cerr << "Error: Expected '.'\n";
               printError();
               exit(1);
           }
       } else {
           std::cerr << "Error: Expected '.'\n";
           printError();
           exit(1);
       }
   } else {
       std::cerr << "Error: Expected '<', '>', '==', '...', or '=!='\n";
       printError();
       exit(1);
   }
   return node;
}


Node* label() { //<label> -> label Identifier
   Node* node = new Node("Label");
   if (currentToken.tokenInstance == "label") {
       node->token1 = currentToken; // Store 'label' token
       getNextToken();
       if (currentToken.tokenID == ID_tk) {
           node->token2 = currentToken; // Store 'Identifier' token
           getNextToken();
       } else {
           std::cerr << "Error: Expected Identifier\n";
           printError();
           exit(1);
       }
   } else {
       std::cerr << "Error: Expected 'label'\n";
       printError();
       exit(1);
   }
   return node;
}


Node* goto_stat() { //<goto> -> jump Identifier
   Node* node = new Node("Goto");
   if (currentToken.tokenInstance == "jump") {
       node->token1 = currentToken; // Store 'jump' token
       getNextToken();
       if (currentToken.tokenID == ID_tk) {
           node->token2 = currentToken; // Store 'Identifier' token
           getNextToken();
       } else {
           std::cerr << "Error: Expected Identifier\n";
           printError();
           exit(1);
       }
   } else {
       std::cerr << "Error: Expected 'jump'\n";
       printError();
       exit(1);
   }
   return node;
}


void printTree(Node* node, int depth = 0) {
   if (node == nullptr) {
       return;
   }


   for (int i = 0; i < depth; ++i) {
       std::cout << "  ";
   }


   std::cout << node->label;
   if (!node->token1.tokenInstance.empty()) {
       std::cout << " (" << node->token1.tokenInstance << ")";
   }
   if (!node->token2.tokenInstance.empty()) {
       std::cout << " (" << node->token2.tokenInstance << ")";
   }
   if (!node->token3.tokenInstance.empty()) {
       std::cout << " (" << node->token3.tokenInstance << ")";
   }
   if (!node->token4.tokenInstance.empty()) {
       std::cout << " (" << node->token4.tokenInstance << ")";
   }
   if (!node->token5.tokenInstance.empty()) {
       std::cout << " (" << node->token5.tokenInstance << ")";
   }
   std::cout << std::endl;


   printTree(node->child1, depth + 1);
   printTree(node->child2, depth + 1);
   printTree(node->child3, depth + 1);
   printTree(node->child4, depth + 1);
}


void printError() {
   std::cerr << "Error at line " << currentToken.lineNum << " character " << currentToken.charNum << ": " << currentToken.tokenInstance << "\n";
}


Node* parser(std::vector<Token> tks) {
   tokens = tks;
   getNextToken();


   Node* root = program();


   return root;
}
