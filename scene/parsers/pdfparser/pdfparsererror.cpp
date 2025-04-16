#include "pdfparsererror.h"



 std::string PdfParserError::convert_zlib_error(int& zlib_error_code){
    switch (zlib_error_code) {
        case 0:
            return "Z_OK";
        case 1:
            return "Z_STREAM_END";
        case 2:
            return "Z_NEED_DICT";
        case -1:
            return "Z_ERRNO";
        case -2:
            return "Z_STREAM_ERROR";
        case -3:
            return "Z_DATA_ERROR";
        case -4:
            return "Z_MEM_ERROR";
        case -5:
            return "Z_BUF_ERROR";
        case -6:
            return "Z_VERSION_ERROR";
        default:
            return "UNKNOWN_ERROR";
    }
}
