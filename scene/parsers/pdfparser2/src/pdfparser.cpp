#include "pdfparser.h"
#include "custombuffer/custombuffer.hpp"
#include "customfunctions/customfunction.h"

PdfParser::PdfParser() {}

void PdfParser::setfilename(std::string filename)
{
    this->filename  = filename;
}

void PdfParser::readfiles(std::vector<std::string> filenames)
{
    for (const std::string& filename : filenames)
    {
        setfilename(filename);
        parsfile();
    }
}

void PdfParser::slines(std::ifstream& file, std::string& buf, int count)
{
    for (int i = 0; i < count; ++i)
        std::getline(file, buf);
}


QVector2D PdfParser::getpoint(std::stack<double> &st)
{
    QVector2D point;
    point.setY(st.top()); st.pop();
    point.setX(st.top()); st.pop();
    return point;
}


std::string PdfParser::readnumber(std::string::iterator it, std::string::iterator end)
{
    std::string str_buf = "";
    while (it != end && (*it >= '0' && *it <= '9')){
        str_buf += *it;
        ++it;
    }
    return str_buf;
}


void PdfParser::createrefs(std::ifstream& file, long long ref)
{
    file.seekg(ref, std::ios::beg);
    std::string buf = ""; std::streampos prevpos = 0;
    slines(file, buf, 1);

    while(buf[0] != 't')
    {
        slines(file, buf, 1);
        if (CustomFunction::stripTrailing(buf, [](char c){return !(c >= 97 && c <= 122);}) == 'n')
        {
            prevpos = file.tellg();
            file.seekg(std::stoi(readnumber(buf.begin(), buf.end())), std::ios::beg);
            slines(file, buf, 2);


            if (!is_set_size)
            {
                long pos  = buf.find("MediaBox ");
                if (pos != std::string::npos)
                {
                    std::string::iterator ptr =  buf.begin() + pos + 11;
                    std::stack<double> s{};
                    while (ptr != buf.end() && *ptr  != ']')
                    {
                        if (*ptr >= '0' && *ptr <= '9')
                        {
                            std::string num = "";
                            while(*ptr != ' ' && *ptr != ']')
                                num += *(ptr++);
                            s.push(std::stod(num));
                        } else if (*ptr != ']') ++ptr;
                    }
                    page_size = getpoint(s);
                    qDebug() << page_size;

                    is_set_size = true;
                }
            }

            if (buf.find("FlateDecode") != std::string::npos)
            {
                long int it = buf.find("Length ");

                if (it != std::string::npos && buf.find("Image") == std::string::npos)
                {
                    it+=7;
                    long long refp = file.tellg();
                    std::pair<long long, long int> ref{refp, std::stoll(readnumber(buf.begin() + it, buf.end()))};
                    refs.push_back(ref);
                }
            }
            file.seekg(prevpos, std::ios::beg);
        }
    }

    slines(file, buf, 1);
    long long ptr = buf.find("Prev ");
    if (ptr != std::string::npos)
    {
        std::string::iterator pt = buf.begin() + ptr + 5;
        createrefs(file, std::stoi(readnumber(pt, buf.end())));
    }
}

void PdfParser::decompress(std::vector<uint8_t>& data)
{
    z_stream stream;
    stream.zalloc = Z_NULL;
    stream.zfree = Z_NULL;
    stream.opaque = Z_NULL;
    if (inflateInit(&stream) != Z_OK)
        return;

    size_t CHUNK_SIZE = data.size()*4;
    std::vector<unsigned char> decompressed_data(data.size());
    stream.next_in = const_cast<unsigned char*>(data.data());
    stream.avail_in = data.size();
    int ret;

    do{
        decompressed_data.resize(CHUNK_SIZE + stream.total_out);
        stream.next_out = decompressed_data.data() + stream.total_out;
        stream.avail_out = CHUNK_SIZE;
        ret = inflate(&stream, Z_NO_FLUSH);
        if (ret != Z_OK && ret != Z_STREAM_END)
            return;
    } while (ret != Z_STREAM_END);


    std::vector<uint8_t>::iterator it = decompressed_data.begin();

    if (CustomFunction::customsearch_beg(it, it + 100, "begincmap") != it + 100)
        return;

    transformation(decompressed_data, stream.total_out);
    inflateEnd(&stream);
}

void PdfParser::decompress_refs()
{
    for (int i =  refs.size() - 1; i >= 0; --i)
    {
        std::ifstream file(filename, std::ios::binary | std::ios::ate);
        file.seekg(refs[i].first, std::ios::beg);
        std::string l;
        std::getline(file, l);
        std::vector<uint8_t> data(refs[i].second);
        file.read(reinterpret_cast<char*>(data.data()), data.size());
        decompress(data);
    }
}

void PdfParser::transformation(std::vector<uint8_t>& decompressed_data, size_t sizearr)
{
    std::vector<uint8_t>::iterator iter = decompressed_data.begin();
    std::vector<uint8_t>::iterator end = decompressed_data.begin() + sizearr - 1;
    std::stack<double> stack;
    std::string num = "";

    while (iter != end)
    {
        if (*iter >= '0' && *iter <= '9'){
            while (iter != end && ((*iter >= '0' && *iter <= '9') || *iter == '.'))
            {
                num += *iter++;
            }
            stack.push(std::stold(num));
            num = "";
        }else if (*iter == 'c' && *(iter + 1) == 'm' && *(iter - 1) == ' '){
            iter+=2;
            while(!stack.empty()) stack.pop();
        }else if (*iter == 'm' && stack.size() >= 2)
        {
            items << new VectorPathSceneItem(getpoint(stack) + dif, {});
        }else if(*iter == 'c' && *(iter-1) == ' ' && stack.size() >= 6)
        {
            QVector2D p3 = getpoint(stack);
            QVector2D p2 = getpoint(stack);
            QVector2D p1 = getpoint(stack);
            if (!items.isEmpty()) items.last()->segments << new VectorPath::CubicCurveSegment{p1 + dif, p2 + dif, p3 + dif};
            while(!stack.empty()) stack.pop();
        }else if(*iter == 'l' && *(iter-1) == ' ' && stack.size() >= 2)
        {
            // if (!items.isEmpty()) items.last()->segments << new VectorPath::LineSegment{getpoint(stack) + dif};
        }else if (*iter == 'q')
        {
            while(!stack.empty()) stack.pop();
        }
        if (iter != end) ++iter;
    }
}

void PdfParser::parsfile()
{
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file.is_open())
    {
        return;
    }

    std::streampos search_pos = file.tellg();
    std::vector<uint8_t> buffer(READ_CHUNK_SIZE);
    CustomVector<uint8_t> customBuffer;

    do{
        search_pos -= READ_CHUNK_SIZE;
        file.seekg(search_pos, std::ios::beg);
        file.read(reinterpret_cast<char*>(buffer.data()), READ_CHUNK_SIZE);
        customBuffer += buffer;
        customBuffer.important_iterator = CustomFunction::customsearch_beg(customBuffer.getbegin(), customBuffer.getend(), "startxref");
    }while (search_pos > 0 && customBuffer.important_iterator == customBuffer.getend());
    createrefs(file, customBuffer.getstartxreftable());
}
