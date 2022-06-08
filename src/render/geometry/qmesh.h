// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_QMESH_H
#define QT3DRENDER_QMESH_H

#include <Qt3DRender/qt3drender_global.h>
#include <Qt3DRender/qgeometryrenderer.h>
#include <QtCore/QUrl>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QMeshPrivate;

/**
* @brief Simple static mesh
*
*/
class Q_3DRENDERSHARED_EXPORT QMesh : public QGeometryRenderer
{
    Q_OBJECT
    Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(QString meshName READ meshName WRITE setMeshName NOTIFY meshNameChanged)
    Q_PROPERTY(Status status READ status NOTIFY statusChanged REVISION 11)
public:
    explicit QMesh(Qt3DCore::QNode *parent = nullptr);
    ~QMesh();

    enum Status {
        None = 0,
        Loading,
        Ready,
        Error
    };
    Q_ENUM(Status) // LCOV_EXCL_LINE


    QUrl source() const;
    QString meshName() const;
    Status status() const;

public Q_SLOTS:
    void setSource(const QUrl &source);
    void setMeshName(const QString &meshName);

Q_SIGNALS:
    void sourceChanged(const QUrl &source);
    void meshNameChanged(const QString &meshName);
    void statusChanged(Status status);

protected:
    explicit QMesh(QMeshPrivate &dd, Qt3DCore::QNode *parent = nullptr);

private:
    Q_DECLARE_PRIVATE(QMesh)
};

}

QT_END_NAMESPACE

#endif // of QT3DRENDER_QMESH_H
