/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtQuick3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/
#include "qdownloadmanager.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QDebug>

QT_BEGIN_NAMESPACE

/*!
    \class QDownloadManager
    \brief The QDownloadManager class provides asset data download capability within the
    current thread.
    \since 4.8
    \ingroup qt3d
    \ingroup qt3d::network

    QDownloadManager extends the QAbstractDownloadManager class for use in a
    single-threaded Qt3D application. The QDownloadManager performs its network
    activities asynchronously, and processing will continue as normal while the download
    is underway (ie. the call is non-blocking).

    It should be noted that this does not preclude its usage in a multi-threaded
    application, rather that it does not itself spawn any threads of its own.

    In circumstances where an application will benefit greatly from multithreading,
    the developer may prefer to use the QThreadedDownloadManager.

    At the core of QDownloadManager is a QNetworkAccessManager which is
    shared by all instances of QDownloadManager.  The download manager itself
    provides all handling of network requests and replies internally, and
    allows for redirection of URLs and handling of errors without the need
    for the user to query network replies.

    To use the QDownloadManager simply create an instance of the class
    within your code, and connect QDownloadManager's downloadComplete() signal
    to a slot in your own code, then call the downloadAsset() function.

    For example

    \code
    QDownloadManager dlmanage;

    connect(this, SLOT(myReceiverSlot(QByteArray)), &dlmanage, SIGNAL(downloadComplete(QByteArray)));

    if (!dlmanage.downloadAsset(QUrl("www.my.image.url.net/image.jpg"))) {
        dWarning("the manager was unable to send the url request.");
    }
    \endcode

    The QDownloadManager will emit the downloadComplete() signal which contains
    all of the downloaded data in a QByteArray.  It is the user's responsibility
    to convert this data to the format they require, and to verify that it is
    correct.
*/

Q_GLOBAL_STATIC(QNetworkAccessManager, getNetworkAccessManager)

/*!
    Constructs a new instance of the QDownloadManager and attach it
    to a \a parent QObject.  Internally this initialises the
    QNetworkAccessManager instance which is shared by all instances of
    this class, and manages downloading of asset data.
*/
QDownloadManager::QDownloadManager(QObject *parent) : QAbstractDownloadManager(parent)
{
}


/*!
    Destroys the current instance of the QDownloadManager.  The destructor
    checks whether other instances of the class exist, and if none are found
    it deletes the internal QNetworkAccessManager instance.
*/
QDownloadManager::~QDownloadManager()
{
}

/*!
    Instructs the QDownloadManager to download the content specified
    in \a assetUrl.  This may be a texture, 3d model, or similar.

    A return value of true indicates that the network request was
    successfully queued for sending, while a return value of false
    indicates a problem with sending (possibly a poorly specified URL, or
    network failure).
*/
bool QDownloadManager::beginDownload(QUrl assetUrl)
{
    //URL Sanity check
    if ( ! assetUrl.isValid()) {
        qWarning() << "Invalid texture URL: " << assetUrl.toString();
        return false;
    }

    QNetworkRequest request(assetUrl);
    request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferCache);
    QNetworkReply *netReply = getNetworkAccessManager()->get(request);

    if (!netReply) {
        qWarning() << "Unable to send the request to the network.";
        return false;
    }

    connect(netReply, SIGNAL(finished()), this, SLOT(netReplyDone()));
    return true;
}

/*!
    \internal
    Internally used slot which is invoked when a download is finished.
    This handles redirection of URLs internally, as well as error checking.

    A successful download will emit the downloadComplete() signal which
    the user is responsible for handling in their own application.

    \sa downloadComplete()
*/
void QDownloadManager::netReplyDone()
{
    //Ensure sanity of the sender
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());

    if (!reply) {
        qWarning("DownloadManager's signal sender was not a QNetworkReply.");
        return;
    }

    if (reply->error() != QNetworkReply::NoError) {
        qWarning() << "Error in network reply: " << reply->url() << "(" << reply->errorString() << ")";
        reply->deleteLater();
        QByteArray tempArray;
        tempArray.clear();
        emit downloadComplete(tempArray);
        return;
    }

    //In the case of a reply which is a redirect
    QVariant redirection = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
    if (redirection.isValid()) {
        QUrl url = redirection.toUrl();
        //Check if we have a relative URL
        if (url.isRelative()) {
            url.setScheme(reply->url().scheme());
            url.setEncodedHost(reply->url().encodedHost());
        }

        //Reissue redirected request.
        QNetworkRequest request(url);
        if (getNetworkAccessManager())
        {
            QNetworkReply * netReply = getNetworkAccessManager()->get(request);
            connect(netReply, SIGNAL(finished()), SLOT(netReplyDone()));
            reply->deleteLater();
        } else {
            QByteArray tempArray;
            tempArray.clear();
            emit downloadComplete(tempArray);
        }

        return;
    }

    //In the case of just data being returned
    //qDebug() << "ContentType:" << reply->header(QNetworkRequest::ContentTypeHeader).toString();
    QByteArray assetData;
    assetData = reply->readAll();
    reply->deleteLater();

    emit downloadComplete(assetData);

}

QT_END_NAMESPACE
