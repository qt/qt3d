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

#ifndef QAILOADER_H
#define QAILOADER_H

#include <QtCore/qurl.h>
#include <QtCore/qstring.h>
#include <QtCore/qmap.h>

#include <Qt3D/qglbuilder.h>

struct aiMaterial;
struct aiMesh;
struct aiNode;
struct aiScene;

QT_BEGIN_NAMESPACE

class QAiMesh;
class QAiScene;
class QGLSceneNode;
class QAiSceneHandler;
class QGLSceneAnimation;
class QGLMaterial;

class QAiLoader
{
public:
    QAiLoader(const aiScene *scene, QAiSceneHandler* handler);
    ~QAiLoader();
    QGLSceneNode *loadMeshes();
    QList<QGLSceneAnimation *> loadAnimations();

private:
    friend class QAiScene;

    void loadMesh(aiMesh *);
    void loadNodes(aiNode *, QGLSceneNode *);
    void loadMaterial(aiMaterial *);
    void loadTextures(aiMaterial *, QGLMaterial *);
    QUrl ensureResource(const QString &);
    void optimizeData();
    void optimizeNodes(QGLSceneNode *node = 0, QGLSceneNode *parent = 0);
    void countChildNodeReferences();
    void setEffectRecursive(QGLSceneNode *node);

    const aiScene *m_scene;
    QGLSceneNode *m_root;
    QAiSceneHandler *m_handler;
    QList<QGLSceneNode *> m_nodes;
    QList<QGLMaterial *> m_materials;
    QList<QGLSceneNode *> m_meshes;
    QMap<aiNode *, QGLSceneNode *> m_nodeMap;
    QMap<QGLSceneNode *, int> m_refCounts;
    QList<QGLSceneAnimation *> m_animations;
    bool m_hasTextures;
    bool m_hasLitMaterials;
    QGLBuilder m_builder;
};

QT_END_NAMESPACE

#endif // QAILOADER_H
