#ifndef TOOL_H
#define TOOL_H

#include <QObject>
#include <qqmlintegration.h>

class Canvas;
class QVector2D;

class Tool : public QObject {
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("unless...")

    Q_PROPERTY(QString name READ name CONSTANT)
    Q_PROPERTY(QVariantList properties READ properties CONSTANT)

public:
    Tool(Canvas *canvas);

    virtual QString name();
    virtual QVariantList properties();

    virtual void mousePress(QVector2D position) = 0;
    virtual void mouseMove(QVector2D position) = 0;
    virtual void mouseRelease() = 0;

    virtual void toolActivated() {}
    virtual void toolDeactivated() {}

protected:
    Canvas *m_canvas = nullptr;
};

#endif // TOOL_H
