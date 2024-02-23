// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#ifndef BOXENTITY_H
#define BOXENTITY_H

#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DExtras/QCuboidMesh>
#include <Qt3DExtras/QPhongMaterial>

class BoxEntity : public Qt3DCore::QEntity
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
    void diffuseColorChanged(const QColor &);
    void angleChanged();
    void radiusChanged();

private:
    void updateTransformation();

    Qt3DCore::QTransform *m_transform;
    Qt3DExtras::QCuboidMesh *m_mesh;
    Qt3DExtras::QPhongMaterial *m_material;
    float m_angle;
    float m_radius;
};

#endif // BOXENTITY_H
