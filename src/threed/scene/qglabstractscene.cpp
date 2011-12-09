/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
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
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qglabstractscene.h"
#include "qglsceneformatplugin.h"
#include "qglpicknode.h"

// copied private header
#include "qfactoryloader_p.h"

#include <QtCore/qfile.h>
#include <QtCore/qfileinfo.h>
#include <QtCore/qlibraryinfo.h>
#include <QtNetwork/qnetworkreply.h>
#include <QtCore/qcoreevent.h>
#include <QtCore/qdebug.h>
#include <QtCore/qcoreapplication.h>
#include <QtCore/qdir.h>
#include <QtCore/qpluginloader.h>

QT_BEGIN_NAMESPACE

/*!
    \class QGLAbstractScene
    \brief The QGLAbstractScene class represents a 3D scene consisting of zero or more QGLSceneNode instances.
    \since 4.8
    \ingroup qt3d
    \ingroup qt3d::scene

    Scenes are typically created by 3D modelling packages and then loaded
    into the application via a QGLSceneFormatPlugin; but they can also be
    constructed programatically.  The functions in this
    class provide access to major scene objects so that they can be
    applied or drawn whenever the application decides.

    QGLAbstractScene presents a very simple model of a 3D scene.  Subclasses
    implement the specific scene graph and object representations in a
    manner specific to the package's format.  Subclasses may also provide
    functionality to create new objects at runtime, but this is not required.

    The scene is defined to consist of a number of important objects
    of the following types:

    \list
    \o Camera objects define a viewing position in world-coordinates and a
       projection that maps 3D world co-ordinates to 2D screen co-ordinates.
       Camera objects should inherit from QGLCamera.
    \o Main objects designate the major elements of the scene besides
       cameras, lights, and effects.  Usually they inherit from QGLSceneNode.
    \o Light objects define positions and parameters for lighting the scene.
       Light objects should inherit from QGLLightParameters.
    \o Effect objects define materials, shaders, and textures for use in
       rendering the surface of objects.  Normally effects are activated
       automatically when main objects are drawn.  But effects can be used
       independently if the 3D format is acting as a library of effects.
    \o Mesh objects define geometry information independently of effects.
       Normally meshes are drawn automatically with an appropriate effect
       when main objects are drawn.  But meshes can be used independently
       if the 3D format is acting as a library of meshes.  Mesh objects
       should inherit from QGLSceneNode.
    \endlist

    Typically, the full scene represented by an external model format
    is not interesting to the application.  3D modelling packages
    regularly insert cameras, lights, effects, and other library
    objects that are useful to the modelling package, but not the
    application.  The mainNode() is usually the most interesting
    to applications.

    QGLAbstractScene makes it easy to access the major scene elements
    with object(), objects(), and mainNode().

    There are many other kinds of objects in the scene that may not
    be accessible via QGLAbstractScene because they are not considered
    "important" enough.  For example, a file that contains the data
    for a skateboard object would contain many objects for the board,
    wheels, texturing effects, animations, and so on.  The application
    may only be interested in the skateboard as a whole, and not its
    sub-components.  The skateboard would be considered an important
    main object in this case, which can be easily accessed and
    incorporated into the application's logic.

    Each Subclass needs to provide its own policy for deciding which
    objects are considered "important".

    \sa QGLSceneNode, QGLSceneFormatPlugin
*/

class QGLAbstractScenePrivate
{
public:
    QGLAbstractScenePrivate()
        : picking(false), nextPickId(-1), pickNodesDirty(true) {}
    bool picking;
    int nextPickId;
    QList<QGLPickNode*> pickNodes;
    QSet<QGLSceneNode*> pickable;
    bool pickNodesDirty;
};

/*!
    Constructs a 3D scene and attaches it to \a parent.
*/
QGLAbstractScene::QGLAbstractScene(QObject *parent)
    : QObject(parent)
    , d_ptr(new QGLAbstractScenePrivate)
{
}

/*!
    Destroys this 3D scene.
*/
QGLAbstractScene::~QGLAbstractScene()
{
}

/*!
    \internal
*/
void QGLAbstractScene::childEvent(QChildEvent *event)
{
    Q_D(QGLAbstractScene);
    if (event->type() == QEvent::ChildAdded)
        d->pickNodesDirty = true;
}

/*!
    Sets this scene to be pickable if \a enable is true, otherwise picking
    is disabled.  If the scene is set to be pickable, pick nodes are
    generated by calling generatePickNodes().

    \sa generatePickNodes(), pickable()
*/
void QGLAbstractScene::setPickable(bool enable)
{
    Q_D(QGLAbstractScene);
    if (enable != d->picking)
    {
        d->picking = enable;
        if (d->picking)
            generatePickNodes();
    }
}

/*!
    Returns true if this scene is pickable.

    \sa setPickable()
*/
bool QGLAbstractScene::pickable() const
{
    Q_D(const QGLAbstractScene);
    return d->picking;
}

/*!
    Generates QGLPickNode instances for important QGLSceneNode instances that are
    pickable.  Objects that are either not important or not pickable can
    be omitted.  The default implementation simply generates pick nodes
    for every top level object of type QGLSceneNode.

    Sub-classes may implement different schemes for picking.  When doing
    so parent the QGLPickNode objects onto the scene, so that they will
    appear in the list returned by pickNodes()

    \sa pickNodes(), setPickable()
*/
void QGLAbstractScene::generatePickNodes()
{
    Q_D(QGLAbstractScene);
    QList<QObject *> objs = objects();
    QList<QObject *>::iterator it = objs.begin();
    d->pickNodes.clear();
    for ( ; it != objs.end(); ++it)
    {
        QGLSceneNode *n = qobject_cast<QGLSceneNode *>(*it);
        if (d) {
            if (!d->pickable.contains(n)) {
                n->setPickNode(new QGLPickNode(this));
                d->pickable.insert(n);
            }
            d->pickNodes.append(n->pickNode());
        }
    }
}

/*!
    Increments and returns the next available pick id for this scene.
*/
int QGLAbstractScene::nextPickId()
{
    return ++d_ptr->nextPickId;
}

/*!
    Returns a list of the pick nodes that have been parented onto this
    scene.

    \sa generatePickNodes()
*/
QList<QGLPickNode *> QGLAbstractScene::pickNodes() const
{
    if (d_ptr->pickNodesDirty)
    {
        const_cast<QGLAbstractScene*>(this)->generatePickNodes();
        d_ptr->pickNodesDirty = false;
    }
    return d_ptr->pickNodes;
}

/*!
    \fn QList<QObject *> QGLAbstractScene::objects() const

    Returns a list of all objects in the scene which are considered
    important.

    Important objects will typically be the main mesh object, cameras,
    lights, and other top-level objects.  Sub-meshes and effects
    are normally not considered important unless the scene is
    acting as a library of meshes and effects.

    \sa objectNames(), object(), mainNode()
*/

/*!
    Returns a list of the names of all objects in the scene which
    are considered important, and which have non-empty names
    associated with them.

    The default implementation calls objects() and then compiles a list
    of all non-empty object names.

    \sa objects()
*/
QStringList QGLAbstractScene::objectNames() const
{
    QList<QObject *> objs = objects();
    QStringList names;
    for (int index = 0; index < objs.count(); ++index) {
        QObject *object = objs.at(index);
        if (object) {
            QString name = object->objectName();
            if (!name.isEmpty())
                names += name;
        }
    }
    return names;
}

/*!
    Returns the scene object that has the specified \a name;
    or null if the object was not found.

    The default implementation searches objects() for an object that
    matches \a name.

    \sa objects()
*/
QObject *QGLAbstractScene::object(const QString& name) const
{
    if (name.isEmpty())
        return 0;
    QList<QObject *> objs = objects();
    for (int index = 0; index < objs.count(); ++index) {
        QObject *object = objs.at(index);
        if (object && object->objectName() == name)
            return object;
    }
    return 0;
}


/*!
    \fn QGLSceneNode *QGLAbstractScene::mainNode() const

    Returns the main mesh node in the scene, or null if the scene
    does not contain a main mesh node.

    \sa objects()
*/

#if !defined (QT_NO_LIBRARY) && !defined(QT_NO_SETTINGS)
Q_GLOBAL_STATIC_WITH_ARGS(QFactoryLoader, loader,
    (QGLSceneFormatFactoryInterface_iid, QLatin1String("/sceneformats")))
#endif

/*!
    Loads a scene from \a device in the specified \a format using
    the registered scene format plugins.  If \a format is an empty
    string, then the format will be autodetected from the filename
    extension of \a device.  The \a url specifies the location of
    the data in \a device so that relative resources can be located.

    The \a options string is passed to the underlying format loader
    and its meaning and format depend on the loader.  For example the
    format string for the .3ds loader accepts the following options:
    \list
    \o ForceSmooth - average normals for a smooth appearance
    \o ForceFaceted - per face normals for a faceted appearance
    \o NativeIndices - map native indices for poorly smoothed models
    \o CorrectNormals - fix inverted normals on models with bad windings
    \o CorrectAcute - fix normals on models that smooth acute angles
    \endlist

    The options may be specified globally for the whole model, or just
    for a particular mesh.

    In this example smoothing is forced on globally, and native indices
    are used on just the mesh called "BattCoverMesh".

    \code
    QString op = "ForceSmooth BattCoverMesh=NativeIndices";
    QString file = "music-player.3ds";
    QGLAbstractScene *scene = QGLAbstractScene::loadScene(file, QString(), op);
    \endcode

    Returns the scene object, or null if the scene could not be loaded
    or the \a format was not supported by any of the plugins.

    \sa QGLSceneFormatPlugin
*/
QGLAbstractScene *QGLAbstractScene::loadScene
    (QIODevice *device, const QUrl& url, const QString& format, const QString &options)
{
#if !defined (QT_NO_LIBRARY) && !defined(QT_NO_SETTINGS)
    if (!device)
        return 0;

    QFactoryLoader *l = loader();
    QStringList keys = l->keys();

    // If the format is not specified, then use the filename/url extension.
    QString fmt = format;
    if (fmt.isEmpty()) {
        QFile *file = qobject_cast<QFile *>(device);
        QString name;
        if (file) {
            name = file->fileName();
        } else {
            QNetworkReply *reply = qobject_cast<QNetworkReply *>(device);
            if (reply)
                name = reply->url().path();
        }
        int dot = name.lastIndexOf(QLatin1Char('.'));
        QString suffix = name.mid(dot+1).toLower();
        int index = keys.indexOf(suffix);
        if (index >= 0)
            fmt = suffix;
    }

    // Find the plugin that handles the format and ask it to create a handler.
    if (QGLSceneFormatFactoryInterface *factory
            = qobject_cast<QGLSceneFormatFactoryInterface*>
                (l->instance(fmt))) {
        QGLSceneFormatHandler *handler = factory->create(device, url, fmt);
        if (handler) {
            handler->setDevice(device);
            handler->setUrl(url);
            handler->setFormat(format);
            if (!options.isEmpty())
                handler->decodeOptions(options);
            QGLAbstractScene *scene = handler->read();
            delete handler;
            return scene;
        }
    }

    // If we get here, then the format is not supported by any of the plugins.
#ifndef QT_NO_DEBUG
    qWarning("Could not create handler for format %s"
             "- check plugins are installed correctly in %s",
             qPrintable(fmt),
             qPrintable(QLibraryInfo::location(QLibraryInfo::PluginsPath)));
#endif
    return 0;
#else // QT_NO_LIBRARY || QT_NO_SETTINGS
    Q_UNUSED(device);
    Q_UNUSED(url);
    Q_UNUSED(format);
    return 0;
#endif // QT_NO_LIBRARY || QT_NO_SETTINGS
}

/*!
    Loads a scene from \a fileName in the specified \a format, with the
    supplied \a options, and using the registered scene format plugins.

    If \a format is an empty string, then the format will be autodetected
    from the extension of \a fileName.

    The \a options string is passed to the underlying format loader
    and its meaning and format depend on the loader.  See the doc above
    for loadScene() for details on the 3ds format options.

    Returns the scene object, or null if the scene could not be loaded
    or the \a format was not supported by any of the plugins.

    \sa QGLSceneFormatPlugin
*/
QGLAbstractScene *QGLAbstractScene::loadScene
    (const QString& fileName, const QString& format, const QString &options)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly))
    {
        if (options.contains(QLatin1String("ShowWarnings")))
            qWarning("Could not read %s", qPrintable(fileName));
        return 0;
    }
    QFileInfo fi(fileName);
    QUrl url = QUrl::fromLocalFile(fi.absoluteFilePath());
    return loadScene(&file, url, format, options);
}

/*!
    \enum QGLAbstractScene::FormatListType
    This enum specifies the format of the list returned by the supportedFormats() function.

    \value AsFilter Return a format list that may be used as a filter.
    \value AsSuffix Return a format list that is simply the filename suffixes.
*/

/*!
    Returns a list of all supported formats known by currently available
    sceneformat plugins, in the format type \a t.

    If \a t is QGLAbstractScene::AsFilter then the result may be passed
    to QDir::setNameFilters(), or used in other filters.  This is the default.

    For example to create a file dialog to load model files use this:
    \code
    QString modelsDir = QDir::toNativeSeperators(QDir::homePath());
    QString filter = tr("Models (%1)").arg(QAbstractScene::supportedFormats().join(" "));
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open File"), modelsDir, filter));
    \endcode

    Otherwise (when \a t is QGLAbstractScene::AsSuffix) it is simply a list
    of file name suffixes.

    Note that this function may be expensive to
    call since it scans for available plugins, and loads each one it
    finds to get an accurate report of formats supported at run-time.
*/
QStringList QGLAbstractScene::supportedFormats(QGLAbstractScene::FormatListType t)
{
    QStringList formats;
    QSet<QString> formatSet;
    QSet<QString> dirSet;
    QStringList pluginPaths = QCoreApplication::libraryPaths();
    QStringList::const_iterator it = pluginPaths.constBegin();
    for ( ; it != pluginPaths.constEnd(); ++it)
    {
        QString path = *it;
        QDir sceneformatDir(path + QLatin1String("/sceneformats"));
        path = sceneformatDir.absolutePath();
        if (!sceneformatDir.exists() || dirSet.contains(path))
            continue;
        dirSet.insert(path);
        sceneformatDir.setFilter(QDir::Files);
        QStringList entries = sceneformatDir.entryList();
        QStringList::const_iterator fit = entries.constBegin();
        for ( ; fit != entries.constEnd(); ++fit)
        {
            QString fi = *fit;
            QPluginLoader loader(sceneformatDir.absoluteFilePath(fi));
            QObject *inst = loader.instance();
            QGLSceneFormatFactoryInterface *iface = qobject_cast<QGLSceneFormatFactoryInterface*>(inst);
            if (iface)
            {
                QStringList formatKeys = iface->keys();
                QStringList::const_iterator kit = formatKeys.constBegin();
                for ( ; kit != formatKeys.constEnd(); ++kit)
                {
                    QString k = *kit;
                    if (!formatSet.contains(k) && !k.contains(QLatin1Char('/'))) // dont add mime-type keys
                    {
                        if (t == AsFilter)
                            k.prepend(QLatin1String("*."));
                        formatSet.insert(k);
                        formats.append(k);
                    }
                }
            }
        }
    }
    return formats;
}


QT_END_NAMESPACE
