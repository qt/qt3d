// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DEXTRAS_QDIFFUSESPECULARMAPMATERIAL_H
#define QT3DEXTRAS_QDIFFUSESPECULARMAPMATERIAL_H

#include <Qt3DExtras/qt3dextras_global.h>
#include <Qt3DRender/qmaterial.h>
#include <QtGui/QColor>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QAbstractTexture;
} // namespace Qt3DRender

namespace Qt3DExtras {

class QDiffuseSpecularMapMaterialPrivate;

class Q_3DEXTRASSHARED_EXPORT QDiffuseSpecularMapMaterial : public Qt3DRender::QMaterial
{
    Q_OBJECT
    Q_PROPERTY(QColor ambient READ ambient WRITE setAmbient NOTIFY ambientChanged)
    Q_PROPERTY(float shininess READ shininess WRITE setShininess NOTIFY shininessChanged)
    Q_PROPERTY(Qt3DRender::QAbstractTexture *specular READ specular WRITE setSpecular NOTIFY specularChanged)
    Q_PROPERTY(Qt3DRender::QAbstractTexture *diffuse READ diffuse WRITE setDiffuse NOTIFY diffuseChanged)
    Q_PROPERTY(float textureScale READ textureScale WRITE setTextureScale NOTIFY textureScaleChanged)

public:
    explicit QDiffuseSpecularMapMaterial(Qt3DCore::QNode *parent = nullptr);
    ~QDiffuseSpecularMapMaterial();

    QColor ambient() const;
    Qt3DRender::QAbstractTexture *diffuse() const;
    Qt3DRender::QAbstractTexture *specular() const;
    float shininess() const;
    float textureScale() const;

public Q_SLOTS:
    void setAmbient(const QColor &ambient);
    void setDiffuse(Qt3DRender::QAbstractTexture *diffuse);
    void setSpecular(Qt3DRender::QAbstractTexture *specular);
    void setShininess(float shininess);
    void setTextureScale(float textureScale);

Q_SIGNALS:
    void ambientChanged(const QColor &ambient);
    void diffuseChanged(Qt3DRender::QAbstractTexture *diffuse);
    void specularChanged(Qt3DRender::QAbstractTexture *specular);
    void shininessChanged(float shininess);
    void textureScaleChanged(float textureScale);

private:
    Q_DECLARE_PRIVATE(QDiffuseSpecularMapMaterial)
};

} // Qt3DExtras

QT_END_NAMESPACE

#endif // QT3DEXTRAS_QDIFFUSESPECULARMAPMATERIAL_H
