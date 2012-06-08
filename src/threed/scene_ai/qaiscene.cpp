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

#include "qaiscene.h"
#include "qaimesh.h"
#include "qailoader.h"

#include "qglscenenode.h"

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

const int g_MaxLevel = 16;
char g_Buffer[1 + g_MaxLevel*2];
void DumpSceneNode(int level, QGLSceneNode* pNode)
{
    if (pNode) {
        if (level > g_MaxLevel)
            level = g_MaxLevel;
        memset(g_Buffer,' ',level*2);
        g_Buffer[level*2] = '\0';
        qDebug("%snode(%p) name='%s'",g_Buffer,pNode,pNode->objectName().toLatin1().constData());
        if (!pNode->localTransform().isIdentity()) {
            qDebug("%s  transform",g_Buffer);
        }
        if (pNode->count() > 0) {
            qDebug("%s  geometry",g_Buffer);
        }
        foreach (QGLSceneNode* pChild, pNode->children()) {
            DumpSceneNode(level+1, pChild);
        }
    }
}

void DumpAnimations(const QList<QGLSceneAnimation *>& rAnimations)
{
    foreach (QGLSceneAnimation* pAnim, rAnimations) {
        qDebug("animation '%s':",pAnim->name().toLatin1().constData());
        QList<QString> nodes = pAnim->affectedNodes();
        foreach (QString node, nodes) {
            qDebug("  affects node '%s'",node.toLatin1().constData());
        }
    }
}

void DumpDefaultTransforms(const QMap<QGLSceneNode*,QGLSceneAnimation::NodeTransform>& rDefaultTransforms)
{
    for (QMap<QGLSceneNode*,QGLSceneAnimation::NodeTransform>::const_iterator It=rDefaultTransforms.begin(); It!=rDefaultTransforms.end(); ++It) {
        qDebug("got default transform for node '%s'",It.key()->objectName().toLatin1().constData());
    }
}

QAiScene::QAiScene(const aiScene *scene, QAiSceneHandler *handler)
    : QGLAbstractScene(0)
{
    Q_ASSERT(handler);
    Q_ASSERT(scene);
    QAiLoader loader(scene, handler);
    m_root = loader.loadMeshes();
    m_root->setParent(this);
    getAnimations() = loader.loadAnimations();
    getDefaultTransformations() = loader.loadDefaultTransformations();
    foreach (QGLSceneAnimation* pAnim, getAnimations()) {
        pAnim->setParent(this);
        QObject::connect(pAnim,SIGNAL(positionChanged()),this,SLOT(processAnimations()));
    }
    qDebug("=======================================");
    qDebug("loaded scene:");
    DumpSceneNode(1,m_root);
    DumpAnimations(getAnimations());
    DumpDefaultTransforms(getDefaultTransformations());
    qDebug("=======================================");
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

QT_END_NAMESPACE
