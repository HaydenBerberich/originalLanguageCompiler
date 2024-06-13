#include "node.h"


Node::Node(std::string label) {
   this->label = label;
   this->child1 = nullptr;
   this->child2 = nullptr;
   this->child3 = nullptr;
   this->child4 = nullptr;
}


Node::~Node() {
   delete child1;
   delete child2;
   delete child3;
   delete child4;
}
