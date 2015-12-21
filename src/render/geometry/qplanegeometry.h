/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QT3DRENDER_QPLANEGEOMETRY_H
#define QT3DRENDER_QPLANEGEOMETRY_H

#include <Qt3DRender/qgeometry.h>
#include <QSize>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QPlaneGeometryPrivate;
class QAttribute;

class QT3DRENDERSHARED_EXPORT QPlaneGeometry : public QGeometry
{
    Q_OBJECT
    Q_PROPERTY(float width READ width WRITE setWidth NOTIFY widthChanged)
    Q_PROPERTY(float height READ height WRITE setHeight NOTIFY heightChanged)
    Q_PROPERTY(QSize resolution READ resolution WRITE setResolution NOTIFY resolutionChanged)
    Q_PROPERTY(Qt3DRender::QAttribute *positionAttribute READ positionAttribute CONSTANT)
    Q_PROPERTY(Qt3DRender::QAttribute *normalAttribute READ normalAttribute CONSTANT)
    Q_PROPERTY(Qt3DRender::QAttribute *texCoordAttribute READ texCoordAttribute CONSTANT)
    Q_PROPERTY(Qt3DRender::QAttribute *tangentAttribute READ tangentAttribute CONSTANT)
    Q_PROPERTY(Qt3DRender::QAttribute *indexAttribute READ indexAttribute CONSTANT)

public:
    explicit QPlaneGeometry(QNode *parent = Q_NULLPTR);
    ~QPlaneGeometry();

    void updateVertices();
    void updateIndices();

    QSize resolution() const;
    float width() const;
    float height() const;

    QAttribute *positionAttribute() const;
    QAttribute *normalAttribute() const;
    QAttribute *texCoordAttribute() const;
    QAttribute *tangentAttribute() const;
    QAttribute *indexAttribute() const;

public Q_SLOTS:
    void setResolution(const QSize &resolution);
    void setWidth(float width);
    void setHeight(float height);

Q_SIGNALS:
    void resolutionChanged(const QSize &resolution);
    void widthChanged(float width);
    void heightChanged(float height);

protected:
    QPlaneGeometry(QPlaneGeometryPrivate &dd, QNode *parent = Q_NULLPTR);

private:
    Q_DECLARE_PRIVATE(QPlaneGeometry)
};

} // Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QPLANEGEOMETRY_H
