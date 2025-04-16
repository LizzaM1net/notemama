#include "customfunction.h"
#include <cstring>

namespace CustomFunction {
    std::vector<uint8_t>::iterator customsearch(std::vector<uint8_t>::iterator start, std::vector<uint8_t>::iterator end, char* str){
        size_t sizestr = std::strlen(str);
        int* callback  = new int[sizestr];
        callback[0] = -1;
        int j = 0, k = -1;
        while (j < sizestr)
        {

            while (k >= 0 && str[j] != str[k])
                k = callback[k];
            j++; k++;
            if (str[j] == str[k])
                callback[j] = callback[k];
            else
                callback[j] = k;
        }
        j = 0;
        while (start != end && j < sizestr)
        {
            while (j >= 0 && *start != str[j])
                j = callback[j];
            start++;
            j++;
        }
        if (j == sizestr)
            return (start - sizestr);
        delete[] callback;
        return start;
    }
}
