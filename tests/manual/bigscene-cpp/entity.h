// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

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
class QEffect;
class QGeometryRenderer;
class QMaterial;
class QParameter;
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
    Entity(Qt3DRender::QEffect *effect, Qt3DCore::QNode *parent = 0);

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
    Qt3DRender::QMaterial *m_material;
    Qt3DRender::QParameter *m_diffuseColorParam;
    float m_theta;
    float m_phi;
    QVector3D m_position;
};

#endif // ENTITY_H
