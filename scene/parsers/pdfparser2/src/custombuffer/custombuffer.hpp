#ifndef CUSTOMBUFFER_H
#define CUSTOMBUFFER_H

#define READ_CHUNK_SIZE 1024
#define READ_OFFSET_SIZE 128

#include <vector>
#include <string>

template<typename T>
class CustomVector{
   std::vector<T> data;
public:
    std::vector<T>::iterator important_iterator;

    CustomVector<T>() {}
    size_t size() const { return data.size(); }
    CustomVector<T>& operator+=(const std::vector<T>& other){
        size_t othersize = other.size();
        std::vector<T> buf(othersize + data.size());
        for (size_t i = 0; i  < othersize; ++i)
            buf[i] = other[i];
        for (size_t i = 0; i < data.size(); ++i)
            buf[othersize + i] = data[i];
        data = buf;
        return *this;
    }
    std::vector<T>::iterator getbegin() { return data.begin(); }
    std::vector<T>::iterator getend() { return data.end(); }
    T& operator[](size_t index){ return data[index]; }
    void nextline(){
        while(important_iterator != data.end() && *important_iterator !='\n') ++important_iterator;
        ++important_iterator;
    }
    long long getstartxreftable()
    {
        nextline();
        std::string str;
        while (*important_iterator >= 48 && *important_iterator <= 57) {
            str += *important_iterator++;
        }
        return std::stoll(str);
    }
};


#endif // CUSTOMBUFFER_H
