#pragma once
#include <vector>
#include <iterator>
#include <iostream>
#include <cstdint>

namespace CustomFunction {
std::vector<uint8_t>::iterator customsearch(std::vector<uint8_t>::iterator start, std::vector<uint8_t>::iterator end, char* str);
}
