/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
****************************************************************************/

#ifndef QT3DRENDER_QGEOMETRY_H
#define QT3DRENDER_QGEOMETRY_H

#include <Qt3DCore/qnode.h>
#include <Qt3DRender/qt3drender_global.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QAttribute;
class QGeometryPrivate;

class Q_3DRENDERSHARED_EXPORT QGeometry : public Qt3DCore::QNode
{
    Q_OBJECT
    Q_PROPERTY(Qt3DRender::QAttribute *boundingVolumePositionAttribute READ boundingVolumePositionAttribute WRITE setBoundingVolumePositionAttribute NOTIFY boundingVolumePositionAttributeChanged)
    Q_PROPERTY(QVector3D minExtent READ minExtent NOTIFY minExtentChanged REVISION 13)
    Q_PROPERTY(QVector3D maxExtent READ maxExtent NOTIFY maxExtentChanged REVISION 13)
public:
    explicit QGeometry(Qt3DCore::QNode *parent = nullptr);
    ~QGeometry();

    QVector<QAttribute *> attributes() const;
    Q_INVOKABLE void addAttribute(Qt3DRender::QAttribute *attribute);
    Q_INVOKABLE void removeAttribute(Qt3DRender::QAttribute *attribute);

    QAttribute *boundingVolumePositionAttribute() const;
    QVector3D minExtent() const;
    QVector3D maxExtent() const;

public Q_SLOTS:
    void setBoundingVolumePositionAttribute(QAttribute *boundingVolumePositionAttribute);

Q_SIGNALS:
    void boundingVolumePositionAttributeChanged(QAttribute *boundingVolumePositionAttribute);
    Q_REVISION(13) void minExtentChanged(const QVector3D &minExtent);
    Q_REVISION(13) void maxExtentChanged(const QVector3D &maxExtent);
protected:
    explicit QGeometry(QGeometryPrivate &dd, Qt3DCore::QNode *parent = nullptr);
    // TODO Unused remove in Qt6
    void sceneChangeEvent(const Qt3DCore::QSceneChangePtr &change) override;

private:
    Q_DECLARE_PRIVATE(QGeometry)
    Qt3DCore::QNodeCreatedChangeBasePtr createNodeCreationChange() const override;
};

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QGEOMETRY_H
