#ifndef PDFPARSERERROR_H
#define PDFPARSERERROR_H

#include <exception>
#include <string>
#include <utility>

class PdfParserError : public std::exception{
    std::string msg;

    public:
    PdfParserError(std::string msg) noexcept
        : msg(msg) {}
    virtual ~PdfParserError() {}
    const char* what() const noexcept { return msg.data();}
    static std::string convert_zlib_error(int& zlib_error_code);
};
#endif // PDFPARSERERROR_H
