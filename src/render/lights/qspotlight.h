/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
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
