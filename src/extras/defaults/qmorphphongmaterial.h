// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DEXTRAS_QMORPHPHONGMATERIAL_H
#define QT3DEXTRAS_QMORPHPHONGMATERIAL_H

#include <Qt3DExtras/qt3dextras_global.h>
#include <Qt3DRender/qmaterial.h>
#include <QtGui/QColor>

QT_BEGIN_NAMESPACE

namespace Qt3DExtras {

class QMorphPhongMaterialPrivate;

class Q_3DEXTRASSHARED_EXPORT QMorphPhongMaterial : public Qt3DRender::QMaterial
{
    Q_OBJECT
    Q_PROPERTY(QColor ambient READ ambient WRITE setAmbient NOTIFY ambientChanged)
    Q_PROPERTY(QColor diffuse READ diffuse WRITE setDiffuse NOTIFY diffuseChanged)
    Q_PROPERTY(QColor specular READ specular WRITE setSpecular NOTIFY specularChanged)
    Q_PROPERTY(float shininess READ shininess WRITE setShininess NOTIFY shininessChanged)
    Q_PROPERTY(float interpolator READ interpolator WRITE setInterpolator NOTIFY interpolatorChanged)

public:
    explicit QMorphPhongMaterial(Qt3DCore::QNode *parent = nullptr);
    ~QMorphPhongMaterial();

    QColor ambient() const;
    QColor diffuse() const;
    QColor specular() const;
    float shininess() const;
    float interpolator() const;

public Q_SLOTS:
    void setAmbient(const QColor &ambient);
    void setDiffuse(const QColor &diffuse);
    void setSpecular(const QColor &specular);
    void setShininess(float shininess);
    void setInterpolator(float interpolator);

Q_SIGNALS:
    void ambientChanged(const QColor &ambient);
    void diffuseChanged(const QColor &diffuse);
    void specularChanged(const QColor &specular);
    void shininessChanged(float shininess);
    void interpolatorChanged(float interpolator);

private:
    Q_DECLARE_PRIVATE(QMorphPhongMaterial)
};

} // namespace Qt3DExtras

QT_END_NAMESPACE

#endif // QT3DEXTRAS_QMORPHPHONGMATERIAL_H

