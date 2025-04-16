#include "customtypespdf.h"

DrawingType::DrawingType(DrawingLineType type) : m_type(type) {}
Move::Move(DrawingLineType type, QVector2D point) : DrawingType(type), m_point(point) {}
Quit::Quit(DrawingLineType type) : DrawingType(type) {}
Line::Line(DrawingLineType type, QVector2D point) : DrawingType(type), m_point(point) {}
CubicBezier::CubicBezier(DrawingLineType type, QList<QVector2D> points) : DrawingType(type), m_points(points) {}
CMatrix::CMatrix(DrawingLineType type, QList<double> matrix) : DrawingType(type), m_matrix(matrix) {}

void CMatrix::print()
{
    qDebug() << m_type << " " <<  m_matrix;
}

void Move::print()
{
    qDebug() << m_type << " " << m_point;
}

void Line::print()
{
    qDebug() << m_type << " " << m_point;
}

void CubicBezier::print()
{
    qDebug() << m_type << " " << m_points;
}

void Quit::print()
{
    qDebug() << m_type;
}
