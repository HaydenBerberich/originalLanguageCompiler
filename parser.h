#pragma once
#include "token.h"
#include "node.h"


Node* parser(std::vector<Token> tokens);
Node* program();
Node* func();
Node* block();
Node* vars();
Node* vars2();
Node* expr();
Node* expr2();
Node* N();
Node* N2();
Node* A();
Node* A2();
Node* M();
Node* R();
Node* stats();
Node* mStat();
Node* stat();
Node* in();
Node* out();
Node* if_stat();
Node* pick();
Node* pickbody();
Node* loop1();
Node* loop2();
Node* assign();
Node* RO();
Node* label();
Node* goto_stat();
void printTree(Node* node, int depth);
void printError();
