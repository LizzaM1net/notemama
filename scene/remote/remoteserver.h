#ifndef REMOTESERVER_H
#define REMOTESERVER_H

#include <QQmlEngine>
#include <QTcpServer>

class Canvas;

class RemoteServer : public QTcpServer
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(quint16 port READ port NOTIFY portChanged)

    Q_PROPERTY(Canvas *canvas READ canvas WRITE setCanvas NOTIFY canvasChanged FINAL)

public:
    explicit RemoteServer(QObject *parent = nullptr);

    bool enabled();
    void setEnabled(bool enabled);

    quint16 port();

    Canvas *canvas() const;
    void setCanvas(Canvas *canvas);

signals:
    void enabledChanged();
    void portChanged();
    void canvasChanged();

private slots:
    void onNewConnection();

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    bool m_enabled = false;

    Canvas *m_canvas = nullptr;
};

#endif // REMOTESERVER_H
