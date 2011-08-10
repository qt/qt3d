/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtQuick3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qglmaterial.h"
#include "qglmaterial_p.h"
#include "qglpainter.h"
#include "qgltexture2d.h"
#include "qglmaterialcollection.h"
#include "qgllightmodel.h"
#include "qfileinfo.h"

#include <QtCore/qurl.h>

QT_BEGIN_NAMESPACE

/*!
    \class QGLMaterial
    \brief The QGLMaterial class describes one-sided material properties for rendering fragments.
    \since 4.8
    \ingroup qt3d
    \ingroup qt3d::materials

    \sa QGLTwoSidedMaterial
*/

/*!
    \qmlclass Material QGLMaterial
    \brief The Material item describes material properties for OpenGL drawing.
    \since 4.8
    \ingroup qt3d::qml3d
*/

QGLMaterialPrivate::QGLMaterialPrivate()
    : specularColor(0, 0, 0, 255),
      emittedLight(0, 0, 0, 255),
      shininess(0),
      collection(0),
      index(-1),
      used(false)
{
    ambientColor.setRgbF(0.2f, 0.2f, 0.2f, 1.0f);
    diffuseColor.setRgbF(0.8f, 0.8f, 0.8f, 1.0f);
}

/*!
    Constructs a QGLMaterial object with its default values,
    and attaches it to \a parent.
*/
QGLMaterial::QGLMaterial(QObject *parent)
    : QGLAbstractMaterial(parent)
    , d_ptr(new QGLMaterialPrivate)
{
}

/*!
    Destroys this QGLMaterial object.
*/
QGLMaterial::~QGLMaterial()
{
}

/*!
    \property QGLMaterial::ambientColor
    \brief the ambient color of the material.  The default value
    is (0.2, 0.2, 0.2, 1.0).

    \sa diffuseColor(), specularColor(), ambientColorChanged()
*/

/*!
    \qmlproperty color Material::ambientColor
    The ambient color of the material.  The default value
    is (0.2, 0.2, 0.2, 1.0).

    \sa diffuseColor, specularColor
*/

QColor QGLMaterial::ambientColor() const
{
    Q_D(const QGLMaterial);
    return d->ambientColor;
}

void QGLMaterial::setAmbientColor(const QColor& value)
{
    Q_D(QGLMaterial);
    if (d->ambientColor != value) {
        d->ambientColor = value;
        emit ambientColorChanged();
        emit materialChanged();
    }
}

/*!
    \property QGLMaterial::diffuseColor
    \brief the diffuse color of the material.  The default value
    is (0.8, 0.8, 0.8, 1.0).

    \sa ambientColor(), specularColor(), diffuseColorChanged()
*/

/*!
    \qmlproperty color Material::diffuseColor
    The diffuse color of the material.  The default value
    is (0.8, 0.8, 0.8, 1.0).

    \sa ambientColor, specularColor
*/

QColor QGLMaterial::diffuseColor() const
{
    Q_D(const QGLMaterial);
    return d->diffuseColor;
}

void QGLMaterial::setDiffuseColor(const QColor& value)
{
    Q_D(QGLMaterial);
    if (d->diffuseColor != value) {
        d->diffuseColor = value;
        emit diffuseColorChanged();
        emit materialChanged();
    }
}

/*!
    \property QGLMaterial::specularColor
    \brief the specular color of the material.  The default value
    is (0, 0, 0, 1).

    \sa ambientColor(), diffuseColor(), specularColorChanged()
*/

/*!
    \qmlproperty color Material::specularColor
    The specular color of the material.  The default value
    is (0, 0, 0, 1).

    \sa ambientColor, diffuseColor
*/

QColor QGLMaterial::specularColor() const
{
    Q_D(const QGLMaterial);
    return d->specularColor;
}

void QGLMaterial::setSpecularColor(const QColor& value)
{
    Q_D(QGLMaterial);
    if (d->specularColor != value) {
        d->specularColor = value;
        emit specularColorChanged();
        emit materialChanged();
    }
}

/*!
    \property QGLMaterial::emittedLight
    \brief the emitted light intensity of the material.
    The default value is (0.0, 0.0, 0.0, 1.0), which indicates
    that the material does not emit any light.

    \sa emittedLightChanged()
*/

/*!
    \qmlproperty color Material::emittedLight
    The emitted light intensity of the material.
    The default value is (0.0, 0.0, 0.0, 1.0), which indicates
    that the material does not emit any light.
*/

QColor QGLMaterial::emittedLight() const
{
    Q_D(const QGLMaterial);
    return d->emittedLight;
}

void QGLMaterial::setEmittedLight(const QColor& value)
{
    Q_D(QGLMaterial);
    if (d->emittedLight != value) {
        d->emittedLight = value;
        emit emittedLightChanged();
        emit materialChanged();
    }
}

/*!
    Sets ambientColor() to 20% of \a value, and diffuseColor() to 80% of
    \a value.  This is a convenience function for quickly setting ambient
    and diffuse lighting colors based on a flat color.

    \sa ambientColor(), diffuseColor()
*/
void QGLMaterial::setColor(const QColor& value)
{
    Q_D(QGLMaterial);
    d->ambientColor.setRgbF
        (value.redF() * 0.2f, value.greenF() * 0.2f,
         value.blueF() * 0.2f, value.alphaF());
    d->diffuseColor.setRgbF
        (value.redF() * 0.8f, value.greenF() * 0.8f,
         value.blueF() * 0.8f, value.alphaF());
    emit ambientColorChanged();
    emit diffuseColorChanged();
    emit materialChanged();
}

/*!
    \property QGLMaterial::shininess
    \brief the specular exponent of the material, or how shiny it is.
    Must be between 0 and 128.  The default value is 0.  A value outside
    this range will be clamped to the range when the property is set.

    \sa shininessChanged()
*/

/*!
    \qmlproperty real Material::shininess
    The specular exponent of the material, or how shiny it is.
    Must be between 0 and 128.  The default value is 0.  A value outside
    this range will be clamped to the range when the property is set.
*/

qreal QGLMaterial::shininess() const
{
    Q_D(const QGLMaterial);
    return d->shininess;
}

void QGLMaterial::setShininess(qreal value)
{
    Q_D(QGLMaterial);
    if (value < 0)
        value = 0;
    else if (value > 128)
        value = 128;
    if (d->shininess != value) {
        d->shininess = value;
        emit shininessChanged();
        emit materialChanged();
    }
}

/*!
    \property QGLMaterial::texture
    \brief the 2D texture associated with \a layer on this material;
    null if no texture.

    Layer 0 is normally the primary texture associated with the material.
    Multiple texture layers may be specified for materials with special
    blending effects or to specify ambient, diffuse, or specular colors
    pixel-by-pixel.

    \sa texturesChanged()
*/
QGLTexture2D *QGLMaterial::texture(int layer) const
{
    Q_D(const QGLMaterial);
    return d->textures.value(layer, 0);
}

void QGLMaterial::setTexture(QGLTexture2D *value, int layer)
{
    Q_ASSERT(layer >= 0);
    Q_D(QGLMaterial);
    QGLTexture2D *prev = d->textures.value(layer, 0);
    if (prev != value) {
        delete prev;
        d->textures[layer] = value;
        emit texturesChanged();
        emit materialChanged();
    }
}

/*!
    \property QGLMaterial::textureUrl
    \brief URL of the 2D texture associated with \a layer on this material.

    By default \a layer is 0, the primary texture.

    If the URL has not been specified, then this property is a null QUrl.

    Setting this property to a non-empty URL will replace any existing texture
    with a new texture based on the image at the given \a url.  If that
    image is not a valid texture then the new texture will be a null texture.

    If an empty url is set, this has the same effect as \c{setTexture(0)}.

    \sa texture(), setTexture()
*/
QUrl QGLMaterial::textureUrl(int layer) const
{
    Q_D(const QGLMaterial);
    QGLTexture2D *tex = d->textures.value(layer, 0);
    if (tex)
        return tex->url();
    else
        return QUrl();
}

void QGLMaterial::setTextureUrl(const QUrl &url, int layer)
{
    Q_ASSERT(layer >= 0);
    if (textureUrl(layer) != url)
    {
        QGLTexture2D *tex = 0;
        if (!url.isEmpty())
        {
            tex = new QGLTexture2D(this);
            connect(tex, SIGNAL(textureUpdated()), this, SIGNAL(texturesChanged()));
            tex->setUrl(url);
        }
        setTexture(tex, layer);
    }
}

/*!
    \enum QGLMaterial::TextureCombineMode
    This enum defines the mode to use when combining a texture with
    the material colors on a QGLMaterial object.

    \value Modulate Modulate the texture with the lighting
           conditions to produce a lit texture.
    \value Decal Combine the texture with the lighting conditions
           to produce a decal effect.
    \value Replace Replace with the contents of the texture,
           ignoring colors and lighting conditions.
*/

/*!
    \property QGLMaterial::textureCombineMode
    \brief the texture combine mode associated with \a layer on this material.
    The default value is \l Modulate.

    \sa texturesChanged()
*/

QGLMaterial::TextureCombineMode QGLMaterial::textureCombineMode(int layer) const
{
    Q_D(const QGLMaterial);
    return d->textureModes.value(layer, Modulate);
}

void QGLMaterial::setTextureCombineMode(QGLMaterial::TextureCombineMode mode, int layer)
{
    Q_D(QGLMaterial);
    if (d->textureModes.value(layer, Modulate) != mode) {
        d->textureModes[layer] = mode;
        emit texturesChanged();
        emit materialChanged();
    }
}

/*!
    Returns the number of texture layers associated with this material.

    The return value may be larger than the number of actual texture
    layers if some of the intermediate layers are null.  For example,
    setting layers 0 and 2 will report textureLayerCount() as 3.
    The main use of this value is to iterate over all layers.

    \sa texture()
*/
int QGLMaterial::textureLayerCount() const
{
    Q_D(const QGLMaterial);
    int maxLayer = -1;
    if (!d->textures.isEmpty())
        maxLayer = qMax(maxLayer, (d->textures.end() - 1).key());
    return maxLayer + 1;
}

/*!
    \reimp
    Returns this material.
*/
QGLMaterial *QGLMaterial::front() const
{
    return const_cast<QGLMaterial *>(this);
}

/*!
    \reimp
*/
bool QGLMaterial::isTransparent() const
{
    Q_D(const QGLMaterial);
    bool transparent = (d->diffuseColor.alpha() != 255);
    QMap<int, QGLTexture2D *>::ConstIterator it;
    for (it = d->textures.begin(); it != d->textures.end(); ++it) {
        TextureCombineMode mode = d->textureModes.value(it.key(), Modulate);
        if (mode == Modulate) {
            // Texture alpha adds to the current alpha.
            if (it.value() && it.value()->hasAlphaChannel())
                transparent = true;
        } else if (mode == Replace) {
            // Replace the current alpha with the texture's alpha.
            if (it.value())
                transparent = it.value()->hasAlphaChannel();
        }
    }
    return transparent;
}

/*!
    \reimp
*/
void QGLMaterial::bind(QGLPainter *painter)
{
    painter->setFaceMaterial(QGL::AllFaces, this);
    const_cast<QGLLightModel *>(painter->lightModel())
        ->setModel(QGLLightModel::OneSided); // FIXME
    bindTextures(painter);
}

/*!
    \internal
*/
void QGLMaterial::bindTextures(QGLPainter *painter)
{
    Q_D(const QGLMaterial);
    QMap<int, QGLTexture2D *>::ConstIterator it;
    for (it = d->textures.begin(); it != d->textures.end(); ++it) {
        QGLTexture2D *tex = it.value();
        painter->glActiveTexture(GL_TEXTURE0 + it.key());
        if (tex)
            tex->bind();
        else
            glBindTexture(GL_TEXTURE_2D, 0);
    }
}

/*!
    \reimp
*/
void QGLMaterial::release(QGLPainter *painter, QGLAbstractMaterial *next)
{
    Q_UNUSED(next);
    Q_D(const QGLMaterial);
    QMap<int, QGLTexture2D *>::ConstIterator it;
    for (it = d->textures.begin(); it != d->textures.end(); ++it) {
        painter->glActiveTexture(GL_TEXTURE0 + it.key());
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}

/*!
    \reimp
*/
void QGLMaterial::prepareToDraw
    (QGLPainter *painter, const QGLAttributeSet &attributes)
{
    bindEffect(painter, attributes, false);
}

/*!
    \internal
*/
void QGLMaterial::bindEffect
    (QGLPainter *painter, const QGLAttributeSet &attributes, bool twoSided)
{
    Q_D(const QGLMaterial);
    Q_UNUSED(twoSided);
    QGL::StandardEffect effect = QGL::LitMaterial;
    if (!d->textures.isEmpty() && attributes.contains(QGL::TextureCoord0)) {
        // TODO: different combine modes for each layer.
        QGLMaterial::TextureCombineMode mode =
            d->textureModes.value(0, Modulate);
        if (mode == Replace)
            effect = QGL::FlatReplaceTexture2D;
        else if (mode == Decal)
            effect = QGL::LitDecalTexture2D;
        else
            effect = QGL::LitModulateTexture2D;
    }
    painter->setStandardEffect(effect);
}

/*!
    \fn void QGLMaterial::ambientColorChanged()

    This signal is emitted when ambientColor() changes.

    \sa ambientColor(), setAmbientColor(), materialChanged()
*/

/*!
    \fn void QGLMaterial::diffuseColorChanged()

    This signal is emitted when diffuseColor() changes.

    \sa diffuseColor(), setDiffuseColor(), materialChanged()
*/

/*!
    \fn void QGLMaterial::specularColorChanged()

    This signal is emitted when specularColor() changes.

    \sa specularColor(), setSpecularColor(), materialChanged()
*/

/*!
    \fn void QGLMaterial::emittedLightChanged()

    This signal is emitted when emittedLight() changes.

    \sa emittedLight(), setEmittedLight(), materialChanged()
*/

/*!
    \fn void QGLMaterial::shininessChanged()

    This signal is emitted when shininess() changes.

    \sa shininess(), setShininess(), materialChanged()
*/

/*!
    \fn void QGLMaterial::texturesChanged()

    This signal is emitted when the texture layers associated with
    this material change.

    \sa texture(), setTexture(), materialChanged()
*/

#ifndef QT_NO_DEBUG_STREAM

QDebug operator<<(QDebug dbg, const QGLMaterial &material)
{
    dbg << &material <<
            "-- Amb:" << material.ambientColor() <<
            "-- Diff:" << material.diffuseColor() <<
            "-- Spec:" << material.specularColor() <<
            "-- Shin:" << material.shininess();
    for (int i = 0; i < material.textureLayerCount(); ++i)
        if (material.texture(i) != 0)
            dbg << "\n    -- Tex" << i << ":" << material.texture(i)
            << material.texture(i)->objectName();
    dbg << "\n";
    return dbg;
}
#endif

QT_END_NAMESPACE
