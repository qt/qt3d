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

#ifndef QGL3DSLOADER_H
#define QGL3DSLOADER_H

#include <QtCore/qurl.h>
#include <QtCore/qstring.h>
#include <QtCore/qmap.h>

#include "aiScene.h"

#include "qglbuilder.h"

class QAiMesh;
class QGLSceneNode;
class QAiSceneHandler;
class QGLMaterial;
class aiMaterial;
class QAiScene;
class QAiLoader
{
public:
    QAiLoader(const aiScene *scene, QAiSceneHandler* handler);
    ~QAiLoader();
    QGLSceneNode *loadMeshes();
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

    const aiScene *m_scene;
    QGLSceneNode *m_root;
    QAiSceneHandler *m_handler;
    QList<QGLSceneNode *> m_nodes;
    QList<QGLMaterial *> m_materials;
    QList<QGLSceneNode *> m_meshes;
    QMap<aiNode *, QGLSceneNode *> m_nodeMap;
    QMap<QGLSceneNode *, int> m_refCounts;
    bool m_hasTextures;
    bool m_hasLitMaterials;
    QGLBuilder m_builder;
};

#endif // QGL3DSLOADER_H
