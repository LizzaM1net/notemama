#ifndef PDFPARSER_H
#define PDFPARSER_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstddef>
#include "pdfparsererror.h"
#include "customfunction.h"
#include "custombuffer.hpp"
#include "customtypespdf.h"

#include "scene/items/vectorpathsceneitem.h"
#include "scene/scene.h"

#include <QVector2D>
#include <QList>
#include <QtGlobal>
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QDebug>

#include "zlib.h"

struct Line;
class pdfparser;

class pdfparser{
    std::string filename{""};

    std::vector<uint8_t>::iterator iter;
    std::vector<uint8_t>::iterator end;


    void setfilename(std::string filename) { this->filename = filename; }
    int  decompress(std::vector<uint8_t>& data);
    void transformation(std::vector<uint8_t>& data, size_t sizearr);
    void parsfile();
    void readxreftable(std::ifstream& file, long long pos);
public:
    double whiledecompress = 0;
    double whileread = 0;
    double whilepars = 0;
	pdfparser(std::string filename = "");
	void readfiles(std::vector<std::string> files);
    QList<VectorPathSceneItem*> m_parseritem;
	~pdfparser();
};

#endif // PDFPARSER_H
