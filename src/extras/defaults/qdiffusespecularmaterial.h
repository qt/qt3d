/****************************************************************************
**
** Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
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
******************************************************************************/

#ifndef QT3DEXTRAS_QDIFFUSESPECULARMATERIAL_H
#define QT3DEXTRAS_QDIFFUSESPECULARMATERIAL_H

#include <Qt3DExtras/qt3dextras_global.h>
#include <Qt3DRender/qmaterial.h>
#include <QtGui/QColor>

QT_BEGIN_NAMESPACE

namespace Qt3DExtras {

class QDiffuseSpecularMaterialPrivate;

class Q_3DEXTRASSHARED_EXPORT QDiffuseSpecularMaterial : public Qt3DRender::QMaterial
{
    Q_OBJECT
    Q_PROPERTY(QColor ambient READ ambient WRITE setAmbient NOTIFY ambientChanged)
    Q_PROPERTY(QVariant diffuse READ diffuse WRITE setDiffuse NOTIFY diffuseChanged)
    Q_PROPERTY(QVariant specular READ specular WRITE setSpecular NOTIFY specularChanged)
    Q_PROPERTY(float shininess READ shininess WRITE setShininess NOTIFY shininessChanged)
    Q_PROPERTY(QVariant normal READ normal WRITE setNormal NOTIFY normalChanged)
    Q_PROPERTY(float textureScale READ textureScale WRITE setTextureScale NOTIFY textureScaleChanged)
    Q_PROPERTY(bool alphaBlending READ isAlphaBlendingEnabled WRITE setAlphaBlendingEnabled NOTIFY alphaBlendingEnabledChanged)

public:
    explicit QDiffuseSpecularMaterial(Qt3DCore::QNode *parent = nullptr);
    ~QDiffuseSpecularMaterial();

    QColor ambient() const;
    QVariant diffuse() const;
    QVariant specular() const;
    float shininess() const;
    QVariant normal() const;
    float textureScale() const;
    bool isAlphaBlendingEnabled() const;

public Q_SLOTS:
    void setAmbient(const QColor &ambient);
    void setDiffuse(const QVariant &diffuse);
    void setSpecular(const QVariant &specular);
    void setShininess(float shininess);
    void setNormal(const QVariant &normal);
    void setTextureScale(float textureScale);
    void setAlphaBlendingEnabled(bool enabled);

Q_SIGNALS:
    void ambientChanged(const QColor &ambient);
    void diffuseChanged(const QVariant &diffuse);
    void specularChanged(const QVariant &specular);
    void shininessChanged(float shininess);
    void normalChanged(const QVariant &normal);
    void textureScaleChanged(float textureScale);
    void alphaBlendingEnabledChanged(bool enabled);

private:
    Q_DECLARE_PRIVATE(QDiffuseSpecularMaterial)
};

} // namespace Qt3DExtras

QT_END_NAMESPACE

#endif // QT3DEXTRAS_QDIFFUSESPECULARMATERIAL_H
