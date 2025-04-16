#ifndef CUSTOMTYPESPDF_H
#define CUSTOMTYPESPDF_H

#include <QVector2D>
#include <QList>
#include <QtGlobal>
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QDebug>

enum DrawingLineType{
    LineTo = 'l',
    CubicBezierTo = 'c',
    MoveTo = 'm',
    QuitTo = 'q',
    CMatrixTransfrom= 208,
    NoTypeTo = 1123
};

class DrawingType{
protected:
    DrawingLineType m_type;
public:
    DrawingType(DrawingLineType type);
    virtual DrawingLineType getType() const = 0;
    virtual void print() = 0;
    virtual ~DrawingType() = default;
};

class CMatrix : public DrawingType{
    QList<double> m_matrix;
    public:
    CMatrix(DrawingLineType type, QList<double> matrix);
    virtual DrawingLineType getType() const override { return m_type; }
    virtual void print() override;
    virtual ~CMatrix() = default;
};

class Move : public DrawingType{
public:
    QVector2D m_point;
    Move(DrawingLineType type, QVector2D point);
    virtual DrawingLineType getType() const override { return m_type; }
    virtual void print() override;
    virtual ~Move() = default;
};

class Line : public DrawingType{
public:
    QVector2D m_point;
    Line(DrawingLineType type, QVector2D point);
    virtual DrawingLineType getType() const override { return m_type; }
    virtual void print() override;
    virtual ~Line() = default;
};

class CubicBezier : public DrawingType{
    QList<QVector2D> m_points;
public:
    CubicBezier(DrawingLineType type, QList<QVector2D> points);
    virtual DrawingLineType getType() const override { return m_type; }
    virtual void print() override;
    virtual ~CubicBezier() = default;
};

class Quit : public DrawingType{
public:
    Quit(DrawingLineType type);
    virtual DrawingLineType getType() const override {return m_type;}
    virtual void print() override;
    virtual ~Quit() = default;
};

#endif // CUSTOMTYPESPDF_H
