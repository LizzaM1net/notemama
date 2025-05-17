#include "remoteclient.h"

#include <QEvent>
#include <QMouseEvent>

#include "canvas.h"

RemoteClient::RemoteClient(QObject *parent)
    : QObject(parent) {
    connect(&m_socket, &QAbstractSocket::stateChanged,
            this, &RemoteClient::socketStateChanged);
}

bool RemoteClient::enabled() const {
    return m_enabled;
}

void RemoteClient::setEnabled(bool newEnabled) {
    if (m_enabled == newEnabled)
        return;

    m_enabled = newEnabled;
    emit enabledChanged();

    tryConnect();
}

QUrl RemoteClient::url() const {
    return m_url;
}

void RemoteClient::setUrl(const QUrl &newUrl) {
    if (m_url == newUrl)
        return;

    m_url = newUrl;
    emit urlChanged();

    tryConnect();
}

Canvas *RemoteClient::canvas() const {
    return m_canvas;
}

void RemoteClient::setCanvas(Canvas *canvas) {
    if (m_canvas == canvas)
        return;

    if (m_canvas)
        m_canvas->removeEventFilter(this);

    if (canvas)
        canvas->installEventFilter(this);

    m_canvas = canvas;
    emit canvasChanged();
}

void RemoteClient::tryConnect() {
    if (m_socket.state() == QAbstractSocket::ConnectedState
        || m_socket.state() == QAbstractSocket::ConnectingState
        || m_socket.state() == QAbstractSocket::HostLookupState)
        m_socket.disconnectFromHost();

    if (!m_enabled || !m_url.isValid() || m_url.port() == -1)
        return;

    m_socket.connectToHost(m_url.host(), m_url.port());
}

void RemoteClient::readyRead() {
    QByteArray data = m_socket.readAll();
    qDebug() << Q_FUNC_INFO << data;
}

bool RemoteClient::connected() const {
    return m_connected;
}

bool RemoteClient::eventFilter(QObject *watched, QEvent *event) {
    switch (event->type()) {
    case QEvent::MouseMove:
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
        if (m_connected) {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            QByteArray payload;
            QDataStream payloadStream(&payload, QIODevice::ReadWrite);
            payloadStream << mouseEvent->type() << mouseEvent->position() << mouseEvent->globalPosition()
                          << mouseEvent->button() << mouseEvent->buttons() << mouseEvent->modifiers();
            m_socket.write(payload);
        }
        break;
    default:
        break;
    }

    return false;
}

void RemoteClient::socketStateChanged(QAbstractSocket::SocketState state) {
    bool connected = state == QAbstractSocket::ConnectedState;

    if (connected == m_connected)
        return;

    m_connected = connected;

    emit connectedChanged();
}
