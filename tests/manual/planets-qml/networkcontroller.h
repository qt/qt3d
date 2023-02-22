// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#ifndef NETWORKCONTROLLER_H
#define NETWORKCONTROLLER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

class NetworkController : public QObject
{
    Q_OBJECT

public:
    explicit NetworkController(QObject *parent = 0);

private:
    QTcpServer m_server;

private slots:
    void newConnection();
    void disconnected();
    void readyRead();

signals:
    void commandAccepted(QString command, QString value);
};

#endif // NETWORKCONTROLLER_H
