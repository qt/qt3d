// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef FBXGEOMETRYLOADER_H
#define FBXGEOMETRYLOADER_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of other Qt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <Qt3DRender/private/qgeometryloaderinterface_p.h>

#include <private/qlocale_tools_p.h>

#include <fbxsdk.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

#define FBXGEOMETRYLOADER_EXT QLatin1String("fbx")

class FbxGeometryLoader : public QGeometryLoaderInterface
{
    Q_OBJECT
public:
    FbxGeometryLoader();
    ~FbxGeometryLoader();

    QGeometry *geometry() const final;

    bool load(QIODevice *ioDev, const QString &subMesh = QString()) final;

protected:
    void recurseNodes();
    void processNode(FbxNode *node);
    void processMesh(FbxMesh *mesh);

private:
    FbxManager *m_manager;
    FbxScene *m_scene;
    QGeometry *m_geometry;
    QString m_mesh;
};

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // FBXGEOMETRYLOADER_H
