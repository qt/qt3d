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

#ifndef BOXENTITY_H
#define BOXENTITY_H

#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DCore/QTranslateTransform>
#include <Qt3DCore/QScaleTransform>
#include <Qt3DRenderer/QCuboidMesh>
#include <Qt3DRenderer/QPhongMaterial>

class BoxEntity : public Qt3D::QEntity
{
    Q_OBJECT

    Q_PROPERTY(QColor diffuseColor READ diffuseColor WRITE setDiffuseColor NOTIFY diffuseColorChanged)
    Q_PROPERTY(float angle READ angle WRITE setAngle NOTIFY angleChanged)
    Q_PROPERTY(float radius READ radius WRITE setRadius NOTIFY radiusChanged)

public:
    BoxEntity(QNode *parent = 0);

    QColor diffuseColor();
    float angle() const;
    float radius() const;

public Q_SLOTS:
    void setDiffuseColor(const QColor &diffuseColor);
    void setAngle(float arg);
    void setRadius(float arg);

Q_SIGNALS:
    void diffuseColorChanged();
    void angleChanged();
    void radiusChanged();

private:
    void updateTransformation();

    Qt3D::QTransform *m_transform;
    Qt3D::QTranslateTransform *m_translate;
    Qt3D::QScaleTransform *m_scale;
    Qt3D::QCuboidMesh *m_mesh;
    Qt3D::QPhongMaterial *m_material;
    float m_angle;
    float m_radius;
};

#endif // BOXENTITY_H
