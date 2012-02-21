/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/
**
** This file is part of the Qt3D module of the Qt Toolkit.
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
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qglsceneformatplugin.h"

QT_BEGIN_NAMESPACE

/*!
    \class QGLSceneFormatHandler
    \brief The QGLSceneFormatHandler class defines the common format I/O interface for loading 3D scene formats.
    \since 4.8
    \ingroup qt3d
    \ingroup qt3d::scene

    This interface will be deprecated in an upcoming release.  For more details see the
    \l{sceneformats-obj.html}{plug-in tutorial documentation}.

    \sa QGLSceneFormatPlugin
*/

class QGLSceneFormatHandlerPrivate
{
public:
    QGLSceneFormatHandlerPrivate()
    {
        device = 0;
    }

    QIODevice *device;
    QString format;
    QUrl url;
};

/*!
    Constructs a 3D scene format handler.
*/
QGLSceneFormatHandler::QGLSceneFormatHandler()
{
    d_ptr = new QGLSceneFormatHandlerPrivate();
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
    the model data with read().  Exactly what the string value may contain
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
    \class QGLSceneFormatFactoryInterface
    \brief The QGLSceneFormatFactoryInterface class provides the factory interface for QGLSceneFormatPlugin.
    \internal
*/

/*!
    \class QGLSceneFormatPlugin
    \brief The QGLSceneFormatPlugin class defines an interface for loading 3D object and scene formats.
    \ingroup qt3d
    \ingroup qt3d::scene

    This class will be deprecated in an upcoming release.  For more details see the
    \l{sceneformats-obj.html}{plug-in tutorial documentation}.

    \sa QGLSceneFormatHandler
*/

/*!
    Constructs a 3D scene format plugin with the given \a parent.  This is
    invoked automatically by the Q_EXPORT_PLUGIN2() macro.
*/
QGLSceneFormatPlugin::QGLSceneFormatPlugin(QObject *parent)
    : QObject(parent)
{
}

/*!
    Destroys this 3D scene format plugin.  This is invoked automatically
    when the plugin is unloaded.
*/
QGLSceneFormatPlugin::~QGLSceneFormatPlugin()
{
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
