 #include "pdfparser.h"

#include <stack>

pdfparser::pdfparser(std::string filename)
    : filename(filename)
{
}

void pdfparser::readxreftable(std::ifstream& file, long long pos)
{
    file.seekg(pos, std::ios::beg);

    std::string buf;
    std::getline(file, buf);
    size_t spos = buf.size() - 1;
    std::streampos prevpos = 0;

    while (buf[0] != 't')
    {

        while(spos > 0 &&  (buf[spos] >= 0 && buf[spos] <= 32)) --spos;
        if (buf[spos] == 'n')
        {
            std::string num;
            for (auto& c : buf)
            {
                if (c == ' ') break;
                num += c;
            }
            prevpos = file.tellg();
            file.seekg(std::stoll(num), std::ios::beg);
            std::string ff;
            std::getline(file, ff);
            std::getline(file, ff);

            if (ff.find("FlateDecode") != std::string::npos)
            {
                size_t it = ff.find("Length ");
                if (it != std::string::npos)
                {
                    it+=7;
                    std::string length;
                    while (ff[it] >= 48 && ff[it] <= 57) length += ff[it++];
                    std::getline(file, ff);
                    std::vector<uint8_t> data(std::stol(length));
                    file.read(reinterpret_cast<char*>(data.data()), data.size());
                   decompress(data);
                }
            }
            file.seekg(prevpos, std::ios::beg);
        }
        std::getline(file, buf);
        spos = buf.size() - 1;
    }
}

void pdfparser::parsfile() {
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file");
    }
    std::streampos search_pos = file.tellg();
    std::vector<uint8_t> buffer(READ_CHUNK_SIZE);
    CustomVector<uint8_t> customBuffer;
    do{
        search_pos -= READ_CHUNK_SIZE;
        file.seekg(search_pos);
        file.read(reinterpret_cast<char*>(buffer.data()), READ_CHUNK_SIZE);
        customBuffer += buffer;
        customBuffer.important_iterator = CustomFunction::customsearch(customBuffer.getbegin(), customBuffer.getend(), "startxref");
        if (customBuffer.important_iterator != customBuffer.getend()) break;
    } while (search_pos > 0);
    readxreftable(file, customBuffer.getstartxreftable());
}

int pdfparser::decompress(std::vector<uint8_t>& data)
{
    auto sstart = std::chrono::high_resolution_clock::now();
    z_stream stream;
    stream.zalloc = Z_NULL;
    stream.zfree = Z_NULL;
    stream.opaque = Z_NULL;
    if (inflateInit(&stream) != Z_OK)
        throw std::runtime_error("Failed to initialize zlib");

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
            throw  PdfParserError(PdfParserError::convert_zlib_error(ret) + " " + filename);
    } while (ret != Z_STREAM_END);
    auto send = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> sduration = send - sstart;
    whiledecompress += sduration.count();
    // std::cout << decompressed_data.size() << std::endl;
    auto start = std::chrono::high_resolution_clock::now();

    std::ofstream file("out.txt", std::ios::app);
    file << std::string(decompressed_data.begin(), decompressed_data.end());
    file.close();
    transformation(decompressed_data, stream.total_out); // return 0;
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    whilepars+=  duration.count();
    inflateEnd(&stream);
    return ret;
}

void pdfparser::transformation(std::vector<uint8_t>& decompressed_data, size_t sizearr)
{
    iter = decompressed_data.begin();
    end = decompressed_data.begin() + sizearr - 1;
    std::stack<long double> stack;
    QVector2D cur_point{0,0};
    if (m_parseritem.size() == 0)
    {
        m_parseritem << new VectorPathSceneItem{cur_point, {}};
    }

    std::string num = "";

    while (iter != end)
    {
        if (*iter >= 48  && *iter <= 57){
            while (*iter != ' ' && iter != end)
            {
                num += *iter++;
            }
            stack.push(std::stold(num));
            num = "";
        }else if (*iter == 'c' && *(iter + 1) == 'm' && *(iter - 1) == ' '){
            QList<double> list(6);
            for (int i = 5; i >= 0; --i)
            {
                list[i] = stack.top();
                stack.pop();
            }
            iter+=2;
        }else if (*iter == 'm' && iter != decompressed_data.begin() && *(iter-1) == ' ' && !(*(iter+1) >= 97 &&   *(iter+1) <= 122))
        {
            cur_point.setY(860 - stack.top());
            stack.pop();
            cur_point.setX(stack.top());
            stack.pop();
            m_parseritem << new VectorPathSceneItem{cur_point, {}};
        }else if(*iter == 'c' && *(iter-1) == ' ' && !(*(iter+1) >= 97 &&   *(iter+1) <= 122) && stack.size() >= 6)
        {
            QList<QVector2D> list(3);
            for (int i = 2; i >= 0; --i)
            {

                QVector2D bufpoint;
                list[i].setY(860 - stack.top());
                stack.pop();
                list[i].setX(stack.top());
                stack.pop();
            }


            for (int i = 0; i < 3; ++i)
            {
                QVector2D buf = list[i];
                list[i] = list[i] - cur_point;
                cur_point = buf;
            }

            m_parseritem.last()->segments << new VectorPath::CubicCurveSegment{list[0], list[1], list[2]};

        }else if(*iter == 'l' && *(iter-1) == ' ' && !(*(iter+1) >= 97 &&   *(iter+1) <= 122) && stack.size() >= 2)
        {
            QVector2D point;
            point.setY(860 - stack.top());
            stack.pop();
            point.setX(stack.top());
            stack.pop();
            m_parseritem.last()->segments << new VectorPath::LineSegment{point - cur_point};
            cur_point = point;
        }else if (*iter == 'q' && *(iter - 1) == ' ')
        {
        }
        if (iter != end) ++iter;
    }
}

void pdfparser::readfiles(std::vector<std::string> files)
{


    for (const auto file : files)
    {
        try{
            setfilename(file);
            parsfile();
        } catch(...) {}
    }
}
pdfparser::~pdfparser() {

}
