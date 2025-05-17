#ifndef REMOTECLIENT_H
#define REMOTECLIENT_H

#include <QObject>
#include <QQmlEngine>
#include <QTcpSocket>

class Canvas;

class RemoteClient : public QObject
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged FINAL)
    Q_PROPERTY(QUrl url READ url WRITE setUrl NOTIFY urlChanged FINAL)

    Q_PROPERTY(bool connected READ connected NOTIFY connectedChanged FINAL)

    Q_PROPERTY(Canvas *canvas READ canvas WRITE setCanvas NOTIFY canvasChanged FINAL)

public:
    explicit RemoteClient(QObject *parent = nullptr);

    bool enabled() const;
    void setEnabled(bool newEnabled);

    QUrl url() const;
    void setUrl(const QUrl &newUrl);

    bool connected() const;

    Canvas *canvas() const;
    void setCanvas(Canvas *canvas);

signals:
    void enabledChanged();
    void urlChanged();
    void connectedChanged();
    void canvasChanged();

private slots:
    void socketStateChanged(QAbstractSocket::SocketState state);

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    void tryConnect();

    void readyRead();

    bool m_enabled = true;
    QUrl m_url;

    bool m_connected = false;

    Canvas *m_canvas = nullptr;

    QTcpSocket m_socket;
};

#endif // REMOTECLIENT_H
