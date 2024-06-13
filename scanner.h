#pragma once
#include <string>
#include <vector>
#include "token.h"


Token scanner(std::istream& input, std::string& line, int& lineNum);
void testScanner(std::istream& input, std::string& line, int& lineNum);
