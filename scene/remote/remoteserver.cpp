#include "remoteserver.h"

#include <QTcpSocket>

#include "canvas.h"

RemoteServer::RemoteServer(QObject *parent)
    : QTcpServer(parent) {
    connect(this, &QTcpServer::newConnection,
            this, &RemoteServer::onNewConnection);
}

bool RemoteServer::enabled() {
    return m_enabled;
}

void RemoteServer::setEnabled(bool enabled) {
    if (enabled == m_enabled)
        return;

    if (enabled) {
        if (!listen())
            return;
    } else {
        close();
    }

    m_enabled = enabled;

    emit enabledChanged();
    emit portChanged();
}

Canvas *RemoteServer::canvas() const {
    return m_canvas;
}

void RemoteServer::setCanvas(Canvas *canvas) {
    if (m_canvas == canvas)
        return;

    if (m_canvas)
        m_canvas->removeEventFilter(this);

    if (canvas)
        canvas->installEventFilter(this);

    m_canvas = canvas;
    emit canvasChanged();
}

void RemoteServer::onNewConnection() {
    QTcpSocket *clientSocket = nextPendingConnection();
    qDebug() << "Client connected from" << clientSocket->peerAddress().toString();
    connect(clientSocket, &QTcpSocket::readyRead, this, [this, clientSocket]() {
        if (clientSocket->size() % 48 == 0 && m_canvas) {
            QDataStream payloadStream(clientSocket);
            while (!payloadStream.atEnd()) {
                QEvent::Type type;
                QPointF position, globalPosition;
                Qt::MouseButton button;
                Qt::MouseButtons buttons;
                Qt::KeyboardModifiers modifiers;
                payloadStream >> type >> position >> globalPosition >> button >> buttons >> modifiers;
                QMouseEvent *event = new QMouseEvent(type, position, globalPosition, button, buttons, modifiers);
                QCoreApplication::postEvent(m_canvas, event);
            }
        } else {
            QByteArray data = clientSocket->readAll();
            qDebug() << "Received:" << data << data.size();
        }
        // clientSocket->write("Echo: " + data);
    });
    connect(clientSocket, &QTcpSocket::disconnected, clientSocket, &QTcpSocket::deleteLater);
}

bool RemoteServer::eventFilter(QObject *watched, QEvent *event) {
    // switch (event->type()) {
    // case QEvent::MouseMove:
    // case QEvent::MouseButtonPress:
    // case QEvent::MouseButtonRelease:
    //     if (m_enabled)
    //         return true;
    //     break;
    // default:
    //     break;
    // }
    return false;
}

quint16 RemoteServer::port() {
    return serverPort();
}
