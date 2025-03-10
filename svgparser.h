#ifndef SVGPARSER_H
#define SVGPARSER_H

#include <iostream>
#include <QFile>
#include <QtQml/qqmlregistration.h>
#include <QTextStream>
#include <QString>
#include <QByteArray>
#include <QDebug>
#include <QPoint>


struct  PathCommand;

class SvgParser
{
    QFile m_svgfile;
    QString m_path;
    QChar curchar;
    size_t bufsize = 0, bufpos = 0;
    QByteArray readbuffer{4096, 0};
    QVector<QString> bufferd;
    QChar next();

    void skipspaces();
    float readnumber();

public:
    QVector<QVector<PathCommand>> paths;

    SvgParser(QString path);
    void gettokens();
    void createcontrollpoints();
    void print();
};



struct PathCommand
{
    QChar type;
    QVector<QPointF> points;
};


class SvgParserController : public QObject
{
    Q_OBJECT
    QML_NAMED_ELEMENT (SvgControll)
    Q_PROPERTY(QString path READ path WRITE setpath NOTIFY pathChanged)
public:

    QString path() const { return m_path; }
    SvgParserController(QObject* parent = nullptr);
    void setpath(QString path)
    {
        if (path == m_path)
            return;

        m_path = path;
        emit pathChanged();
    }
signals:
    void pathChanged();
public slots:
    void createparser();

private:
    QString m_path;
};

#endif // SVGPARSER_H
