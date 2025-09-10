// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DEXTRAS_QNORMALDIFFUSEMAPMATERIAL_H
#define QT3DEXTRAS_QNORMALDIFFUSEMAPMATERIAL_H

#include <Qt3DExtras/qt3dextras_global.h>
#include <Qt3DRender/qmaterial.h>
#include <QtGui/QColor>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QAbstractTexture;

} // namespace Qt3DRender

namespace Qt3DExtras {

class QNormalDiffuseMapMaterialPrivate;

class Q_3DEXTRASSHARED_EXPORT QNormalDiffuseMapMaterial : public Qt3DRender::QMaterial
{
    Q_OBJECT
    Q_PROPERTY(QColor ambient READ ambient WRITE setAmbient NOTIFY ambientChanged)
    Q_PROPERTY(QColor specular READ specular WRITE setSpecular NOTIFY specularChanged)
    Q_PROPERTY(Qt3DRender::QAbstractTexture *diffuse READ diffuse WRITE setDiffuse NOTIFY diffuseChanged)
    Q_PROPERTY(Qt3DRender::QAbstractTexture *normal READ normal WRITE setNormal NOTIFY normalChanged)
    Q_PROPERTY(float shininess READ shininess WRITE setShininess NOTIFY shininessChanged)
    Q_PROPERTY(float textureScale READ textureScale WRITE setTextureScale NOTIFY textureScaleChanged)

public:
    explicit QNormalDiffuseMapMaterial(Qt3DCore::QNode *parent = nullptr);
    ~QNormalDiffuseMapMaterial();

    QColor ambient() const;
    QColor specular() const;
    Qt3DRender::QAbstractTexture *diffuse() const;
    Qt3DRender::QAbstractTexture *normal() const;
    float shininess() const;
    float textureScale() const;

public Q_SLOTS:
    void setAmbient(const QColor &ambient);
    void setSpecular(const QColor &specular);
    void setDiffuse(Qt3DRender::QAbstractTexture *diffuse);
    void setNormal(Qt3DRender::QAbstractTexture *normal);
    void setShininess(float shininess);
    void setTextureScale(float textureScale);

Q_SIGNALS:
    void ambientChanged(const QColor &ambient);
    void diffuseChanged(Qt3DRender::QAbstractTexture *diffuse);
    void normalChanged(Qt3DRender::QAbstractTexture *normal);
    void specularChanged(const QColor &specular);
    void shininessChanged(float shininess);
    void textureScaleChanged(float textureScale);

protected:
    QNormalDiffuseMapMaterial(QNormalDiffuseMapMaterialPrivate &dd, Qt3DCore::QNode *parent = nullptr);

private:
    Q_DECLARE_PRIVATE(QNormalDiffuseMapMaterial)
};

} // Qt3DExtras

QT_END_NAMESPACE

#endif // QT3DEXTRAS_QNORMALDIFFUSEMAPMATERIAL_H
