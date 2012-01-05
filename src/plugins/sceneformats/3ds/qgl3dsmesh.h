/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef QGL3DSMESH_H
#define QGL3DSMESH_H

#include "qglbuilder.h"
#include "qgl3dsscenehandler.h"

#include <lib3ds/types.h>
#include <lib3ds/mesh.h>

class QGLMaterialCollection;
class QGL3dsLoader;
class Lib3dsMesh;
class ModulateRecord;

class QGL3dsMesh : public QGLBuilder
{
public:
    explicit QGL3dsMesh(Lib3dsMesh *mesh, QGLMaterialCollection *materials);
    virtual ~QGL3dsMesh();
    void initialize();
    void setOptions(QGL::ModelOptions options) { m_options = options; }
    bool hasTexture() { return m_hasTextures; }

    typedef Lib3dsFace *FacePtr;

protected:
    void analyzeMesh();
    void modulateMesh();
    void checkTextures(int);
    QMatrix4x4 meshMatrix() const;
    void generateVertices();

private:
    void processNodeForMaterial(int matIx, QGLSceneNode *node);
    QArray<int> mapFacesToVerts(Lib3dsDword *allKeys);
    void addToAdjacencyMap(Lib3dsFace *face, Lib3dsFace *nbr);
    void buildAdjacencyMap(const QArray<int> &vlist);
    int cachedMaterialLookup(const char *material);
    void findCommonNormal(ModulateRecord *mod) const;
    void initAdjacencyMap();

    Lib3dsMesh *m_mesh;
    bool m_hasTextures;
    Lib3dsDword m_smoothingGroups;
    int m_smoothingGroupCount;
    QSet<int> m_plainMaterials;
    QSet<int> m_textureMaterials;
    bool m_texFlip;
    bool m_hasZeroSmoothing;
    FacePtr *m_faceMap;
    QGL::ModelOptions m_options;
};

#endif // QGL3DSMESH_H
