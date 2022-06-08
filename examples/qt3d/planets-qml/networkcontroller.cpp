// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include "networkcontroller.h"

NetworkController::NetworkController(QObject *parent) :
    QObject(parent)
{
    QObject::connect(&m_server, &QTcpServer::newConnection, this, &NetworkController::newConnection);

    if (!m_server.listen(QHostAddress::Any, 8080)) {
        qDebug() << "Failed to run http server";
    }
}

void NetworkController::newConnection()
{
    QTcpSocket *socket = m_server.nextPendingConnection();

    if (!socket)
        return;

    QObject::connect(socket, &QAbstractSocket::disconnected, this, &NetworkController::disconnected);
    QObject::connect(socket, &QIODevice::readyRead, this, &NetworkController::readyRead);
}

void NetworkController::disconnected()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());
    if (!socket)
        return;

    socket->disconnect();
    socket->deleteLater();
}

void NetworkController::readyRead()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());
    if (!socket || socket->state() == QTcpSocket::ClosingState)
        return;

    QString requestData = socket->readAll();
    QStringList list = requestData.split(' ');
    QString path = list[1];
    list = path.split('/');

    QByteArray reply;
    if (list.count() == 3) {
        socket->write("HTTP/1.1 200 OK\r\n");
        reply = QStringLiteral("Command accepted: %1 %2").arg(list[1], list[2]).toUtf8();
        emit commandAccepted(list[1], list[2]);
    } else {
        socket->write("HTTP/1.1 404 Not Found\r\n");
        reply = "Command rejected";
    }

    socket->write("Content-Type: text/plain\r\n");
    socket->write(QStringLiteral("Content-Length: %1\r\n").arg(reply.size()).toUtf8());
    socket->write("Connection: close\r\n");
    socket->write("\r\n");
    socket->write(reply);
    socket->disconnectFromHost();
}
