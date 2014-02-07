/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
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
#include <QUrl>

QT_BEGIN_NAMESPACE

/*!
    \class QDownloadManager
    \brief The QDownloadManager class provides asset data download capability.
    \since 4.8
    \ingroup qt3d
    \ingroup qt3d::network

    QDownloadManager encapsulates the functions needed for the downloading
    of asset data (eg. textures) from a network URL (eg. a website).

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

    connect(this, SLOT(myReceiverSlot(QByteArray*)), &dlmanage, SIGNAL(downloadComplete(QByteArray*)));

    if (!dlmanage.downloadAsset(QUrl("www.my.image.url.net/image.jpg"))) {
        dWarning("the manager was unable to send the url request.");
    }
    \endcode

    The QDownloadManager will emit the downloadComplete() signal which contains
    all of the downloaded data in a QByteArray.  It is the user's responsibility
    to convert this data to the format they require, and to verify that it is
    correct.
*/
QNetworkAccessManager* QDownloadManager::m_netAccessMgr = NULL;

/*!
    Construct a new instance of the QDownloadManager and attach it
    to \a parent.  Internally this initialises the QNetworkAccessManager
    which is shared by instances of this class, if it has not yet been
    initialised.
*/
QDownloadManager::QDownloadManager(QObject *parent):QObject(parent)
{
    if (!m_netAccessMgr) {
        m_netAccessMgr = new QNetworkAccessManager();
    }
}

/*!
    Returns a pointer to the underlying QNetworkAccessManager which the
    QDownloadManager instances use to download content.
*/
QNetworkAccessManager * QDownloadManager::getNetworkManager()
{
    return m_netAccessMgr;
}

/*!
    Instructs the QDownloadManager to download the content specified
    in \a assetUrl.

    A return value of true indicates that the network request was
    successfully queued/sent, while a return value of false indicates
    a problem with sending (possibly a poorly specified URL).
*/
bool QDownloadManager::downloadAsset(QUrl assetUrl)
{
    //URL Sanity check
    if ( ! assetUrl.isValid()) {
        qWarning() << "Invalid texture URL: " << assetUrl.toString();
        return false;
    }

    QNetworkRequest request(assetUrl);
    request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferCache);
    QNetworkReply *netReply = m_netAccessMgr->get(request);

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
    //Q_ASSERT(reply);
    if (!reply) {
        qWarning("DownloadManager's signal sender was not a QNetworkReply.");
        return;
    }

    if (reply->error() != QNetworkReply::NoError) {
        qWarning() << "Error in network reply: " << reply->url() << "(" << reply->errorString() << ")";
        reply->deleteLater();
        emit downloadComplete(NULL);
        return;
    }

    //In the case of a reply which is a redirect
    QVariant redirection = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
    if (redirection.isValid()) {
        QUrl url = redirection.toUrl();
        //Check if we have a relative URL
        if (url.isRelative()) {
            url.setScheme(reply->url().scheme());
            url.setHost(reply->url().host());
        }

        //Reissue redirected request.
        QNetworkRequest request(url);
        QNetworkReply * netReply = m_netAccessMgr->get(request);
        connect(netReply, SIGNAL(finished()), SLOT(netReplyDone()));
        reply->deleteLater();
        return;
    }

    //In the case of just data being returned
    //qDebug() << "ContentType:" << reply->header(QNetworkRequest::ContentTypeHeader).toString();
    QByteArray *assetData = new QByteArray();
    *assetData = reply->readAll();
    reply->deleteLater();

    emit downloadComplete(assetData);
}

/*!
    \fn QDownloadManager::downloadComplete(QByteArray* assetData)

    Signals that the download is completed.   A successful download will
    have a valid QByteArray stored in \a assetData, while a failed download
    (due to network error, etc), will result in a NULL value.
*/

QT_END_NAMESPACE
