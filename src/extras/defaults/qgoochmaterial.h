// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DEXTRAS_QGOOCHMATERIAL_H
#define QT3DEXTRAS_QGOOCHMATERIAL_H

#include <Qt3DExtras/qt3dextras_global.h>
#include <Qt3DRender/qmaterial.h>
#include <QtGui/QColor>

QT_BEGIN_NAMESPACE

namespace Qt3DExtras {

class QGoochMaterialPrivate;

class Q_3DEXTRASSHARED_EXPORT QGoochMaterial : public Qt3DRender::QMaterial
{
    Q_OBJECT
    Q_PROPERTY(QColor diffuse READ diffuse WRITE setDiffuse NOTIFY diffuseChanged)
    Q_PROPERTY(QColor specular READ specular WRITE setSpecular NOTIFY specularChanged)
    Q_PROPERTY(QColor cool READ cool WRITE setCool NOTIFY coolChanged)
    Q_PROPERTY(QColor warm READ warm WRITE setWarm NOTIFY warmChanged)
    Q_PROPERTY(float alpha READ alpha WRITE setAlpha NOTIFY alphaChanged)
    Q_PROPERTY(float beta READ beta WRITE setBeta NOTIFY betaChanged)
    Q_PROPERTY(float shininess READ shininess WRITE setShininess NOTIFY shininessChanged)

public:
    explicit QGoochMaterial(Qt3DCore::QNode *parent = nullptr);
    ~QGoochMaterial();

    QColor diffuse() const;
    QColor specular() const;
    QColor cool() const;
    QColor warm() const;
    float alpha() const;
    float beta() const;
    float shininess() const;

public Q_SLOTS:
    void setDiffuse(const QColor &diffuse);
    void setSpecular(const QColor &specular);
    void setCool(const QColor &cool);
    void setWarm(const QColor &warm);
    void setAlpha(float alpha);
    void setBeta(float beta);
    void setShininess(float shininess);

Q_SIGNALS:
    void diffuseChanged(const QColor &diffuse);
    void specularChanged(const QColor &specular);
    void coolChanged(const QColor &cool);
    void warmChanged(const QColor &warm);
    void alphaChanged(float alpha);
    void betaChanged(float beta);
    void shininessChanged(float shininess);

protected:
    explicit QGoochMaterial(QGoochMaterialPrivate &dd, Qt3DCore::QNode *parent = nullptr);

private:
    Q_DECLARE_PRIVATE(QGoochMaterial)
};

} // namespace Qt3DExtras

QT_END_NAMESPACE

#endif // QT3DEXTRAS_QGOOCHMATERIAL_H
