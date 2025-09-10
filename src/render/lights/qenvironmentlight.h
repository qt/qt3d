// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_QENVIRONMENTLIGHT_H
#define QT3DRENDER_QENVIRONMENTLIGHT_H

#include <Qt3DRender/qt3drender_global.h>
#include <Qt3DCore/qcomponent.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QAbstractTexture;
class QEnvironmentLightPrivate;

class Q_3DRENDERSHARED_EXPORT QEnvironmentLight : public Qt3DCore::QComponent
{
    Q_OBJECT
    Q_PROPERTY(Qt3DRender::QAbstractTexture *irradiance READ irradiance WRITE setIrradiance NOTIFY irradianceChanged)
    Q_PROPERTY(Qt3DRender::QAbstractTexture *specular READ specular WRITE setSpecular NOTIFY specularChanged)

public:
    explicit QEnvironmentLight(Qt3DCore::QNode *parent = nullptr);
    ~QEnvironmentLight();

    Qt3DRender::QAbstractTexture *irradiance() const;
    Qt3DRender::QAbstractTexture *specular() const;

public Q_SLOTS:
    void setIrradiance(Qt3DRender::QAbstractTexture *irradiance);
    void setSpecular(Qt3DRender::QAbstractTexture *specular);

protected:
    explicit QEnvironmentLight(QEnvironmentLightPrivate &dd, Qt3DCore::QNode *parent = nullptr);

Q_SIGNALS:
    void irradianceChanged(Qt3DRender::QAbstractTexture *environmentIrradiance);
    void specularChanged(Qt3DRender::QAbstractTexture *environmentSpecular);

private:
    Q_DECLARE_PRIVATE(QEnvironmentLight)

    Q_PRIVATE_SLOT(d_func(), void _q_updateEnvMapsSize())
};

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QENVIRONMENTLIGHT_H
