#ifndef CANVAS_H
#define CANVAS_H

#include <QQuickRhiItem>
#include <QSGRendererInterface>
#include <rhi/qrhi.h>
#include <QQuickWindow>

class Canvas;

struct LineUpdate {
    int index;
    int updateStart;
    int updateEnd;
};

class Canvas : public QQuickRhiItem {
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(double lastCompletedTime READ lastCompletedTime NOTIFY lastCompletedTimeChanged FINAL)
    Q_PROPERTY(QString graphicsApi READ graphicsApi NOTIFY graphicsApiChanged FINAL)
    Q_PROPERTY(InputMode inputMode READ inputMode WRITE setInputMode NOTIFY inputModeChanged FINAL)

    friend class CanvasRenderer;

public:
    enum InputMode {
        Raw,
        Lines,
    }; Q_ENUM(InputMode)

public:
    Canvas();

    double lastCompletedTime() const;

    QString graphicsApi() const;

    InputMode inputMode() const;
    void setInputMode(InputMode newInputMode);

signals:
    void lastCompletedTimeChanged();

    void graphicsApiChanged();

    void inputModeChanged();

private slots:
    void setLastCompletedTime(double newLastCompletedTime);

    void windowChanged(QQuickWindow *window);

protected:
    QQuickRhiItemRenderer *createRenderer();

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

private:
    InputMode m_inputMode = Raw;

    QList<QList<QPointF>> m_lines;
    QList<LineUpdate> m_lineUpdates;

    bool m_pressed = false;

    double m_lastCompletedTime = 0;

    QSGRendererInterface::GraphicsApi m_graphicsApi = QSGRendererInterface::GraphicsApi::Null;
};

#endif // CANVAS_H
