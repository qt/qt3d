/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtQuick3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qglsceneformatplugin.h"
#include "qdownloadmanager.h"
#include "qabstractdownloadmanager.h"
#include "qthreadeddownloadmanager.h"
#include <QDebug>

QT_BEGIN_NAMESPACE

/*!
    \class QGLSceneFormatHandler
    \brief The QGLSceneFormatHandler class defines the common format I/O interface for loading 3D scene formats.
    \since 4.8
    \ingroup qt3d
    \ingroup qt3d::scene

    \sa QGLSceneFormatPlugin
*/

class QGLSceneFormatHandlerPrivate
{
public:
    QGLSceneFormatHandlerPrivate()
    {
        device = 0;
        m_scene = 0;
    }

    QIODevice *device;
    QString format;
    QUrl url;
    QGLAbstractScene *m_scene;
};

/*!
    Constructs a 3D scene format handler.
*/
QGLSceneFormatHandler::QGLSceneFormatHandler()
{
    d_ptr = new QGLSceneFormatHandlerPrivate();
    m_downloadManager = 0;
}

/*!
    Destroys this 3D scene format handler.
*/
QGLSceneFormatHandler::~QGLSceneFormatHandler()
{
    delete d_ptr;
}

/*!
    Returns the device currently assigned to the 3D scene format handler.
    Returns null if no device has been assigned.

    \sa setDevice()
*/
QIODevice *QGLSceneFormatHandler::device() const
{
    return d_ptr->device;
}

/*!
    Sets the device for this 3D scene format handler to \a device.
    The handler will use this device when reading 3D scenes.

    The device can only be set once and must be set before calling
    read().  If you need to read multiple files, construct multiple
    instances of the appropriate QGLSceneFormatHandler subclass.

    \sa device()
*/
void QGLSceneFormatHandler::setDevice(QIODevice *device)
{
    d_ptr->device = device;
}

/*!
    Returns the format that is currently assigned to this 3D scene
    format handler.  Returns an empty string if no format has been assigned.

    \sa setFormat()
*/
QString QGLSceneFormatHandler::format() const
{
    return d_ptr->format;
}

/*!
    Sets the format of this 3D scene format handler to \a format.

    \sa format()
*/
void QGLSceneFormatHandler::setFormat(const QString& format)
{
    d_ptr->format = format;
}

/*!
    Returns the url of the data provided by device().  This is
    typically used when the data in device() refers to additional
    files that need to be located relative to the original
    directory.

    \sa setUrl()
*/
QUrl QGLSceneFormatHandler::url() const
{
    return d_ptr->url;
}

/*!
    Sets the \a url of the data provided by device().

    \sa url()
*/
void QGLSceneFormatHandler::setUrl(const QUrl& url)
{
    d_ptr->url = url;
}

/*!
    Decodes and applies \a options to this handler.  Generally this will be
    called by the QGLAbstractScene::loadScene() method prior to reading
    the model data with read() or download().  Exactly what the string value may contain
    and the meaning of the encoded options depends on each individual plugin.

    This default implementation simply does nothing.
*/
void QGLSceneFormatHandler::decodeOptions(const QString &options)
{
    Q_UNUSED(options);
}

/*!
    \fn QGLAbstractScene *QGLSceneFormatHandler::read()

    Reads a 3D scene from device() and returns it.  Returns null if
    the format of device() is invalid and a scene could not be read.
*/

/*!
    \fn QGLAbstractScene *QGLSceneFormatHandler::download()

    Reads a 3D scene from a url and returns a stub to it which can be used
    the application, and will be updated with the full scene later.

    Returns NULL if the scene could not be created.
*/

/*!
    \class QGLSceneFormatFactoryInterface
    \brief The QGLSceneFormatFactoryInterface class provides the factory interface for QGLSceneFormatPlugin.
    \internal
*/

/*!
    \class QGLSceneFormatPlugin
    \brief The QGLSceneFormatPlugin class defines an interface for loading 3D object and scene formats.
    \ingroup qt3d
    \ingroup qt3d::scene

    \sa QGLSceneFormatHandler
*/

/*!
    Constructs a 3D scene format plugin with the given \a parent.  This is
    invoked automatically by the Q_EXPORT_PLUGIN2() macro.
*/
QGLSceneFormatPlugin::QGLSceneFormatPlugin(QObject *parent)
    : QObject(parent)
{
    //do nothing
}

/*!
    Destroys this 3D scene format plugin.  This is invoked automatically
    when the plugin is unloaded.
*/
QGLSceneFormatPlugin::~QGLSceneFormatPlugin()
{
    //do nothing
}

/*!
    Initiates download of the specified scene.  The scene must have been
    previously specified with URL, options, format, and so on.  This
    function will then create a QGLAbstractDownloadManager instance and
    submit the network request for the scene.

    The resultant data will be handled by the downloadComplete() slot.
*/
void QGLSceneFormatHandler::downloadScene()
{
    if (!m_downloadManager) {
           if (getenv(QT3D_MULTITHREAD)) {
               //Download in a multithreaded environment
               m_downloadManager = new QThreadedDownloadManager();
           } else {
           //Download in a single threaded environment
           m_downloadManager = new QDownloadManager();
           }
           connect(m_downloadManager,SIGNAL(downloadComplete(QByteArray)), this, SLOT(downloadComplete(QByteArray)));
   }

   if (!m_downloadManager->beginDownload(QUrl(url().toString()))) {
       qWarning("Unable to issue asset download request.");
   }
}

/*!
    Set the pointer for the scene \a theScene currently being processed.
    This is specifically used during network loading.
*/
void QGLSceneFormatHandler::setScene(QGLAbstractScene *theScene)
{
    d_ptr->m_scene = theScene;
}

/*!
    Get the pointer for the scene currently being processed.  This is
    specifically used during network loading.
*/
QGLAbstractScene * QGLSceneFormatHandler::getScene() const
{
    return d_ptr->m_scene;
}

/*!
    This function should be called when the download of a remotely hosted
    scene is complete.  The \a sceneData will contain the relevant data for
    the scene.

    The user is required to reimplement this if they intend to support network
    loading of scenes in their plugin.
*/
void QGLSceneFormatHandler::downloadComplete(QByteArray sceneData)
{
    //unused - re-implement if you intend to support network
    //loading of model files.
}

/*!
    Called at the end of the read() and download() functions, this function
    completes any extra cleanup required, and sets the QGLSceneFormatHandler
    it belongs to as being ready for deletion when the main event loop is next
    processed.
*/
void QGLSceneFormatHandler::finalize()
 {
    deleteLater();
 }

/*!
    \fn QStringList QGLSceneFormatPlugin::keys() const

    Returns the list of format keys this plugin supports.  These keys
    are usually the names of the 3D scene formats that are implemented in
    the plugin (e.g., "3ds", "obj", etc).  The returned elements must
    be in lower case.

    \sa create()
*/

/*!
    \fn QGLSceneFormatHandler *QGLSceneFormatPlugin::create(QIODevice *device, const QUrl& url, const QString &format) const

    Creates and returns a QGLSceneFormatHandler for handling the data in
    \a device according to \a format.  The \a url specifies the original
    location of the data for resolving relative resource references.

    \sa keys()
*/

QT_END_NAMESPACE
