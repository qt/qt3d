// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DEXTRAS_QNORMALDIFFUSESPECULARMAPMATERIAL_H
#define QT3DEXTRAS_QNORMALDIFFUSESPECULARMAPMATERIAL_H

#include <Qt3DExtras/qt3dextras_global.h>
#include <Qt3DRender/qmaterial.h>
#include <QtGui/QColor>

QT_BEGIN_NAMESPACE

namespace Qt3DExtras {

class QNormalDiffuseSpecularMapMaterialPrivate;

class Q_3DEXTRASSHARED_EXPORT QNormalDiffuseSpecularMapMaterial : public Qt3DRender::QMaterial
{
    Q_OBJECT
    Q_PROPERTY(QColor ambient READ ambient WRITE setAmbient NOTIFY ambientChanged)
    Q_PROPERTY(Qt3DRender::QAbstractTexture *diffuse READ diffuse WRITE setDiffuse NOTIFY diffuseChanged)
    Q_PROPERTY(Qt3DRender::QAbstractTexture *normal READ normal WRITE setNormal NOTIFY normalChanged)
    Q_PROPERTY(Qt3DRender::QAbstractTexture *specular READ specular WRITE setSpecular NOTIFY specularChanged)
    Q_PROPERTY(float shininess READ shininess WRITE setShininess NOTIFY shininessChanged)
    Q_PROPERTY(float textureScale READ textureScale WRITE setTextureScale NOTIFY textureScaleChanged)

public:
    explicit QNormalDiffuseSpecularMapMaterial(Qt3DCore::QNode *parent = nullptr);
    ~QNormalDiffuseSpecularMapMaterial();

    QColor ambient() const;
    Qt3DRender::QAbstractTexture *diffuse() const;
    Qt3DRender::QAbstractTexture *normal() const;
    Qt3DRender::QAbstractTexture *specular() const;
    float shininess() const;
    float textureScale() const;

public Q_SLOTS:
    void setAmbient(const QColor &ambient);
    void setDiffuse(Qt3DRender::QAbstractTexture *diffuse);
    void setNormal(Qt3DRender::QAbstractTexture *normal);
    void setSpecular(Qt3DRender::QAbstractTexture *specular);
    void setShininess(float shininess);
    void setTextureScale(float textureScale);

Q_SIGNALS:
    void ambientChanged(const QColor &ambient);
    void diffuseChanged(Qt3DRender::QAbstractTexture *diffuse);
    void normalChanged(Qt3DRender::QAbstractTexture *normal);
    void specularChanged(Qt3DRender::QAbstractTexture *specular);
    void shininessChanged(float shininess);
    void textureScaleChanged(float textureScale);

protected:
    QNormalDiffuseSpecularMapMaterial(QNormalDiffuseSpecularMapMaterialPrivate &dd, Qt3DCore::QNode *parent = nullptr);

private:
    Q_DECLARE_PRIVATE(QNormalDiffuseSpecularMapMaterial)
};

} // namespace Qt3DExtras

QT_END_NAMESPACE

#endif // QT3DEXTRAS_QNORMALDIFFUSESPECULARMAPMATERIAL_H
