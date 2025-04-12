#ifndef TOOL_H
#define TOOL_H

#include <QObject>

class Canvas;
class QVector2D;

class Tool : QObject {
    Q_OBJECT

    Q_PROPERTY(QString name READ name CONSTANT)

public:
    Tool(Canvas *canvas);

    virtual QString name();

    virtual void mousePress(QVector2D position) = 0;
    virtual void mouseMove(QVector2D position) = 0;
    virtual void mouseRelease() = 0;

protected:
    Canvas *m_canvas = nullptr;
};

#endif // TOOL_H
