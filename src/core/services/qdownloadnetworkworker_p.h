// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DCORE_QDOWNLOADNETWORKWORKER_P_H
#define QT3DCORE_QDOWNLOADNETWORKWORKER_P_H

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

#include <QSharedPointer>
#include <QObject>
#include <QUrl>
#include <Qt3DCore/private/qdownloadhelperservice_p.h>
#include <QMutex>

QT_BEGIN_NAMESPACE

class QThread;
class QNetworkAccessManager;
class QNetworkReply;

namespace Qt3DCore {

class QDownloadRequest;
typedef QSharedPointer<QDownloadRequest> QDownloadRequestPtr;

class QDownloadNetworkWorker : public QObject
{
    Q_OBJECT
public:
    QDownloadNetworkWorker(QObject *parent = nullptr);

signals:
    void submitRequest(const Qt3DCore::QDownloadRequestPtr &request);
    void cancelRequest(const Qt3DCore::QDownloadRequestPtr &request);
    void cancelAllRequests();

    void requestDownloaded(const Qt3DCore::QDownloadRequestPtr &request);

private Q_SLOTS:
    void onRequestSubmited(const Qt3DCore::QDownloadRequestPtr &request);
    void onRequestCancelled(const Qt3DCore::QDownloadRequestPtr &request);
    void onAllRequestsCancelled();
    void onRequestFinished(QNetworkReply *reply);
    void onDownloadProgressed(qint64 bytesReceived, qint64 bytesTotal);

private:
    QNetworkAccessManager *m_networkManager;
    QList< QPair<QDownloadRequestPtr, QNetworkReply *> > m_requests;
    QMutex m_mutex;
};

} // namespace Qt3DCore

QT_END_NAMESPACE

#endif // QT3DCORE_QDOWNLOADNETWORKWORKER_P_H
