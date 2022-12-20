// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include <Qt3DCore/qt3dcore-config.h>

#ifndef QT3DCORE_DEBUG_ASPECTCOMMANDDEBUGGER_H
#define QT3DCORE_DEBUG_ASPECTCOMMANDDEBUGGER_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of other Qt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QTcpServer>
#include <Qt3DCore/private/qt3dcore_global_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

class QSystemInformationService;

namespace Debug {

class AsynchronousCommandReply;

class Q_3DCORE_PRIVATE_EXPORT AspectCommandDebugger : public QTcpServer
{
    Q_OBJECT
public:
    explicit AspectCommandDebugger(QSystemInformationService *parent = nullptr);

    void initialize();

    struct Q_3DCORE_PRIVATE_EXPORT ReadBuffer {
        QByteArray buffer;
        qsizetype startIdx = 0;
        qsizetype endIdx = 0;

        inline qsizetype size() const { return endIdx - startIdx; }
        void insert(const QByteArray &array);
        void trim();
    };

private Q_SLOTS:
    void asynchronousReplyFinished(AsynchronousCommandReply *reply);

private:
    void sendReply(QTcpSocket *socket, const QByteArray &data);
    void onCommandReceived(QTcpSocket *socket);
    void executeCommand(const QString &command, QTcpSocket *socket);

    QList<QTcpSocket *> m_connections;
    QSystemInformationService *m_service;

    ReadBuffer m_readBuffer;
    QHash<AsynchronousCommandReply *, QTcpSocket *> m_asyncCommandToSocketEntries;
};

} // Debug

} // Qt3DCore

QT_END_NAMESPACE

#endif // QT3DCORE_DEBUG_ASPECTCOMMANDDEBUGGER_H
