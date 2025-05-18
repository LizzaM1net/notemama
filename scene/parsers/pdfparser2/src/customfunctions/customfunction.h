#ifndef CUSTOMFUNCTION_H
#define CUSTOMFUNCTION_H


#include <vector>
#include <iterator>
#include <iostream>
#include <cstdint>
#include <string>

namespace CustomFunction {
    std::vector<uint8_t>::iterator customsearch_end(std::vector<uint8_t>::iterator start, std::vector<uint8_t>::iterator end, char* str);
    std::vector<uint8_t>::iterator customsearch_beg(std::vector<uint8_t>::iterator start, std::vector<uint8_t>::iterator end, char* str);

    template<typename Func>
    char stripTrailing(std::string& str, Func&& condition){
        std::string::iterator start = str.begin();
        std::string::iterator end = str.end();
        while (start != end && condition(*end)) --end;
        return *end;
    }
}

#endif //custumfunction
