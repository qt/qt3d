// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_QMESH_P_H
#define QT3DRENDER_QMESH_P_H

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

#include <Qt3DCore/private/qdownloadhelperservice_p.h>
#include <Qt3DRender/private/qgeometryrenderer_p.h>
#include <Qt3DRender/private/qt3drender_global_p.h>
#include <Qt3DRender/qmesh.h>
#include <QUrl>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {
class QDownloadHelperService;
}

namespace Qt3DRender {

namespace Render {
class NodeManagers;
}

class Q_3DRENDERSHARED_PRIVATE_EXPORT QMeshPrivate : public QGeometryRendererPrivate
{
public:
    QMeshPrivate();

    Q_DECLARE_PUBLIC(QMesh)
    static QMeshPrivate *get(QMesh *q);

    void setScene(Qt3DCore::QScene *scene) override;
    void updateFunctor();
    void setStatus(QMesh::Status status);

    QUrl m_source;
    QString m_meshName;
    QMesh::Status m_status;
};

class Q_AUTOTEST_EXPORT MeshDownloadRequest : public Qt3DCore::QDownloadRequest
{
public:
    MeshDownloadRequest(Qt3DCore::QNodeId mesh, QUrl source, Render::NodeManagers *managers);

    void onCompleted() override;

private:
    Qt3DCore::QNodeId m_mesh;
    Render::NodeManagers *m_nodeManagers;
};

class Q_AUTOTEST_EXPORT MeshLoaderFunctor : public Qt3DCore::QGeometryFactory
{
public :
    MeshLoaderFunctor(QMesh *mesh, const QByteArray &sourceData = QByteArray());

    void setNodeManagers(Render::NodeManagers *managers) { m_nodeManagers = managers; }
    Render::NodeManagers *nodeManagers() const { return m_nodeManagers; }

    void setDownloaderService(Qt3DCore::QDownloadHelperService *service) { m_downloaderService = service; }
    Qt3DCore::QDownloadHelperService *downloaderService() const { return m_downloaderService; }

    void setSourceData(const QByteArray &data) { m_sourceData = data; }
    QByteArray sourceData() const { return m_sourceData; }

    QUrl sourcePath() const { return m_sourcePath; }
    Qt3DCore::QNodeId mesh() const { return m_mesh; }
    QString meshName() const { return m_meshName; }

    QMesh::Status status() const { return m_status; }

    Qt3DCore::QGeometry *operator()() override;
    bool equals(const Qt3DCore::QGeometryFactory &other) const override;
    QT3D_FUNCTOR(MeshLoaderFunctor)

private:
    Qt3DCore::QNodeId m_mesh;
    QUrl m_sourcePath;
    QString m_meshName;
    QByteArray m_sourceData;
    Render::NodeManagers *m_nodeManagers;
    Qt3DCore::QDownloadHelperService *m_downloaderService;
    QMesh::Status m_status;
};


} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QMESH_P_H
