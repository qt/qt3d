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

#ifndef ENTITY_H
#define ENTITY_H

#include <Qt3DCore/QEntity>
#include <QtGui/QColor>
#include <QtGui/QVector3D>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {
class QTransform;
}

namespace Qt3DRender {
class QCylinderMesh;
class QPhongMaterial;
}

QT_END_NAMESPACE

class Entity : public Qt3DCore::QEntity
{
    Q_OBJECT
    Q_PROPERTY(float theta READ theta WRITE setTheta NOTIFY thetaChanged)
    Q_PROPERTY(float phi READ phi WRITE setPhi NOTIFY phiChanged)
    Q_PROPERTY(QVector3D position READ position WRITE setPosition NOTIFY positionChanged)
    Q_PROPERTY(QColor diffuseColor READ diffuseColor WRITE setDiffuseColor NOTIFY diffuseColorChanged)

public:
    Entity(Qt3DCore::QNode *parent = 0);

    float theta() const;
    float phi() const;
    QVector3D position() const;
    QColor diffuseColor() const;

public slots:
    void setTheta(float theta);
    void setPhi(float phi);
    void setPosition(QVector3D position);
    void setDiffuseColor(QColor diffuseColor);

signals:
    void thetaChanged(float theta);
    void phiChanged(float phi);
    void positionChanged(QVector3D position);
    void diffuseColorChanged(QColor diffuseColor);

private:
    void updateTransform();

private:
    Qt3DCore::QTransform *m_transform;
    Qt3DRender::QCylinderMesh *m_mesh;
    Qt3DRender::QPhongMaterial *m_material;
    float m_theta;
    float m_phi;
    QVector3D m_position;
};

#endif // ENTITY_H
