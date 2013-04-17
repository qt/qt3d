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

#include "qaiscene_p.h"
#include "qaimesh_p.h"
#include "qailoader_p.h"

#include <qglmaterialcollection.h>
#include <qglscenenode.h>

#include <QtCore/qsharedpointer.h>

QT_BEGIN_NAMESPACE

/*!
    \internal
    \class QAiScene
    \brief The QAiScene class manages and contains an asset importer scene.
    The scene consists of a QGLSceneNode object which is the top-level node
    and references the geometry imported from a file.
*/

/*!
    \internal
    Construct a new QAiScene object using the data in the \a scene,
    and setting the given \a parent.

    The QAiScene object takes ownership of the \a file.
*/
QAiScene::QAiScene(const aiScene *scene, QAiSceneHandler *handler)
    : QGLAbstractScene(0)
{
    Q_ASSERT(handler);
    Q_ASSERT(scene);
    QAiLoader loader(scene, handler);
    m_root = loader.loadMeshes();
    m_root->setParent(this);
    m_animations = loader.loadAnimations();
    m_aiLoader = 0;
}

/*!
    \internal
    Construct a new QAiScene object and setting the given \a handler.

    Note that the scene which will be used to generate the QAiScene has
    not been added yet - this is used in the network case and requires
    a later call to the loadScene function to work correctly

    \sa loadScene()
*/
QAiScene::QAiScene(QAiSceneHandler *handler)
    : QGLAbstractScene(0)
{
    Q_ASSERT(handler);
    //create a temporary loader and get a temporary root node for the scene.
    m_aiLoader = new QAiLoader(0, handler);
    m_root=m_aiLoader->m_builder.sceneNode();
}

/*!
    \internal
    \reimp
    Destroy this QAiScene, recovering all resources.
*/
QAiScene::~QAiScene()
{
    delete m_aiLoader;
}

/*!
    \internal
    \reimp
*/
QList<QObject *> QAiScene::objects() const
{
    QList<QObject *> objs;
    if (!m_root)
        return objs;
    objs.append(m_root);
    QList<QGLSceneNode*> children = m_root->allChildren();
    QList<QGLSceneNode*>::const_iterator it = children.constBegin();
    for ( ; it != children.constEnd(); ++it)
        objs.append(*it);
    return objs;
}

/*!
    \internal
    \reimp
*/
QGLSceneNode *QAiScene::mainNode() const
{
    return m_root;
}

/*!
    \internal
    Returns the aiLoader associated with the scene (if any).
*/
QAiLoader * QAiScene::aiLoader() const
{
    return m_aiLoader;
}

/*!
    \internal
    Load the new \a scene and swap the extant palette from the temporary roote
    node into the current root node.

    The QAiScene object takes ownership of the \a file.
*/
void QAiScene::loadScene(const aiScene *scene)
{
    Q_ASSERT(scene);

    //Get the old material information
    QSharedPointer<QGLMaterialCollection>oldPalette= m_aiLoader->m_builder.palette();
    int oldIndex = m_root->materialIndex();

    //Reset the palette for the root node.
    QSharedPointer<QGLMaterialCollection>newPalette = QSharedPointer<QGLMaterialCollection>(new QGLMaterialCollection());
    m_aiLoader->m_builder.sceneNode()->setPalette(newPalette);
    m_aiLoader->m_scene = scene;

    //Commence loading of the mesh.
    m_root = m_aiLoader->loadMeshes(); //this won't actually change the root unless we've messed up.
    delete m_aiLoader;
    m_aiLoader = 0;

    //Swap out the materials palette
    int indexCount=0;
    int materialCount=0;
    QGLMaterial * currentMaterial = NULL;
    do {
        currentMaterial = oldPalette->removeMaterial(indexCount);
        if (currentMaterial) {
            materialCount = m_root->palette()->addMaterial(currentMaterial);
            indexCount++;
        }
    } while (currentMaterial);
    m_root->setMaterialIndex(materialCount-(indexCount-1)+oldIndex);

    //update picking nodes for the whole scene if needed
    if (pickable()) generatePickNodes();

    emit sceneUpdated();
}

QList<QGLSceneAnimation *> QAiScene::animations() const
{
    return m_animations;
}

QT_END_NAMESPACE
