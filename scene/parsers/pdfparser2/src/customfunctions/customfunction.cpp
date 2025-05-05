#include "customfunction.h"

namespace CustomFunction {
    std::vector<uint8_t>::iterator customsearch_end(std::vector<uint8_t>::iterator start, std::vector<uint8_t>::iterator end, char* str){
        size_t len = std::strlen(str);
        int* callback = new int[len];
        callback[len - 1]  = len;
        int j = len - 1,  k = len;
        while (j >= 0)
        {
            while ((k <= len - 1) && str[j] != str[k])
                k = callback[k];
            k--; j--;

            if (str[k] == str[j])
                callback[j] = callback[k];
            else
                callback[j] = k;
        }
        j = len - 1;
        while(start != end  && j > 0)
        {
            while (j < len && *end != str[j])
                j = callback[j];
            end--;
            j--;
        }
        //  pipiska

        if (j == 0)
        {
            return end + len - 1;
        }
        return start;
    }



    std::vector<uint8_t>::iterator customsearch_beg(std::vector<uint8_t>::iterator start, std::vector<uint8_t>::iterator end, char* str){
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
