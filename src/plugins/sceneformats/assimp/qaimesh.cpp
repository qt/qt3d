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

#include "qaimesh.h"
#include "qglscenenode.h"
#include "qglmaterialcollection.h"
#include "qglbuilder.h"

#include <QtGui/qmatrix4x4.h>
#include <QtCore/qmath.h>

#include "aiMesh.h"
#include "DefaultLogger.h"

QAiMesh::QAiMesh(aiMesh *mesh)
    : m_mesh(mesh)
{
}

QAiMesh::~QAiMesh()
{
}

static inline QVector2D qv2d(const aiVector3D &v)
{
    return QVector2D(v.x, v.y);
}

static inline QVector2D qv2d_inv(const aiVector3D &v)
{
    // invert the v coord because Qt is upside-down
    return QVector2D(v.x, (1.0 - v.y));
}

static inline QVector3D qv3d(const aiVector3D &v)
{
    return QVector3D(v.x, v.y, v.z);
}

void QAiMesh::loadTriangles(QGLBuilder &builder)
{
    QGeometryData data;
    for (unsigned int i = 0; i < m_mesh->mNumVertices; ++i)
        data.appendVertex(qv3d(m_mesh->mVertices[i]));
    if (m_mesh->HasNormals())
        for (unsigned int i = 0; i < m_mesh->mNumVertices; ++i)
            data.appendNormal(qv3d(m_mesh->mNormals[i]));
    int k = m_mesh->GetNumUVChannels();
    for (int t = 0; t < k; ++t)
    {
        if (m_mesh->mNumUVComponents[t] != 2)
            Assimp::DefaultLogger::get()->warn("Tex co-ords only supports U & V");
        QGLMaterial *m = builder.currentNode()->material();
        if (m && m->textureUrl().path().endsWith(".dds", Qt::CaseInsensitive))
        {
            for (unsigned int i = 0; i < m_mesh->mNumVertices; ++i)
                data.appendTexCoord(qv2d_inv(m_mesh->mTextureCoords[t][i]), static_cast<QGL::VertexAttribute>(QGL::TextureCoord0 + t));
        }
        else
        {
            for (unsigned int i = 0; i < m_mesh->mNumVertices; ++i)
                data.appendTexCoord(qv2d(m_mesh->mTextureCoords[t][i]), static_cast<QGL::VertexAttribute>(QGL::TextureCoord0 + t));
        }
    }

    for (unsigned int i = 0; i < m_mesh->mNumFaces; ++i)
    {
        aiFace *face = &m_mesh->mFaces[i];
        data.appendIndices(face->mIndices[0], face->mIndices[1], face->mIndices[2]);
    }

    // raw triangle mode
    builder.addTriangles(data);
}

void QAiMesh::build(QGLBuilder &builder, bool showWarnings)
{
    QGLSceneNode *node = builder.currentNode();
    QString name = node->objectName();

    if (!m_mesh->HasFaces() || !m_mesh->HasPositions())
    {
        if (showWarnings)
        {
            QString error = QLatin1String("Mesh %1 has zero vertex/face count");
            error.arg(name.isEmpty() ? QString(QLatin1String("<unnamed mesh>")) : name);
            Assimp::DefaultLogger::get()->warn(error.toStdString());
        }
        return;
    }

    node->setMaterialIndex(m_mesh->mMaterialIndex);
    node->palette()->markMaterialAsUsed(m_mesh->mMaterialIndex);

    if (m_mesh->mPrimitiveTypes & aiPrimitiveType_TRIANGLE)
        loadTriangles(builder);
    else
        return;  // TODO - lines, points, quads, polygons

    QGLMaterial * mat = node->palette()->material(m_mesh->mMaterialIndex);
    if (mat->property("isTwoSided").isValid() && mat->property("isTwoSided").toBool())
        node->setBackMaterialIndex(m_mesh->mMaterialIndex);
    if (mat->property("isWireFrame").isValid() && mat->property("isWireFrame").toBool())
        node->setDrawingMode(QGL::Lines);
}
