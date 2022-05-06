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
******************************************************************************/

#ifndef QT3DCORE_QGEOMETRY_P_H
#define QT3DCORE_QGEOMETRY_P_H

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

#include <Qt3DCore/private/qt3dcore_global_p.h>
#include <Qt3DCore/private/qnode_p.h>
#include <Qt3DCore/qgeometry.h>
#include <QVector3D>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

class QAttribute;

class Q_3DCORESHARED_EXPORT QGeometryPrivate : public QNodePrivate
{
public:
    Q_DECLARE_PUBLIC(QGeometry)
    QGeometryPrivate();
    ~QGeometryPrivate();

    void setScene(QScene *scene) override;
    void update() override;
    void setExtent(const QVector3D &minExtent, const QVector3D &maxExtent);
    static QGeometryPrivate *get(QGeometry *q);

    QList<QAttribute *> m_attributes;
    QAttribute *m_boundingVolumePositionAttribute;
    QVector3D m_minExtent;
    QVector3D m_maxExtent;
    bool m_dirty;
};

} // namespace Qt3DCore

QT_END_NAMESPACE

#endif // QT3DCORE_QGEOMETRY_P_H

