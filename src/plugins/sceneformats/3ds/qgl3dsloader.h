/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtQuick3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
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

class Lib3dsFile;
class Lib3dsMesh;
class Lib3dsMaterial;
class Lib3dsNode;
class QGL3dsMesh;
class QGLSceneNode;
class QGL3dsSceneHandler;

class QGL3dsLoader
{
public:
    QGL3dsLoader(Lib3dsFile *file, QGL3dsSceneHandler* sh);
    ~QGL3dsLoader();
    QGLSceneNode *loadMeshes();

private:
    void loadMesh(Lib3dsMesh *);
    void loadNodes(Lib3dsNode *, QGLSceneNode *);
    void loadMaterial(Lib3dsMaterial *);
    QUrl ensureResource(const QString &);
    void setUrl(const QUrl &url);

    Lib3dsFile *m_file;
    QGLSceneNode *m_rootNode;
    QGL3dsSceneHandler *m_scene;
    QList<QGLSceneNode *> m_nodes;
    QMap<QString, QGLSceneNode *> m_meshes;
    QUrl m_url;
    bool m_hasTextures;
    bool m_hasLitMaterials;
};

#endif // QGL3DSLOADER_H
