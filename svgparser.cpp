#include "svgparser.h"

SvgParser::SvgParser(QString path)
    :m_svgfile(path), m_path(path)
{
    if (!m_svgfile.open(QIODevice::ReadOnly))
    {
        std::cout << "error" << std::endl;
    }
    curchar = next();
}


float SvgParser::readnumber()
{
    skipspaces();
    QString num;
    while(curchar.isDigit() || curchar == '-' || curchar == '.')
    {
        num += curchar;
        curchar = next();
    }


    if (num.isEmpty())
        return 0.0f;


    float result = num.toFloat();

    if (curchar == ',') {
        curchar = next();
    }

    return result;
}



void SvgParser::gettokens()
{
    while (!curchar.isNull())
    {
        skipspaces();

        if (curchar == '<')
        {
            curchar = next();

            if (curchar == '/')
            {
                while (curchar.isLetter() || curchar == ':') curchar = next();
            }
            else
            {
                QString tagName;
                tagName.reserve(10);

                while (curchar.isLetter() || curchar == ':')
                {
                    tagName += curchar;
                    curchar = next();
                }

                if (tagName == "path")
                {
                    skipspaces();

                    while (curchar != '>' && curchar != '/' && !curchar.isNull())
                    {
                        QString attstring;
                        attstring.reserve(10);

                        while (curchar.isLetter() || curchar == '-' || curchar == '.' || curchar == ':')
                        {
                            attstring += curchar;
                            curchar = next();
                        }

                        skipspaces();

                        if (curchar == '=')
                        {
                            curchar = next();
                            skipspaces();
                        }

                        if (attstring == "d")
                        {
                            if (curchar == '"')
                            {
                                curchar = next();
                                QVector<PathCommand> path;
                                QChar lastCommand;
                                float num1 = readnumber();
                                float num2 = readnumber();

                                while (curchar != '"' && !curchar.isNull())
                                {
                                    if (curchar.isLetter())
                                    {
                                        lastCommand = curchar;
                                        path.push_back({lastCommand, {}});
                                        curchar = next();
                                    }
                                    else if (curchar.isDigit() || curchar == '-' || curchar == '.')
                                    {

                                        if (lastCommand == 'm' && !path.isEmpty() && path.last().points.size() != 0)
                                        {
                                            lastCommand = 'l';
                                            path.push_back({lastCommand, {}});
                                        }
                                        else if (lastCommand == 'c' && !path.isEmpty() && path.last().points.size() == 3)
                                        {
                                            lastCommand = 'l';
                                            path.push_back({lastCommand, {}});
                                        }

                                        if (lastCommand.toLower() == 'h' || lastCommand.toLower() == 'v')
                                        {
                                            num1 = readnumber();
                                        }
                                        else
                                        {
                                            num1 = readnumber();
                                            num2 = readnumber();
                                        }

                                        if (!path.isEmpty())
                                        {
                                            if (lastCommand.toLower() == 'h')
                                            {
                                                path.last().points.push_back({num1, 0.0f});
                                            }
                                            else if (lastCommand.toLower() == 'v')
                                            {
                                                path.last().points.push_back({0.0f, num1});
                                            }
                                            else
                                            {
                                                path.last().points.push_back({num1, num2});
                                            }
                                        }
                                    }
                                    else
                                    {
                                        curchar = next();
                                    }
                                }
                                if (!curchar.isNull()) curchar = next();
                                paths.push_back(path);
                            }
                        }
                        else
                        {
                            if (curchar == '"')
                            {
                                curchar = next();
                                while (curchar != '"' && !curchar.isNull()) curchar = next();
                                if (!curchar.isNull()) curchar = next();
                            }
                        }

                        skipspaces();
                    }

                    if (curchar == '/')
                    {
                        curchar = next();
                        if (curchar == '>') curchar = next();
                    }
                    else if (curchar == '>')
                    {
                        curchar = next();
                    }
                }
            }
        }
        else if (curchar.isLetter() || curchar == '.' || curchar == '-')
        {
            while (curchar.isLetter() || curchar == '.' || curchar == '-') curchar = next();
        }
        else
        {
            curchar = next();
        }
    }

    m_svgfile.close();
    qDebug() << "finished";
}



QChar SvgParser::next()
{
    if (bufpos >= bufsize)
    {
        bufsize = m_svgfile.read(readbuffer.data(), readbuffer.size());
        bufpos = 0;

        if (bufsize == 0) {
            return '\0';
        }
    }

    return readbuffer[bufpos++];

}


void SvgParser::skipspaces()
{
    while (curchar.isSpace()) {
        curchar = next();
    }
}

SvgParserController::SvgParserController(QObject *parent)
    : QObject(parent)
{
    QObject::connect(this, &SvgParserController::pathChanged, this, &SvgParserController::createparser);
}

void SvgParser::print()

{
    for (const auto& path : paths)
    {
        for (const auto& line : path)
        {
            qDebug() << line.type;
            for (const auto& point : line.points){

                qDebug() << point << " " << point.isNull();
            }
        }

        qDebug() << "End of path";
    }

}

void SvgParserController::createparser()
{
    SvgParser pars(m_path);
    pars.gettokens();
    pars.print();

}




