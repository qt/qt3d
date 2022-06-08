// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DEXTRAS_QPHONGALPHAMATERIAL_H
#define QT3DEXTRAS_QPHONGALPHAMATERIAL_H

#include <Qt3DExtras/qt3dextras_global.h>
#include <Qt3DRender/qblendequation.h>
#include <Qt3DRender/qblendequationarguments.h>
#include <Qt3DRender/qmaterial.h>
#include <QtGui/QColor>

QT_BEGIN_NAMESPACE

namespace Qt3DExtras {

class QPhongAlphaMaterialPrivate;

class Q_3DEXTRASSHARED_EXPORT QPhongAlphaMaterial : public Qt3DRender::QMaterial
{
    Q_OBJECT
    Q_PROPERTY(QColor ambient READ ambient WRITE setAmbient NOTIFY ambientChanged)
    Q_PROPERTY(QColor diffuse READ diffuse WRITE setDiffuse NOTIFY diffuseChanged)
    Q_PROPERTY(QColor specular READ specular WRITE setSpecular NOTIFY specularChanged)
    Q_PROPERTY(float shininess READ shininess WRITE setShininess NOTIFY shininessChanged)
    Q_PROPERTY(float alpha READ alpha WRITE setAlpha NOTIFY alphaChanged)
    Q_PROPERTY(Qt3DRender::QBlendEquationArguments::Blending sourceRgbArg READ sourceRgbArg WRITE setSourceRgbArg NOTIFY sourceRgbArgChanged)
    Q_PROPERTY(Qt3DRender::QBlendEquationArguments::Blending destinationRgbArg READ destinationRgbArg WRITE setDestinationRgbArg NOTIFY destinationRgbArgChanged)
    Q_PROPERTY(Qt3DRender::QBlendEquationArguments::Blending sourceAlphaArg READ sourceAlphaArg WRITE setSourceAlphaArg NOTIFY sourceAlphaArgChanged)
    Q_PROPERTY(Qt3DRender::QBlendEquationArguments::Blending destinationAlphaArg READ destinationAlphaArg WRITE setDestinationAlphaArg NOTIFY destinationAlphaArgChanged)
    Q_PROPERTY(Qt3DRender::QBlendEquation::BlendFunction blendFunctionArg READ blendFunctionArg WRITE setBlendFunctionArg NOTIFY blendFunctionArgChanged)

public:
    explicit QPhongAlphaMaterial(Qt3DCore::QNode *parent = nullptr);
    ~QPhongAlphaMaterial();

    QColor ambient() const;
    QColor diffuse() const;
    QColor specular() const;
    float shininess() const;
    float alpha() const;

    Qt3DRender::QBlendEquationArguments::Blending sourceRgbArg() const;
    Qt3DRender::QBlendEquationArguments::Blending destinationRgbArg() const;
    Qt3DRender::QBlendEquationArguments::Blending sourceAlphaArg() const;
    Qt3DRender::QBlendEquationArguments::Blending destinationAlphaArg() const;
    Qt3DRender::QBlendEquation::BlendFunction blendFunctionArg() const;

public Q_SLOTS:
    void setAmbient(const QColor &ambient);
    void setDiffuse(const QColor &diffuse);
    void setSpecular(const QColor &specular);
    void setShininess(float shininess);
    void setAlpha(float alpha);
    void setSourceRgbArg(Qt3DRender::QBlendEquationArguments::Blending sourceRgbArg);
    void setDestinationRgbArg(Qt3DRender::QBlendEquationArguments::Blending destinationRgbArg);
    void setSourceAlphaArg(Qt3DRender::QBlendEquationArguments::Blending sourceAlphaArg);
    void setDestinationAlphaArg(Qt3DRender::QBlendEquationArguments::Blending destinationAlphaArg);
    void setBlendFunctionArg(Qt3DRender::QBlendEquation::BlendFunction blendFunctionArg);

Q_SIGNALS:
    void ambientChanged(const QColor &ambient);
    void diffuseChanged(const QColor &diffuse);
    void specularChanged(const QColor &specular);
    void shininessChanged(float shininess);
    void alphaChanged(float alpha);
    void sourceRgbArgChanged(Qt3DRender::QBlendEquationArguments::Blending sourceRgbArg);
    void destinationRgbArgChanged(Qt3DRender::QBlendEquationArguments::Blending destinationRgbArg);
    void sourceAlphaArgChanged(Qt3DRender::QBlendEquationArguments::Blending sourceAlphaArg);
    void destinationAlphaArgChanged(Qt3DRender::QBlendEquationArguments::Blending destinationAlphaArg);
    void blendFunctionArgChanged(Qt3DRender::QBlendEquation::BlendFunction blendFunctionArg);

private:
    Q_DECLARE_PRIVATE(QPhongAlphaMaterial)
};

} // namespace Qt3DExtras

QT_END_NAMESPACE

#endif // QT3DEXTRAS_QPHONGALPHAMATERIAL_H
