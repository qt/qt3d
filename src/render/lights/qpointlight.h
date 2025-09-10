// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_QPOINTLIGHT_H
#define QT3DRENDER_QPOINTLIGHT_H

#include <Qt3DRender/qabstractlight.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QPointLightPrivate;

class Q_3DRENDERSHARED_EXPORT QPointLight : public QAbstractLight
{
    Q_OBJECT
    Q_PROPERTY(float constantAttenuation READ constantAttenuation WRITE setConstantAttenuation NOTIFY constantAttenuationChanged)
    Q_PROPERTY(float linearAttenuation READ linearAttenuation WRITE setLinearAttenuation NOTIFY linearAttenuationChanged)
    Q_PROPERTY(float quadraticAttenuation READ quadraticAttenuation WRITE setQuadraticAttenuation NOTIFY quadraticAttenuationChanged)

public:
    explicit QPointLight(Qt3DCore::QNode *parent = nullptr);
    ~QPointLight();

    float constantAttenuation() const;
    float linearAttenuation() const;
    float quadraticAttenuation() const;

public Q_SLOTS:
    void setConstantAttenuation(float value);
    void setLinearAttenuation(float value);
    void setQuadraticAttenuation(float value);

Q_SIGNALS:
    void constantAttenuationChanged(float constantAttenuation);
    void linearAttenuationChanged(float linearAttenuation);
    void quadraticAttenuationChanged(float quadraticAttenuation);

protected:
    Q_DECLARE_PRIVATE(QPointLight)
    QPointLight(QPointLightPrivate &dd, Qt3DCore::QNode *parent);
};

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QPOINTLIGHT_H
