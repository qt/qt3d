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

#include "qdeclarativeeffect.h"

#include "qglpainter.h"
#include "qglmaterial.h"

#include <QNetworkRequest>
#include <QNetworkReply>

#include <QtDeclarative/qdeclarativeengine.h>

#include <qdeclarativeinfo.h>
#include "qglscenenode.h"

/*!
    \qmlclass Effect QDeclarativeEffect
    \brief The Effect item defines simple effects within the QML/3D environment.  Examples
    of such effects include textures, simple material and lighting effects, and so on.
    \since 4.8
    \ingroup qt3d::qml3d

    \section1 Defining an Effect

    Effects can be defined within QML using the normal syntax for creating any QML/3d object.  To create a
    default effect with no extra information, for example, we can simply using the following:

    \code
    Effect {}
    \endcode

    More complex effects use the usual QML syntax for accessing and updating properties.  In order to specify
    a texture, for example, the following could be used:

    \code
    Effect {
        id: myTextureEffect
        texture: "texture.png"
    }
    \endcode
*/

QT_BEGIN_NAMESPACE

class QDeclarativeEffectPrivate
{
public:
    QDeclarativeEffectPrivate()
        : color(255, 255, 255, 255),
          useLighting(true),
          decal(false),
          blending(false),
          palette(0),
          materialIndex(-1),
          progress(0.0)
    {
    }

    ~QDeclarativeEffectPrivate()
    {
        delete palette;
    }

    QColor color;
    bool useLighting;
    bool decal;
    bool blending;
    QGLMaterialCollection *palette;
    int materialIndex;
    qreal progress;
    inline void ensureMaterial() {
        if (!palette)
        {
            palette = new QGLMaterialCollection();
        }
        if (materialIndex == -1)
        {
            materialIndex = palette->addMaterial(new QGLMaterial(palette));
            palette->material(materialIndex)->setColor(color);
        }
        // TODO: decal & blending
    }
};

/*!
    \internal
    Constructs the Effect object with \a parent as its parent.
*/
QDeclarativeEffect::QDeclarativeEffect(QObject *parent)
    : QObject(parent)
{
    d = new QDeclarativeEffectPrivate;
}

/*!
    \internal
    Destroy the \l Effect object and delete any unneeded data.
*/
QDeclarativeEffect::~QDeclarativeEffect()
{
    delete d;
}

/*!
    \qmlproperty color Effect::color

    The color of the object for simple non-texture effects.
    The default value for this property is white.
*/

QColor QDeclarativeEffect::color() const
{
    return d->color;
}

void QDeclarativeEffect::setColor(const QColor& value)
{
    d->color = value;
    d->ensureMaterial();
    material()->setColor(value);
    emit effectChanged();
}

/*!
    \qmlproperty bool Effect::useLighting

    The lighting control parameter; true if this effect should
    use lighting or false if this effect should use flat colors and
    textures.

    The default value for this property is true.
*/
bool QDeclarativeEffect::useLighting() const
{
    return d->useLighting;
}

void QDeclarativeEffect::setUseLighting(bool value)
{
    d->useLighting = value;
    emit effectChanged();
}

/*!
    \qmlproperty bool Effect::decal

    This property should be set to true if the \l texture should be
    combined with \l color to decal the texture onto the object.
    This property should be set to false to use the texture
    directly, combined with the \l material parameters.

    The default value for this property is false.
*/
bool QDeclarativeEffect::decal() const
{
    return d->decal;
}

void QDeclarativeEffect::setDecal(bool value)
{
    if (d->decal != value) {
        d->decal = value;
        emit effectChanged();
    }
}

/*!
    \qmlproperty bool Effect::blending

    This property should be set to true if alpha blending should be
    enabled when this effect is active, or false otherwise.  The
    default is false.

    This property overrides the viewport-specific blending setting
    that is specified by Viewport::blending.
*/
bool QDeclarativeEffect::blending() const
{
    return d->blending;
}

void QDeclarativeEffect::setBlending(bool value)
{
    if (d->blending != value) {
        d->blending = value;
        emit effectChanged();
    }
}

/*!
    \qmlproperty url Effect::texture

    Texture effects are defined by this property.  A texture is
    provided by means of a QUrl which is then used for texturing.

    Textures can also be defined directly as images using the textureImage
    property.

    If the value is non-empty, and the texture could not be loaded from
    the QUrl for any reason, then the property will not be changed.

    \sa textureImage
*/
QUrl QDeclarativeEffect::texture() const
{
    if (!material())
        return QUrl();
    return material()->textureUrl();
}

void QDeclarativeEffect::setTexture(const QUrl& value)
{
    if (material() && material()->textureUrl() == value)
        return;

    if (d->progress != 0.0)
    {
        d->progress = 0.0;
        emit progressChanged(d->progress);
    }

    if (value.isEmpty())
    {
        if (material())
        {
            material()->setTextureUrl(value);
            emit effectChanged();
        }
    }
    else
    {
        d->ensureMaterial();
        // Warning: This will trigger the deletion of the old texure.
        material()->setTextureUrl(value);
        emit effectChanged();
    }
}

/*!
    \qmlproperty image Effect::textureImage

    This property directly defines a texture using an existing QImage.

    Textures can also be defined based on a URL using the texture property.

    \sa texture
*/
QImage QDeclarativeEffect::textureImage() const
{
    return (material() && material()->texture()) ?
                material()->texture()->image() : QImage();
}

/*!
  \internal
  Sets this effect to use \value as the image for it's texture.
  
  OpenGL defaults have been overridden to clamp the texture both horizontally and vertically as per QGL::Clamp.
*/
void QDeclarativeEffect::setTextureImage(const QImage& value)
{
    QGLTexture2D * tex;
    d->ensureMaterial();
    if (!material()->texture())
    {
        // Should this texture be parented?
        tex = new QGLTexture2D();
        material()->setTexture(tex);
    } else
    {
        tex = material()->texture();
    }

    // Equality test of images can be very expensive, so always assign the
    // value and emit effect changed
    tex->setImage(value);
    
    // prevents artifacts due to texture smoothing wrapping around edges of texture 
    tex->setHorizontalWrap(QGL::Clamp);
    tex->setVerticalWrap(QGL::Clamp);
    
    emit effectChanged();
}

/*!
    \qmlproperty Material Effect::material

    Defines the material to apply to items that use this effect.
*/
QGLMaterial *QDeclarativeEffect::material() const
{
    if (!d->palette)
        return 0;
    return d->palette->material(d->materialIndex);
}

/*!
  \internal
  Sets the material for use with this effect.  Creates a QGLMaterialCollection
  to contain it if necessary.
*/
void QDeclarativeEffect::setMaterial(QGLMaterial *value)
{
    d->ensureMaterial();

    int newIndex = d->palette->addMaterial(value);
    if (newIndex != d->materialIndex)
    {
        d->materialIndex = newIndex;
        emit effectChanged();
        // TODO: deleting old materials
    }
}

/*!
    \qmlproperty real Effect::progress

    Tracks how much of a remote resource has been downloaded, where 0.0
    is no progress, and 1.0 is completion.
*/

/*!
    \internal
    Enable the effect on for a given \a painter.
*/
void QDeclarativeEffect::enableEffect(QGLPainter *painter)
{
    painter->setColor(d->color);
    if (d->materialIndex != -1 && d->palette->material(d->materialIndex))
    {
        painter->setFaceMaterial(QGL::FrontFaces, material()->front());
        painter->setFaceMaterial(QGL::BackFaces, material()->back());
    } else
        painter->setFaceColor(QGL::AllFaces, d->color);

    QGLTexture2D *tex = 0;
    if (material())
        tex = material()->texture();
    if (d->useLighting) {
        if (tex && !tex->isNull()) {
            if (d->decal)
                painter->setStandardEffect(QGL::LitDecalTexture2D);
            else
                painter->setStandardEffect(QGL::LitModulateTexture2D);
            tex->bind();
        } else {
            painter->setStandardEffect(QGL::LitMaterial);
        }
    } else {
        if (tex && !tex->isNull()) {
            if (d->decal)
                painter->setStandardEffect(QGL::FlatDecalTexture2D);
            else
                painter->setStandardEffect(QGL::FlatReplaceTexture2D);
//            painter->glActiveTexture(GL_TEXTURE0);
            tex->bind();
        } else {
            painter->setStandardEffect(QGL::FlatColor);
        }
    }
}

/*!
    \internal
    Disable the effect for a given \a painter.
*/
void QDeclarativeEffect::disableEffect(QGLPainter *painter)
{
    painter->setStandardEffect(QGL::FlatColor);
    painter->setColor(Qt::white);
    painter->glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

/*!
  \internal
  Set a scenenode's material and effect properties to enact this effect.

  The desired functionality should mirror the Mesh::draw(QGLPainter*)
  functionality as closely as possible.

  Subclasses can reimplement this function to set desired effects (e.g. user
  effects);
*/
void QDeclarativeEffect::applyTo(QGLSceneNode *node)
{
    d->ensureMaterial();

    node->setPalette(d->palette);
    node->setMaterialIndex(d->materialIndex);

    Q_ASSERT(node->material());

    QGLTexture2D *tex = material()->texture();
    if (tex && !tex->isNull())
        node->material()->setTexture(tex);

    // Determine which standard effect to use
    if (d->useLighting) {
        if (tex && !tex->isNull()) {
            if (d->decal)
                node->setEffect(QGL::LitDecalTexture2D);
            else
                node->setEffect(QGL::LitModulateTexture2D);
        } else {
            node->setEffect(QGL::LitMaterial);
        }
    } else {
        if (tex && !tex->isNull()) {
            if (d->decal)
                node->setEffect(QGL::FlatDecalTexture2D);
            else
                node->setEffect(QGL::FlatReplaceTexture2D);
        } else {
            node->setEffect(QGL::FlatColor);
        }
    }
}

QGLTexture2D *QDeclarativeEffect::texture2D()
{
    if (!material())
        return 0;
    return material()->texture();
}

/*!
  Returns the progress of remote resource loading.
  */
qreal QDeclarativeEffect::progress()
{
    return d->progress;
}

QT_END_NAMESPACE
