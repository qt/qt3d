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

#include "qdeclarativemesh.h"

#include "qglabstractscene.h"
#include "qglscenenode.h"
#include "qglmaterial.h"
#include "qglmaterialcollection.h"

#include <QNetworkRequest>
#include <QNetworkReply>
#include <QtDeclarative/qdeclarativeengine.h>
#include <QtCore/qlist.h>

/*!
    \qmlclass Mesh QDeclarativeMesh
    \brief The Mesh item provides a means of abstracting 3D mesh and geometry representations in
    a way that can be used in QML.  The Mesh class contains necessary properties and methods for
    display and manipulation of 3d objects of this type.
    \since 4.8
    \ingroup qt3d::qml3d

    \section1 Underlying Architecture

    QML/3d Mesh objects are a QML enabled means of representing any scene class based
    on the \l QGLAbstractScene class.

    In cases where the scene is composed of multiple nodes (for example, in a \i .3ds file)
    the mesh allows users to split away branches of the tree and manipulate them in
    isolation from the original scene graph.

    In order to achieve this the mesh object contains a map correlating numeric identifiers
    for individual branches with specific \l QGLSceneNode objects which are taken from
    the original scene.

    More details on achieving this can be found in the \l Item3D class.

    \section1 Specifying Simple Mesh Objects

    Like other objects in QML/3d, Mesh objects are created by the standard QML syntax for
    objects.  Meshes should always specify an appropriate file containing the data for
    the Mesh,

    \code
    Mesh {source: "model.obj"}
    \endcode

    For objects loaded from the network we can wait for an "onLoaded" signal in QML (from the
    loaded() signal in the Mesh class) before performing specific actions.  For example:

    \code
    Item3D {
        id: theMesh
        mesh: Mesh {
            source: "http://www.example_3d_model_url.com/sample.obj"
            onLoaded:{theMesh.effect=theEffect}
        }

        Effect {
            id: theEffect
            texture: ":/qml/texture.jpg" }
        }
    }
    \endcode

    This code will wait until the mesh has been fully loaded before attempting to apply the effects.
*/

QT_BEGIN_NAMESPACE

class QDeclarativeMeshPrivate
{
public:
    QDeclarativeMeshPrivate()
        : dataReply(0)
        , scene(0)
        , nextSceneBranchId(0)
        , mainSceneObject(NULL)
        , refCount(1)
        , completed(false)
        , loaded(false)
        , dumpInfo(false)
    {}
    ~QDeclarativeMeshPrivate()
    {
        delete scene;
    }

    struct branchObject {
        QObject *previousParent;
        QGLSceneNode *rootSceneObject;
    };

    QUrl data;
    QString meshName;
    QDeclarativeMesh *parentMesh;
    QNetworkReply *dataReply;
    QGLAbstractScene *scene;
    int nextSceneBranchId;
    QMap<int, branchObject> sceneBranches;
    QList<QGLSceneNode *>sceneObjects;
    QGLSceneNode *mainSceneObject;
    QList<QGLMaterial *> connected;
    int refCount;
    bool completed;
    bool loaded;
    QString options;
    bool dumpInfo;
};

/*!
    \internal
    Construct a \l Mesh object with \a parent as its parent.
*/
QDeclarativeMesh::QDeclarativeMesh(QObject *parent)
    : QObject(parent)
{
    d = new QDeclarativeMeshPrivate();
}

/*!
    \internal
    Destroy the \l Mesh object and free any unneeded memory.
*/
QDeclarativeMesh::~QDeclarativeMesh()
{
    delete d;
}

/*!
    \qmlproperty url Mesh::source

    Each mesh requires a set of data that describes (among other things) vertices
    transformations, textures, and so on.  This data is generally stored in files and
    specified via the source property.

    Source files can be of any type supported by QtQuick3D.  The types of file currently
    supported can be found in the \c sceneFormat plugins, with \i .3ds, \i .bez, \i. obj
    files currently being supported.

    Also due to limitations of 3rd party libraries, .dxf support is rather limited.
    Only 3d components (no 2d lines and such supported), and there are also issues with
    loading newer file types (everything up to the 2004-2006 file format seems to work,
    anything after we've had issues with).

    Meshes can also be stored within QRC files and loaded via the standard resource
    mechanisms, however there may be issues with some esoteric remote resource loading
    requirements within the different file types.
*/

QUrl QDeclarativeMesh::source() const
{
    return d->data;
}

void QDeclarativeMesh::setSource(const QUrl& value)
{
    if (d->data == value)
        return;
    d->data = value;
    if (d->data.scheme() == QLatin1String("file")) {
        QGLAbstractScene *s = QGLAbstractScene::loadScene(d->data.toLocalFile(),
                                                          QString(), d->options);
        setScene(s);
    } else if (d->data.scheme().toLower() == QLatin1String("qrc")) {
        // strips off any qrc: prefix and any excess slashes and replaces it with :/
        d->data.setScheme("");
        QGLAbstractScene *s = QGLAbstractScene::loadScene(
                    QLatin1String(":")+d->data.toString(),
                    QString(), d->options);
        setScene(s);
    } else {
        //network loading
        QGLAbstractScene *s = QGLAbstractScene::loadScene(d->data,QString(), d->options);
        connect(s, SIGNAL(sceneUpdated()), this, SLOT(dataRequestFinished()));
    }
}

/*!
    \qmlproperty string Mesh::meshName

    Users can associate a name with a mesh to facilitate easy finding or
    description of a mesh.
*/
QString QDeclarativeMesh::meshName() const
{
    return d->meshName;
}

void QDeclarativeMesh::setMeshName(const QString& value)
{
    bool emitDataChanged = false;
    if (d->meshName != value)
    {
        d->meshName = value;
        emitDataChanged = true;
    }
    if (d->loaded && d->scene) {
        QGLSceneNode *insertObject;
        if (value.isEmpty())
            insertObject = getSceneObject();
        else
            insertObject = getSceneObject(value);
        if (!insertObject && !value.isEmpty()) {
            qWarning() << "could not find" << value << "available:" << getSceneObjectNames();
        } else if (!insertObject) {
            qWarning() << "could not find main object in scene!";
        }

        //Add the object: if null it will make the object non-drawable, but still able
        //to run.
        addSceneBranch(insertObject);
        emitDataChanged = true;
    }
    if (emitDataChanged)
        emit dataChanged();
}

/*!
    \qmlproperty string Mesh::options

    The behaviour of the underlying file loader can be controlled by
    string encoded options.  Consult the documentation for each loader for
    the options available.

    Specifies the current options for loading this mesh.  The default value
    is the null string, meaning no options are applied.
*/
QString QDeclarativeMesh::options() const
{
    return d->options;
}

void QDeclarativeMesh::setOptions(const QString &options)
{
    if (options != d->options)
    {
        d->options = options;
        emit optionsChanged();
    }
}

/*!
  \internal
  Once the request for data has been finished the \l Mesh class is now free to load
  the scene.
*/
void QDeclarativeMesh::dataRequestFinished()
{
    //other processing may be required, but for now, simply emit a
    //change of data signal.
    QGLAbstractScene *sceneData = qobject_cast<QGLAbstractScene*>(sender());

    if (sceneData) {
        if (sceneData == d->scene)
            emit dataChanged();
        else
            setScene(sceneData);
    } else {
        qWarning("Data request recieved a signal from a class other than a valid scene.");
    }
}

/*!
    \internal
    Because the branches of the overall scene are essentially /i moveable, and the
    standard methods of getting objects/names from a scene rely on a single tree,
    the \l Mesh class supports its own list of mesh objects in the scene which is
    taken on startup from the normal \l QGLAbstractScene based class for the current
    mesh.

    This means that object lookup can be performed at any time without having to
    reconstitute the original scene or maintain logical links between split branches.

    As an added advantage it also eliminates the (minor) additional cost of having
    to traverse the scene-graph/tree in order to locate objects.

    \sa QGLAbstractScene
*/
void QDeclarativeMesh::initSceneObjectList()
{
    d->sceneObjects.clear();
    if (d->scene) {
        QList<QObject *> objs = d->scene->objects();
        for (int index = 0; index < objs.count(); ++index) {
            QGLSceneNode *node = qobject_cast<QGLSceneNode *>(objs.at(index));
            if (node)
                d->sceneObjects.append(node);
        }
        QGLSceneNode *old = d->mainSceneObject;
        d->mainSceneObject = d->scene->mainNode();
        if (old != d->mainSceneObject)
            emit nodeChanged();
    }
}

/*!
    \qmlproperty QGLSceneNode Mesh::getSceneObject

    Get the main scene node for the \l QGLAbstractScene associated with this mesh.
*/
QGLSceneNode *QDeclarativeMesh::getSceneObject()
{
    //This variant of the function gets the main scene object for the scene
    if (!d->mainSceneObject)
        initSceneObjectList();

    return d->mainSceneObject;
}

/*!
    \internal
    Get the scene object called \a name, specified as a QString, and retrieve the
    scene object in this mesh which corresponds to it.
*/
QGLSceneNode *QDeclarativeMesh::getSceneObject(const QString &name)
{
    //This variant of the function gets the mesh scene object for a scene
    //based on the name of the object.
    if (d->sceneObjects.empty())
        initSceneObjectList();

    foreach (QGLSceneNode *object, d->sceneObjects) {
        if (object && object->objectName().startsWith(name))
        {
            return object;
        }
    }

    return NULL;
}

/*!
    \internal
    Used mainly for diagnostic purposes this function returns a QStringList containing
    all of the object names for the given mesh.
*/
QStringList QDeclarativeMesh::getSceneObjectNames()
{
    //Get a string list of all mesh object names in the scene.
    if (d->sceneObjects.empty())
        initSceneObjectList();

    QStringList names;
    foreach (QGLSceneNode *object, d->sceneObjects) {
        if (object) {
            QString name = object->objectName();
            if (!name.isEmpty())
                names += name;
        }
    }
    return names;
}

/*!
    \qmlproperty bool Mesh::dumpInfo

    Flag set to true if the description of the item will be dumped to the console.

    When this flag is true, useful debugging and diagnostic information, for example the
    names of all the subnodes of the mesh, materials and geometry parameters, will be
    dumped to the console during the loading of the model.
*/
bool QDeclarativeMesh::dumpInfo() const
{
    return d->dumpInfo;
}

void QDeclarativeMesh::setDumpInfo(bool enable)
{
    if (enable != d->dumpInfo)
    {
        d->dumpInfo = enable;
        emit dumpInfoChanged();
    }
}

/*!
    \internal
    Set the \a scene associated with this mesh.

    The function attempts to load a meaningful scene if one exists, and will attempt to
    locate the section of the scene to which this mesh applies based on mesh-name.

    Failure to find an appropriate scene will generate the warning messages.

    It should be noted that for successful operation of this function, the appropriate plugin
    file reader should be built and installed.  Failure to do so will result in a warning and
    failure to display the scene.

    \sa QGLAbstractScene
*/
void QDeclarativeMesh::setScene(QGLAbstractScene *scene)
{
    delete d->scene;
    resetSceneBranches();
    d->scene = scene;
    if (!d->scene) {
        qWarning("Could not load %s (possibly plugin could not be found)",
                 d->data.toString().toLatin1().constData());
    }
    else {
        QGLSceneNode *insertObject;
        initSceneObjectList();
        if (d->meshName.isEmpty())
            insertObject = getSceneObject();
        else
            insertObject = getSceneObject(d->meshName);
        //check if we found a valid/useful object to use based on the node name,
        //otherwise just output a list of valid names
        if (!insertObject && !d->meshName.isEmpty()) {
            qWarning() << "could not find" << d->meshName << "available:"
                       << getSceneObjectNames();
        } else if (!insertObject) {
            qWarning() << "could not find main object in scene!";
        }
        //in either case we still need to add an object to the scene, so if we fail
        //we simply add a null value to indicate that this object is non-drawable
        addSceneBranch(insertObject);

#ifndef QT_NO_DEBUG_STREAM
        if (insertObject && d->dumpInfo)
        {
            QGLSceneNode *node = qobject_cast<QGLSceneNode*>(insertObject);
            if (node)
                qDumpScene(node);
        }
#endif
    }
    emit dataChanged();
    d->loaded = true;
    if (d->completed)
        emit loaded();
}

/*!
    \internal
    When a scene has been deconstructed into several branches, each one being a separately
    manipulated sub-branch of the whole, the branches taken are identified numerically.  This
    function returns the next scene branch ID number
*/
int QDeclarativeMesh::nextSceneBranchId() const
{
    //Retrieve the next unused scene branch ID
    return d->nextSceneBranchId;
}

/*!
    \internal
    This function "prunes" a specific branch away from the main scene-tree/graph and adds it to the
    list of independent branches drawn by the mesh.  This facilitates animation & picking of specific
    sub-branches of the larger tree.

    The user specifies a sceneobject/node \a nodeName, which will form the "root" of the new branch,
    as well as a \a parent to which this branch shall be attached.  Normally the branch will simply
    be given the original scene as its parent, so that all standard object hierarchy manipulation/
    destruction rules apply, however this \a parent parameter gives the user extra flexibility if
    required.
*/
int QDeclarativeMesh::createSceneBranch(QString nodeName, QObject *parent)
{
    if (!d->scene) {
        qWarning() << "Unable to split mesh: no scene initialised - attempt to add scene object failed.";
        return -1;
    }
    else {
        int branchId = nextSceneBranchId();
        QGLSceneNode *sceneNode = getSceneObject(nodeName);
#ifndef QT_NO_DEBUG_STREAM
        if (d->dumpInfo)
            qDumpScene(sceneNode);
#endif
        if (sceneNode) {
            QGLSceneNode *parentNode = qobject_cast<QGLSceneNode *>(sceneNode->parent());
            QObject *prevParent=parentNode;
            if (parentNode)
                parentNode->removeNode(sceneNode);  //this becomes irrelevant.

            //sceneNode->setParent(d->scene);     //TODO: currently this fails as sceneNode changes make problems.
            //If no specific parent is nominated, use the scene specified by the mesh
            parent ? sceneNode->setParent(parent)  : sceneNode->setParent(d->scene);
            addSceneBranch(sceneNode, prevParent);

            return branchId;
        }
        else {
            qWarning() << "Warning: Unable to find node " << nodeName << " in given mesh.  Available nodes:"
                       << getSceneObjectNames();
            return -1;
        }
    }
}

/*!
    \internal
     Given a scene object \a rootSceneObject, this function adds a pointer to the object to an internally
     maintained map of scene branches.

     The \a previousParent of \a rootSceneObject is also stored so that the object can be restored to its
     previous parent if necessary.
*/
int QDeclarativeMesh::addSceneBranch(QGLSceneNode *rootSceneObject, QObject *previousParent)
{
    //when adding a new object to a mesh we also store the previous parent information
    //in case we wish to 'reattach' it to the parent at a later stage.
    QDeclarativeMeshPrivate::branchObject newBranch;
    newBranch.previousParent=previousParent;
    newBranch.rootSceneObject = rootSceneObject;

    d->sceneBranches.insert(d->nextSceneBranchId, newBranch);

    return ++d->nextSceneBranchId;
}

/*!
    \internal
    When an object is deleted or the user otherwise calls this function, the scene branch identified by
    \a branchId is reattached to the parent node from which it was originally pruned.

    If a problem occurs or the parent node is undefined (eg. null), an attempt is made to reattach it instead
    to the primary/default scene object/node, which corresponds to item 0 in the map.

    If the item selected is already the default item, and no parent can be found, it is deleted.
*/
void QDeclarativeMesh::restoreSceneBranch(int branchId)
{

    if (d->sceneBranches.contains(branchId)) {
        qWarning() <<"Mesh does not contain branch " << branchId<<".  Ignoring.\n";
        return;
    }

    QDeclarativeMeshPrivate::branchObject targetBranch = d->sceneBranches.value(branchId);

    if (!targetBranch.previousParent && branchId!=0) {
        targetBranch.rootSceneObject->setParent(getSceneObject());
    }
    else if (!targetBranch.previousParent){
        qWarning() << "Unable to find a parent to reattach default scene object to. Skipping.";
        targetBranch.rootSceneObject->setParent(d->scene);
    } else {
        targetBranch.rootSceneObject->setParent(targetBranch.previousParent);
    }

    if (!d->sceneBranches.remove(branchId)) {
        qWarning() << "Unable to remove branch "<<branchId<<" from Mesh. Ignoring.";
    }
}

/*!
    \internal
    Return a pointer to the scene branch identified by \a branchId.
*/
QGLSceneNode *QDeclarativeMesh::getSceneBranch(int branchId) const
{
    if (!d->sceneBranches.contains(branchId)) return NULL;
    QDeclarativeMeshPrivate::branchObject targetBranch = d->sceneBranches.value(branchId);
    return targetBranch.rootSceneObject;
}


/*!
    \internal
    This function iterates through the list of scene branches in the internal map and attempts to
    restore them to their original position in the object hierarchy of the scene.  It then clears the
    map ready for reuse.
*/
void QDeclarativeMesh::resetSceneBranches()
{
    //Delete all "hanging" object entries in the scene branches list and return them
    //to their rightful place in their parent scene if possible.
    for (int i=1; i<d->nextSceneBranchId; i++) {
        restoreSceneBranch(i);
    }

    d->sceneBranches.clear();
    d->nextSceneBranchId = 0;
}

/*!
    \internal
    Core "draw" function for the mesh draws scene branch \a branchId using \a painter.
*/
void QDeclarativeMesh::draw(QGLPainter *painter, int branchId)
{
    if (!d->sceneBranches.contains(branchId)) {
        qWarning() << "Unable to find valid scene object with ID: " << branchId;
    } else {
        QDeclarativeMeshPrivate::branchObject targetBranch = d->sceneBranches.value(branchId);
        targetBranch.rootSceneObject->draw(painter);
    }
}

/*!
    \internal
    Reference counting increment.
*/
void QDeclarativeMesh::ref()
{
    ++(d->refCount);
}

/*!
    \internal
    Reference counting decrement; returns true if there are still outstanding references
    to the class.
*/
bool QDeclarativeMesh::deref()
{
    --(d->refCount);
    return d->refCount > 0;
}


/*!
    Assign the mesh the material \a materialName from the \l QGLMaterialCollection class,
    and bind to the node identified by \a nodeName.

    Returns a pointer to the QGLMaterial material object \a materialName; or NULL if
    either there is no current scene, or if the scene does not contain such a material.
*/
QObject *QDeclarativeMesh::material(const QString& nodeName, const QString& materialName)
{
    if (!d->scene)
        return 0;

    QGLSceneNode *sceneObject;

    if (nodeName.isEmpty())
        sceneObject = getSceneObject();
    else
        sceneObject = getSceneObject(nodeName);

    if (!sceneObject || materialName.isEmpty()) {
        qWarning()  << "Attempt to get material data " <<materialName << " from scene node "
                    << nodeName <<" failed.";
        return NULL;
    }

    QGLSceneNode *node = qobject_cast<QGLSceneNode *>(sceneObject);

    QGLMaterialCollection *p = node->palette();

    QGLMaterial *params =  p->material(materialName);
    if (params && !d->connected.contains(params)) {
        d->connected.append(params);
        connect(params, SIGNAL(materialChanged()), this, SIGNAL(dataChanged()));
    }
    return params;
}

/*!
    \internal
*/
void QDeclarativeMesh::classBegin()
{
}

/*!
    \internal
    Checks that all loading and initialisation has been finished, and emits the  loaded() signal if
    the component is complete.

    \sa loaded()
*/
void QDeclarativeMesh::componentComplete()
{
    d->completed = true;
    if (d->loaded)
    {
        emit loaded();
        emit nodeChanged();
        emit dataChanged();
    }
}

/*!
  \qmlsignal Mesh::onLoaded()

  This handler is called when mesh loading is complete.
*/

QT_END_NAMESPACE
