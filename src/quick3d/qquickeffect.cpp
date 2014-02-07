/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qquickeffect.h"

#include "qglpainter.h"
#include "qglmaterial.h"

#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>

#include <QtQml/QQmlEngine>
#include <QtQml/QQmlInfo>

#include "qglscenenode.h"

/*!
    \qmltype Effect
    \instantiates QQuickEffect
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

    For more precise control over color and textures pertaining to Effect users should set a Material on the
    material property of Effect.

    With this in mind it should be noted that the color and texture properties of a QML Effect are convenience
    function which may have undesirable results in some circumstances.  This is because color and texture will
    directly affect the underlying QGLMaterial associated with the Effect.

    If no material has been specified when the color and texture are set, then a new default material is
    created and used - any later material specification will then override these values.

    Properties are parsed and set in order, so this should inform usage of these properties.

    For example:

    \code
    Effect {
        id: effect1
        color: "green"
        material: Material {
            diffuseColor: "blue"
        }
    }

    Effect {
        id: effect2
        material: Material {
            diffuseColor: "blue"
        }
        color: "green"
    }
    \endcode

    In the case of effect1 the result will be a blue colored item.  This is because the green Material
    created when the color property is set is overridden when the "blue" material property is later set.

    In the case of effect2, however, the resultant item will be colored green.  In thise case the Material
    specified shall have its color set from "blue" to "green" when the color property is set.

    \note Because color and texture properties directly change the values in the underlying Material (and hence
    could unintentionally change the color of a material which is shared between two Effect it is recommended
    that users only use color and texture properties of an Effect in simple cases - if an underlying Material is
    specified, the user should not use the color and texture properties at all.

    \sa QGLMaterial
*/

QT_BEGIN_NAMESPACE

class QQuickEffectPrivate
{
public:
    QQuickEffectPrivate()
        : color(255, 255, 255, 255),
          useLighting(true),
          decal(false),
          blending(false),
          palette(0),
          materialIndex(-1),
          progress(0.0)
    {
    }

    ~QQuickEffectPrivate()
    {
        palette.clear();
    }

    QColor color;
    bool useLighting;
    bool decal;
    bool blending;
    QSharedPointer<QGLMaterialCollection> palette;
    int materialIndex;
    float progress;

    void cleanupResources();
};

void QQuickEffectPrivate::cleanupResources()
{
    if (!palette)
        return;

    for (int i=0; i<palette->size(); ++i) {
        QGLMaterial* pMat = palette->material(i);
        if (pMat) {
            for (int l=0; l<pMat->textureLayerCount(); ++l) {
                QGLTexture2D* pTex = pMat->texture(l);
                if (pTex) {
                    pTex->cleanupResources();
                }
            }
        }
    }
}

/*!
    \internal
    Constructs the Effect object with \a parent as its parent.
*/
QQuickEffect::QQuickEffect(QObject *parent)
    : QObject(parent)
{
    d = new QQuickEffectPrivate;
}

/*!
    \internal
    Destroy the \l Effect object and delete any unneeded data.
*/
QQuickEffect::~QQuickEffect()
{
    delete d;
}

/*!
    \qmlproperty color Effect::color

    The color of the object for simple non-texture effects.
    The default value for this property is white.
*/

QColor QQuickEffect::color() const
{
    return d->color;
}

void QQuickEffect::setColor(const QColor& value)
{
    d->color = value;
    ensureMaterial();
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
bool QQuickEffect::useLighting() const
{
    return d->useLighting;
}

void QQuickEffect::setUseLighting(bool value)
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
bool QQuickEffect::decal() const
{
    return d->decal;
}

void QQuickEffect::setDecal(bool value)
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
bool QQuickEffect::blending() const
{
    return d->blending;
}

void QQuickEffect::setBlending(bool value)
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
QUrl QQuickEffect::texture() const
{
    if (!material())
        return QUrl();
    return material()->textureUrl();
}

void QQuickEffect::setTexture(const QUrl& value)
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
        ensureMaterial();
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
QImage QQuickEffect::textureImage() const
{
    return (material() && material()->texture()) ?
                material()->texture()->image() : QImage();
}

/*!
  \internal
  Sets this effect to use \value as the image for it's texture.
  
  OpenGL defaults have been overridden to clamp the texture both horizontally and vertically as per QGL::Clamp.
*/
void QQuickEffect::setTextureImage(const QImage& value)
{
    QGLTexture2D * tex;
    ensureMaterial();
    if (!material()->texture())
    {
        // Should this texture be parented?
        tex = new QGLTexture2D(material());
        material()->setTexture(tex);
    } else
    {
        tex = material()->texture();
    }

    // Equality test of images can be very expensive, so always assign the
    // value and emit effect changed
    tex->setImage(value);
    
    // prevents artifacts due to texture smoothing wrapping around edges of texture 
    tex->setHorizontalWrap(QGL::ClampToEdge);
    tex->setVerticalWrap(QGL::ClampToEdge);
    
    emit effectChanged();
}

/*!
    \qmlproperty Material Effect::material

    Defines the material to apply to items that use this effect.
*/
QGLMaterial *QQuickEffect::material() const
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
void QQuickEffect::setMaterial(QGLMaterial *value)
{
    ensureMaterial();
    int newIndex = -1;
    if (value)
        newIndex = d->palette->addMaterial(value);
    if (newIndex != d->materialIndex)
    {
        d->materialIndex = newIndex;
        QGLMaterial *current = d->palette->material(d->materialIndex);
        if (current) {
            disconnect(current, SIGNAL(materialChanged()), this, SIGNAL(effectChanged()));

        }
        if (value) {
            connect(value, SIGNAL(materialChanged()), this, SIGNAL(effectChanged()));
        }

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
void QQuickEffect::enableEffect(QGLPainter *painter)
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
void QQuickEffect::disableEffect(QGLPainter *painter)
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
void QQuickEffect::applyTo(QGLSceneNode *node)
{
    ensureMaterial();

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

QGLTexture2D *QQuickEffect::texture2D()
{
    if (!material())
        return 0;
    return material()->texture();
}

/*!
  Returns the progress of remote resource loading.
  */
float QQuickEffect::progress()
{
    return d->progress;
}

void QQuickEffect::openglContextIsAboutToBeDestroyed()
{
    d->cleanupResources();
}

void QQuickEffect::ensureMaterial() {
    if (!d->palette)
    {
        d->palette = QSharedPointer<QGLMaterialCollection>(new QGLMaterialCollection());
    }
    if (d->materialIndex == -1)
    {
        QGLMaterial * newMaterial = new QGLMaterial(d->palette.data());
        newMaterial->setColor(d->color);
        connect(newMaterial, SIGNAL(materialChanged()), this, SIGNAL(effectChanged()));
        d->materialIndex = d->palette->addMaterial(newMaterial);
    }
}

QT_END_NAMESPACE
