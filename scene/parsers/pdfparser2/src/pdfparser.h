#ifndef PDFPARSER_H
#define PDFPARSER_H


#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <iterator>
#include <utility>
#include <stack>

#include "items/sceneitem.h"

#include "items/spinnersceneitem.h"
#include "items/vectorpathsceneitem.h"
#include "items/pointsceneitem.h"

#include "zlib.h"

#include <QList>
#include <QVector2D>
#include <QDebug>

class PdfParser{
    std::string filename{""};
    std::vector<std::pair<long , long int>> refs;

    bool is_set_size{false};
    QVector2D page_size{0, 0};
    QVector2D dif{0, 0};


    // methods
    void setfilename(std::string filename);
    void createrefs(std::ifstream& file, long long pos);
    void slines(std::ifstream& file, std::string& buf, int count);
    std::string readnumber(std::string::iterator it, std::string::iterator end);
    QVector2D getpoint(std::stack<double> &st);
    void decompress(std::vector<uint8_t>& data);
    void transformation(std::vector<uint8_t>& decompressed_data, size_t sizearr);
    void parsfile();
public:

    PdfParser();
    QList<VectorPathSceneItem*> items;

    //methods
    void decompress_refs();
    void readfiles(std::vector<std::string> filenames);


};

#endif
