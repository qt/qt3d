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

#include "qglmaterialcollection.h"
#include "qglmaterial_p.h"
#include <QtCore/qlist.h>
#include <QtCore/qhash.h>

QT_BEGIN_NAMESPACE

/*!
    \class QGLMaterialCollection
    \brief The QGLMaterialCollection class manages groups of materials.
    \since 4.8
    \ingroup qt3d
    \ingroup qt3d::enablers

    Managing more complex 3d graphics with several materials is easier when the
    materials can be referred to as a collection.  This is the role of the
    QGLMaterialCollection class.

    Plug-ins implementing 3D formats may make the materials defined in
    the format available to the application via a QGLMaterialCollection.

    The collection is also optimised for the case where many small objects
    must refer to materials - such as faces in a mesh, or particles.  In
    this case the materials can be specified as a short data type using an
    offset into the collection, rather than the material name.

    When building up a collection, meshes that refer to the various materials
    can check off which ones are used by calling markMaterialAsUsed(), and then
    remove spurious unused materials by calling removeUnusedMaterials().  This
    technique is suitable for models loaded from a model file where a large
    number of materials may be specified but only a few of those materials
    are used by the particular mesh selected from the scene.

    To make a material available from a collection, call addMaterial().  To
    retrieve a material from the collection call removeMaterial().

    The collection takes ownership of the QGLMaterial
    objects passed to it by the addMaterial() function.  These
    objects will be destroyed when the collection is destroyed.
*/

class QGLMaterialCollectionPrivate
{
public:
    QGLMaterialCollectionPrivate()
    {
    }

    QList<QGLMaterial *> materials;
    QHash<QString, int> materialNames;
};

/*!
    Construct a new empty QGLMaterialCollection object.  The \a parent
    is set as the parent of this object.
*/
QGLMaterialCollection::QGLMaterialCollection(QObject *parent)
    : QObject(parent)
    , d_ptr(new QGLMaterialCollectionPrivate)
{
}

/*!
    Destroy this collection.  All material objects referred to by this
    collection will be destroyed.
*/
QGLMaterialCollection::~QGLMaterialCollection()
{
    // The QGLMaterial QObject's are reparented to the collection
    // when addMaterial() is called, so the QObject destructor
    // will take care of cleaning them up for us.
}

/*!
    Returns a pointer to the material corresponding to \a index; or null
    if \a index is out of range or the material has been removed.

    Here's an example of searching for a material with a given ambient
    \c{color} in the collection \c{materials}:

    \code
    for (int colorIndex; colorIndex < materials->size(); ++colorIndex) {
        if (material(colorIndex) &&
                material(colorIndex)->ambientColor() == color)
            break;
    }
    if (colorIndex < materials->size())
        myObject->setMaterial(colorIndex);
    \endcode
*/
QGLMaterial *QGLMaterialCollection::material(int index) const
{
    Q_D(const QGLMaterialCollection);
    return d->materials.value(index, 0);
}

/*!
    \overload

    Returns the material associated with \a name in this collection;
    null if \a name is not present or the material has been removed.
*/
QGLMaterial *QGLMaterialCollection::material(const QString &name) const
{
    Q_D(const QGLMaterialCollection);
    int index = d->materialNames.value(name, -1);
    if (index >= 0)
        return d->materials[index];
    else
        return 0;
}

/*!
    Returns true if this collection contains \a material; false otherwise.

    \sa indexOf()
*/
bool QGLMaterialCollection::contains(QGLMaterial *material) const
{
    return material && material->d_func()->collection == this;
}

/*!
    \overload

    Returns true if this collection contains a material called \a name;
    false otherwise.

    \sa indexOf()
*/
bool QGLMaterialCollection::contains(const QString &name) const
{
    Q_D(const QGLMaterialCollection);
    return d->materialNames.contains(name);
}

/*!
    Returns the index of \a material in this collection; -1 if
    \a material is not present in this collection.

    \sa contains()
*/
int QGLMaterialCollection::indexOf(QGLMaterial *material) const
{
    if (material && material->d_func()->collection == this)
        return material->d_func()->index;
    else
        return -1;
}

/*!
    \overload

    Returns the index of the material called \a name in this collection;
    -1 if \a name is not present in this collection.

    \sa contains()
*/
int QGLMaterialCollection::indexOf(const QString &name) const
{
    Q_D(const QGLMaterialCollection);
    return d->materialNames.value(name, -1);
}

/*!
    Returns the name of the material at \a index in this material collection;
    a null QString if \a index is out of range.
*/
QString QGLMaterialCollection::materialName(int index) const
{
    Q_D(const QGLMaterialCollection);
    if (index >= 0 && index < d->materials.count()) {
        QGLMaterial *material = d->materials[index];
        if (material) {
            // Use the name in the private data block just in case the
            // application has modified objectName() since adding.
            return material->d_func()->name;
        }
    }
    return QString();
}

/*!
    Returns true if the material at \a index in this collection has been
    marked as used by markMaterialAsUsed().

    \sa markMaterialAsUsed()
*/
bool QGLMaterialCollection::isMaterialUsed(int index) const
{
    QGLMaterial *mat = material(index);
    if (mat)
        return mat->d_func()->used;
    else
        return false;
}

/*!
    Flags the material corresponding to the \a index as used.  Some model files
    may contain a range of materials, applying to various objects in the scene.

    When a particular object is loaded from the file, many of those
    materials may not be used in that object.  This wastes space,
    with many spurious materials being stored.

    Use this method during model loading or construction to mark off
    materials that have been used.  Materials so marked will not
    be removed by removeUnusedMaterials().

    \sa removeUnusedMaterials(), isMaterialUsed()
*/
void QGLMaterialCollection::markMaterialAsUsed(int index)
{
    QGLMaterial *mat = material(index);
    if (mat)
        mat->d_func()->used = true;
}

/*!
    Removes and deletes materials which have not been marked as used.

    \sa markMaterialAsUsed(), isMaterialUsed()
*/
void QGLMaterialCollection::removeUnusedMaterials()
{
    Q_D(QGLMaterialCollection);
    for (int index = 0; index < d->materials.size(); ++index) {
        QGLMaterial *material = d->materials[index];
        if (material && !material->d_func()->used)
            delete removeMaterial(index);
    }
}

/*!
    Adds \a material to this collection and returns its new index.  The
    collection takes ownership of the material and will delete it when the
    collection is destroyed.  Initially the \a material is marked as unused.

    The QObject::objectName() of \a material at the time addMaterial()
    is called will be used as the material's name within this collection.
    Changes to the object name after the material is added are ignored.

    If \a material is already present in this collection, then this
    function will return the index that was previously assigned.

    Returns -1 if \a material has been added to another collection.

    \sa removeMaterial(), markMaterialAsUsed()
*/
int QGLMaterialCollection::addMaterial(QGLMaterial *material)
{
    Q_D(QGLMaterialCollection);
    Q_ASSERT(material);

    // Allocate a new index for the material.
    int index = d->materials.count();

    // Record the index in the private data attached to the material.
    // This allows us to find the material's index quickly later.
    QGLMaterialPrivate *dm = material->d_func();
    if (dm->collection) {
        if (dm->collection == this)
            return dm->index;
        return -1;
    }
    dm->collection = this;
    dm->index = index;
    dm->name = material->objectName();
    dm->used = false;

    // Add the material to this collection.
    material->setParent(this);
    d->materials.append(material);
    if (!dm->name.isEmpty())
        d->materialNames[dm->name] = index;
    connect(material, SIGNAL(destroyed()), this, SLOT(materialDeleted()));
    return index;
}

/*!
    Removes all instances of \a material from this collection.
    The \a material object is not deleted and can be reused.

    Does nothing if \a material is null or not a member of
    this collection.

    \sa addMaterial()
*/
void QGLMaterialCollection::removeMaterial(QGLMaterial *material)
{
    Q_D(QGLMaterialCollection);
    if (!material)
        return;

    // Check the material's owning collection.
    QGLMaterialPrivate *dm = material->d_func();
    if (dm->collection != this)
        return;

    // Remove the material from the collection.
    d->materials[dm->index] = 0;
    if (!dm->name.isEmpty())
        d->materialNames.remove(dm->name);
    material->setParent(0);

    // Detach from the owning collection.
    dm->collection = 0;
    dm->index = -1;
}

/*!
    Removes the material at \a index from this collection, and returns
    a pointer to the material.

    Since the collection is designed for fast lookup by index, the
    the stored material pointer is set to null but the \a index
    otherwise remains valid.
*/
QGLMaterial *QGLMaterialCollection::removeMaterial(int index)
{
    Q_D(QGLMaterialCollection);

    // Bail out if the material is invalid.
    if (index < 0 || index >= d->materials.count())
        return 0;
    QGLMaterial *material = d->materials[index];
    if (!material)
        return 0;

    // Remove the material from the collection.
    QGLMaterialPrivate *dm = material->d_func();
    d->materials[index] = 0;
    if (!dm->name.isEmpty())
        d->materialNames.remove(dm->name);
    material->setParent(0);

    // Detach from the owning collection.
    dm->collection = 0;
    dm->index = -1;
    return material;
}

/*!
    Returns true if this collection is empty, false otherwise.

    \sa size()
*/
bool QGLMaterialCollection::isEmpty() const
{
    Q_D(const QGLMaterialCollection);
    return d->materials.isEmpty();
}

/*!
    Returns the number of (possibly null) materials in this collection.
    Null materials result from calling removeMaterial().

    \sa isEmpty()
*/
int QGLMaterialCollection::size() const
{
    Q_D(const QGLMaterialCollection);
    return d->materials.size();
}

/*!
    \internal
    Responds to the destroyed() signal by calling removeMaterial() on the
    material about to be deleted;
*/
void QGLMaterialCollection::materialDeleted()
{
    removeMaterial(qobject_cast<QGLMaterial *>(sender()));
}
