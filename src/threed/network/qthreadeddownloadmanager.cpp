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
#include "qthreadeddownloadmanager.h"
#include "qthreadeddownloadmanager_p.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QDebug>
#include <QEventLoop>

QT_BEGIN_NAMESPACE

/*!
    \class QThreadedDownloadManager
    \brief The QThreadedDownloadManager class provides asset data download capability which
    runs in its own download thread.
    \since 4.8
    \ingroup qt3d
    \ingroup qt3d::network

    QThreadedDownloadManager extends the QAbstractDownloadManager class for use in a
    multi-threaded Qt3D application. The QThreadedDownloadManager creates a single download
    thread which is shared by all instances of the class.  This thread is based on QThread,
    and it automatically queues and dispatches network requests.

    The downloader thread contains its own a QNetworkAccessManager.  The download manager
    itself provides all handling of network requests and replies internally, and
    allows for redirection of URLs and handling of errors without the need
    for the user to query network replies.

    To use the QAbstractDownloadManager simply create an instance of the class
    within your code, and connect QDownloadManager's downloadComplete() signal
    to a slot in your own code, then call the downloadAsset() function.

    For example

    \code
    QAbstractDownloadManager dlmanage;

    connect(this, SLOT(myReceiverSlot(QByteArray)), &dlmanage, SIGNAL(downloadComplete(QByteArray)));

    if (!dlmanage.downloadAsset(QUrl("www.my.image.url.net/image.jpg"))) {
        dWarning("the manager was unable to send the url request.");
    }
    \endcode

    The QAbstractDownloadManager will emit the downloadComplete() signal which contains
    all of the downloaded data in a QByteArray.  It is the user's responsibility
    to convert this data to the format they require, and to verify that it is
    correct.

    When no requests are queue, the QAbstractDownloadManager will halt the downloader
    thread.  Multiple QAbstractDownloadManager classes can simultaneously submit
    requests, though the classes themselves are not intended to be completely thread
    safe, and developers should refrain from attempting to interact with the
    underlying classes through anything other than the specified API.
*/

Q_GLOBAL_STATIC(QDownloadThread, getDownloadThread)

/*!
    Constructs a new instance of the QDownloadManager and attach it
    to a \a parent QObject.
*/
QThreadedDownloadManager::QThreadedDownloadManager(QObject *parent) :
    QAbstractDownloadManager(parent)
{
}

/*!
    Destroys the current instance of the QThreadedDownloadManager.  The destructor
    checks whether other instances of the class exist, and if none are found
    it sends a signal to the download thread to halt.  It waits for the thread
    to end, then deletes the thread instance.
*/
QThreadedDownloadManager::~QThreadedDownloadManager()
{
}

/*!
    Instructs the QThreadedDownloadManager to start the download thread.  If no thread
    class currently exists this function will create one.

    If an existing thread is already in the process of shutting down the function will wait
    for the thread to end before re-starting it.

    If an existing thread is running, the function will do nothing.
*/
void QThreadedDownloadManager::beginThread()
{
    //Connect the thread instance if one doesn't exist
    connect(this, SIGNAL(endThread()), getDownloadThread(), SLOT(quit()));

    //Signal thread completion so we can do any cleanup
    connect(getDownloadThread(), SIGNAL(finished()), this, SLOT(threadExit()));

    //If thread has had a quit message posted, wait until it's done before commencing
    if (getDownloadThread()->isThreadQuitting())
    {
        if (!getDownloadThread()->wait(1000))
        {
            qWarning("Download thread did not finish in timely manner - killing");
            getDownloadThread()->terminate();
            qWarning("Waiting for termination");
            while (getDownloadThread()->wait(1000));
        }
    }

    //Spin the thread if it isn't running, otherwise, it's already going, so just carry on.
    if (!getDownloadThread()->isRunning()) {
        getDownloadThread()->start();
    }
}

/*!
    Instructs the QThreadedDownloadManager to download the content specified
    in \a assetUrl.  This may be a texture, 3d model, or similar.

    A return value of true indicates that the network request was
    successfully queued for sending, while a return value of false
    indicates a problem with sending (possibly a poorly specified URL, or
    network failure).
*/
bool QThreadedDownloadManager::beginDownload(QUrl assetUrl)
{
    //Ensure a thread is running
    beginThread();

    //Create the download instance - this object will be transferred to worker thread
    //control once all signals and slots to the current thread have been connected.
    QDownloadInstance * cObject = new QDownloadInstance();

    cObject->doSetup(*this);
    cObject->moveToThread(getDownloadThread());

    emit startDownload(assetUrl);
    return true;
}

/*!
    Instructs the QThreadedDownloadManager that the network replies have been received
    successfully.  This currently performs no other processing, though is retained
    and can be overriden for debugging purposes.
*/
void QThreadedDownloadManager::netReplyDone()
{
    //do nothing - this is kept for debugging/extension purposes
}

/*!
    Instructs the QThreadedDownloadManager that the downloader thread has exited
    successfully.  This currently performs no other processing, though is retained
    and can be overriden for debugging purposes.
*/
void QThreadedDownloadManager::threadExit()
{
    //do nothing - this is kept for debugging/extension purposes
}

/*!
    \fn QThreadedDownloadManager::endThread()
    Signals the download thread to exit.  This will be performed gracefully, and
    remaining messages in the message loop will be processed.
*/

/*!
    \fn QThreadedDownloadManager::startDownload(QUrl assetUrl)
    Signals the download thread to begin downloading from \a assetUrl.  This will
    be performed in the background until the download is complete.
*/

//Private Function Implementation for QDownloadInstance below.
QDownloadInstance::QDownloadInstance(QObject *parent) : QObject(parent)
{
    //do nothing
}

QDownloadInstance::~QDownloadInstance()
{
    //do nothing
}

void QDownloadInstance::doSetup(QThreadedDownloadManager &cComm)
{
    //Connect messages for starting the thread and alerting for completion of downloading (or error states)
    connect (&cComm, SIGNAL(startDownload(QUrl)), this, SLOT(doWork(QUrl)));
    connect (this, SIGNAL(downloadComplete(QByteArray)), &cComm, SIGNAL(downloadComplete(QByteArray)));
}

void QDownloadInstance::doWork(QUrl assetUrl)
{
    //Get the network access manager belonging to the QDownloadThread we're in (if any)
    QNetworkAccessManager * netAccessMgr;
    if (!(netAccessMgr=getNetworkManager())) {
        emit downloadComplete(NULL);
        return;
    }

    //Send requests as needed.
    QNetworkRequest request(assetUrl);
    request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferCache);
    QNetworkReply *netReply = netAccessMgr->get(request);

    connect(netReply, SIGNAL(finished()), this, SLOT(doneWork()));
    connect (this, SIGNAL(killDownloadThread()), getDownloadThread(), SLOT(quit()));

    //Add one to the current request count
    incrementRequestCount();
}

void QDownloadInstance::doneWork()
{
    //Ensure sanity of the sender as a QNetworkReply
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) {
        qWarning("QDownload::DoneWork signal sender was not a QNetworkReply.");
        sender()->deleteLater();
        return;
    }

    //Check for error states
    if (reply->error() != QNetworkReply::NoError) {
        qWarning() << "Error in network reply: " << reply->url() << "(" << reply->errorString() << ")";
        reply->deleteLater();
        emit downloadComplete(NULL);
        sender()->deleteLater();
        deleteLater();
        decrementRequestCount();
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
        QNetworkAccessManager * netAccessMgr;
        if (!(netAccessMgr=getNetworkManager())) {
            emit downloadComplete(NULL);
            return;
        }
        QNetworkReply * netReply = netAccessMgr->get(request);
        connect(netReply, SIGNAL(finished()), SLOT(netReplyDone()));
        reply->deleteLater();
        return;
    }

    //In the case of just data being returned
    //qDebug() << "ContentType:" << reply->header(QNetworkRequest::ContentTypeHeader).toString();
    QByteArray assetData;
    assetData = reply->readAll();
    emit downloadComplete(assetData);
    reply->deleteLater();

    //A successful request means that we can decrement the number of requests in the current
    //thread - once this reaches zero, the thread will be halted for efficiency.
    deleteLater();
    decrementRequestCount();
}

QNetworkAccessManager * QDownloadInstance::getNetworkManager()
{
    QDownloadThread *downloadThread = qobject_cast<QDownloadThread*>(thread());

    if (!downloadThread)
    {
        qWarning() << "QDownload instance is not running in a DownloadThread - cannot access network.";
        return NULL;
    }

    return downloadThread->m_netAccessMgr;
}

void QDownloadInstance::incrementRequestCount()
{
    QDownloadThread *downloadThread = qobject_cast<QDownloadThread*>(thread());

    if (!downloadThread)
    {
        qWarning() << "QDownload instance is not running in a DownloadThread - cannot increment network request count.";
        return;
    }

    //We keep a count of the number of network requests currently queued by the
    //network access manager in the thread.  This is used to check if we want to keep the
    //thread running - if no requests are pending we will simply kill the thread.
    downloadThread->m_requestCount++;
}

void QDownloadInstance::decrementRequestCount()
{
    QDownloadThread *downloadThread = qobject_cast<QDownloadThread*>(thread());

    if (!downloadThread)
    {
        qWarning() << "QDownload instance is not running in a DownloadThread - cannot decrement network request count.";
        return;
    }

    //We keep a count of the number of network requests currently queued by the
    //network access manager in the thread.  This is used to check if we want to keep the
    //thread running - if no requests are pending we will simply kill the thread (after
    //processing current messages, of course).
    downloadThread->m_requestCount--;

    if (downloadThread->m_requestCount<=0) {
        downloadThread->m_threadQuitting = true;
        emit killDownloadThread();
    }
}

//Private Function Implementation for QDownloadThread below.
QDownloadThread::QDownloadThread(QObject *parent) : QThread(parent)
{
    m_threadQuitting = false;
    m_requestCount = 0;
    m_netAccessMgr = 0;
}

QDownloadThread::~QDownloadThread()
{
}

void QDownloadThread::run()
{
    //When we run the thread we ensure a valid QNetworkAccessManager exists.
    if (!m_netAccessMgr) {
        m_netAccessMgr = new QNetworkAccessManager(this);
    }

    m_threadQuitting = false;
    exec();
}

bool QDownloadThread::isThreadQuitting() const
{
    return m_threadQuitting;
}

QT_END_NAMESPACE
