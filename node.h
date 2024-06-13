#pragma once
#include <string>
#include "token.h"


class Node {
public:
   std::string label;
   Token token1;
   Token token2;
   Token token3;
   Token token4;
   Token token5;
   Node* child1;
   Node* child2;
   Node* child3;
   Node* child4;


   Node(std::string label);
   ~Node();
};
