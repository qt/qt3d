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

#ifndef QT3DEXTRAS_QPLANEGEOMETRY_H
#define QT3DEXTRAS_QPLANEGEOMETRY_H

#include <Qt3DExtras/qt3dextras_global.h>
#include <Qt3DRender/qgeometry.h>
#include <QtCore/QSize>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QAttribute;

} // Qt3DRender

namespace Qt3DExtras {

class QPlaneGeometryPrivate;

class Q_3DEXTRASSHARED_EXPORT QPlaneGeometry : public Qt3DRender::QGeometry
{
    Q_OBJECT
    Q_PROPERTY(float width READ width WRITE setWidth NOTIFY widthChanged)
    Q_PROPERTY(float height READ height WRITE setHeight NOTIFY heightChanged)
    Q_PROPERTY(QSize resolution READ resolution WRITE setResolution NOTIFY resolutionChanged)
    Q_PROPERTY(bool mirrored READ mirrored WRITE setMirrored NOTIFY mirroredChanged REVISION 9)
    Q_PROPERTY(Qt3DRender::QAttribute *positionAttribute READ positionAttribute CONSTANT)
    Q_PROPERTY(Qt3DRender::QAttribute *normalAttribute READ normalAttribute CONSTANT)
    Q_PROPERTY(Qt3DRender::QAttribute *texCoordAttribute READ texCoordAttribute CONSTANT)
    Q_PROPERTY(Qt3DRender::QAttribute *tangentAttribute READ tangentAttribute CONSTANT)
    Q_PROPERTY(Qt3DRender::QAttribute *indexAttribute READ indexAttribute CONSTANT)

public:
    explicit QPlaneGeometry(QNode *parent = nullptr);
    ~QPlaneGeometry();

    void updateVertices();
    void updateIndices();

    QSize resolution() const;
    float width() const;
    float height() const;
    bool mirrored() const;

    Qt3DRender::QAttribute *positionAttribute() const;
    Qt3DRender::QAttribute *normalAttribute() const;
    Qt3DRender::QAttribute *texCoordAttribute() const;
    Qt3DRender::QAttribute *tangentAttribute() const;
    Qt3DRender::QAttribute *indexAttribute() const;

public Q_SLOTS:
    void setResolution(const QSize &resolution);
    void setWidth(float width);
    void setHeight(float height);
    void setMirrored(bool mirrored);

Q_SIGNALS:
    void resolutionChanged(const QSize &resolution);
    void widthChanged(float width);
    void heightChanged(float height);
    void mirroredChanged(bool mirrored);

protected:
    QPlaneGeometry(QPlaneGeometryPrivate &dd, QNode *parent = nullptr);

private:
    Q_DECLARE_PRIVATE(QPlaneGeometry)
};

} // Qt3DExpoerimental

QT_END_NAMESPACE

#endif // QT3DEXTRAS_QPLANEGEOMETRY_H
