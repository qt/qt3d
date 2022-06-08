// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_QSPOTLIGHT_H
#define QT3DRENDER_QSPOTLIGHT_H

#include <Qt3DRender/qabstractlight.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QSpotLightPrivate;

class Q_3DRENDERSHARED_EXPORT QSpotLight : public QAbstractLight
{
    Q_OBJECT
    Q_PROPERTY(float constantAttenuation READ constantAttenuation WRITE setConstantAttenuation NOTIFY constantAttenuationChanged)
    Q_PROPERTY(float linearAttenuation READ linearAttenuation WRITE setLinearAttenuation NOTIFY linearAttenuationChanged)
    Q_PROPERTY(float quadraticAttenuation READ quadraticAttenuation WRITE setQuadraticAttenuation NOTIFY quadraticAttenuationChanged)
    Q_PROPERTY(QVector3D localDirection READ localDirection WRITE setLocalDirection NOTIFY localDirectionChanged)
    Q_PROPERTY(float cutOffAngle READ cutOffAngle WRITE setCutOffAngle NOTIFY cutOffAngleChanged)

public:
    explicit QSpotLight(Qt3DCore::QNode *parent = nullptr);
    ~QSpotLight();

    QVector3D localDirection() const;
    float cutOffAngle() const;

    float constantAttenuation() const;
    float linearAttenuation() const;
    float quadraticAttenuation() const;

public Q_SLOTS:
    void setConstantAttenuation(float value);
    void setLinearAttenuation(float value);
    void setQuadraticAttenuation(float value);
    void setLocalDirection(const QVector3D &localDirection);
    void setCutOffAngle(float cutOffAngle);

Q_SIGNALS:
    void constantAttenuationChanged(float constantAttenuation);
    void linearAttenuationChanged(float linearAttenuation);
    void quadraticAttenuationChanged(float quadraticAttenuation);
    void localDirectionChanged(const QVector3D &localDirection);
    void cutOffAngleChanged(float cutOffAngle);

protected:
    explicit QSpotLight(QSpotLightPrivate &dd, Qt3DCore::QNode *parent = nullptr);

private:
    Q_DECLARE_PRIVATE(QSpotLight)
};

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QSPOTLIGHT_H
