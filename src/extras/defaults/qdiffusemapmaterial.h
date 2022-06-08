// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DEXTRAS_QDIFFUSEMAPMATERIAL_H
#define QT3DEXTRAS_QDIFFUSEMAPMATERIAL_H

#include <Qt3DExtras/qt3dextras_global.h>
#include <Qt3DRender/qmaterial.h>
#include <QtGui/QColor>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QAbstractTexture;

} // namespace Qt3DRender

namespace Qt3DExtras {

class QDiffuseMapMaterialPrivate;

class Q_3DEXTRASSHARED_EXPORT QDiffuseMapMaterial : public Qt3DRender::QMaterial
{
    Q_OBJECT
    Q_PROPERTY(QColor ambient READ ambient WRITE setAmbient NOTIFY ambientChanged)
    Q_PROPERTY(QColor specular READ specular WRITE setSpecular NOTIFY specularChanged)
    Q_PROPERTY(float shininess READ shininess WRITE setShininess NOTIFY shininessChanged)
    Q_PROPERTY(Qt3DRender::QAbstractTexture *diffuse READ diffuse WRITE setDiffuse NOTIFY diffuseChanged)
    Q_PROPERTY(float textureScale READ textureScale WRITE setTextureScale NOTIFY textureScaleChanged)

public:
    explicit QDiffuseMapMaterial(Qt3DCore::QNode *parent = nullptr);
    ~QDiffuseMapMaterial();

    QColor ambient() const;
    QColor specular() const;
    float shininess() const;
    Qt3DRender::QAbstractTexture *diffuse() const;
    float textureScale() const;

public Q_SLOTS:
    void setAmbient(const QColor &color);
    void setSpecular(const QColor &specular);
    void setShininess(float shininess);
    void setDiffuse(Qt3DRender::QAbstractTexture *diffuse);
    void setTextureScale(float textureScale);

Q_SIGNALS:
    void ambientChanged(const QColor &ambient);
    void diffuseChanged(Qt3DRender::QAbstractTexture *diffuse);
    void specularChanged(const QColor &specular);
    void shininessChanged(float shininess);
    void textureScaleChanged(float textureScale);

private:
    Q_DECLARE_PRIVATE(QDiffuseMapMaterial)
};

} // Qt3DExtras

QT_END_NAMESPACE

#endif // QT3DEXTRASQDIFFUSEMAPMATERIAL_H
