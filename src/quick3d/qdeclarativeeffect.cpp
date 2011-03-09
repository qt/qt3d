/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtQuick3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qdeclarativeeffect.h"

#include <Qt3D/qglpainter.h>
#include <Qt3D/qglmaterial.h>

#include <QNetworkRequest>
#include <QNetworkReply>

#include <QtDeclarative/qdeclarativeengine.h>

#include "private/qdeclarativepixmapcache_p.h"
#include <qdeclarativeinfo.h>

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
          textureChanged(false),
          decal(false),
          blending(false),
          texture2D(0),
          material(0),
          declarativePixmap(),
          progress(0.0)
    {
    }

    ~QDeclarativeEffectPrivate()
    {
        delete texture2D;
    }

    QColor color;
    bool useLighting;
    bool textureChanged;
    bool decal;
    bool blending;
    QGLTexture2D *texture2D;
    QUrl textureUrl;
    QGLMaterial *material;

    QDeclarativePixmap declarativePixmap;
    qreal progress;
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

    \sa textureImage
*/
QUrl QDeclarativeEffect::texture() const
{
    return d->textureUrl;
}

void QDeclarativeEffect::setTexture(const QUrl& value)
{
    if (d->textureUrl == value)
        return;

    d->textureUrl = value;
    // got a new value, so abort any in-progress request
    d->declarativePixmap.clear(this);

    if (d->progress != 0.0)
    {
        d->progress = 0.0;
        emit progressChanged(d->progress);
    }

    if (d->textureUrl.isEmpty())
    {
        d->textureChanged = true;
    }
    else
    {
        // Start loading the new texture
        d->textureUrl = value;
        emit effectChanged();

        bool async(true);
#ifndef QT_NO_LOCALFILE_OPTIMIZED_QML
        async = d->textureUrl.scheme() != QLatin1String("file");
#endif
#if QT_VERSION >= 0x040702
        QDeclarativePixmap::Options options = QDeclarativePixmap::Cache;
        if (async)
            options |= QDeclarativePixmap::Asynchronous;
        d->declarativePixmap.load(qmlEngine(this), d->textureUrl, options);
#else
        d->declarativePixmap.load(qmlEngine(this), d->textureUrl, async );
#endif
        if (d->declarativePixmap.isLoading())
        {
            d->declarativePixmap.connectFinished(this, SLOT(textureRequestFinished()));
            d->declarativePixmap.connectDownloadProgress(this, SLOT(textureRequestProgress(qint64,qint64)));
        }
        else
        {
            if (d->declarativePixmap.isError())
                qWarning() << "Error loading pixmap: " <<                     d->declarativePixmap.error();
            else if (d->declarativePixmap.isReady())
            {
                textureRequestFinished();
            }
        }
    }
}

/*!
    This function is used to emit the appropriate signal when the texture request has finished.

    \sa effectChanged()
*/
void QDeclarativeEffect::textureRequestFinished()
{
    QDeclarativePixmap::Status status = d->declarativePixmap.status();
    d->textureChanged = true;
    if (status == QDeclarativePixmap::Ready)
    {
        setTextureImage(d->declarativePixmap.pixmap().toImage());
        if (d->declarativePixmap.pixmap().isNull() ) {
            qWarning() << "Could not load specified texture file";
        }
        d->progress = 1.0;
        emit progressChanged(d->progress);
        emit effectChanged();
    } else if (status != QDeclarativePixmap::Loading)
    {
        qWarning() << "Error getting texture image from cache: "
                << d->declarativePixmap.error();
    }
}

/*!
    Updates the progress of an asynchronous resource request.  Progress is
    simply \a received / \a total.
*/
void QDeclarativeEffect::textureRequestProgress(qint64 received, qint64 total)
{
    if (d->declarativePixmap.status() == QDeclarativePixmap::Loading && total > 0) {
        d->progress = qreal(received)/total;
        emit progressChanged(d->progress);
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
    // Expensive
    return d->declarativePixmap.pixmap().toImage();
}

/*!
  \internal
  Sets this effect to use \value as the image for it's texture.
*/
void QDeclarativeEffect::setTextureImage(const QImage& value)
{
    d->declarativePixmap.setPixmap(QPixmap::fromImage(value));
    d->textureChanged = true;
    emit effectChanged();
}

/*!
    \qmlproperty Material Effect::material

    Defines the material to apply to items that use this effect.
*/
QGLMaterial *QDeclarativeEffect::material() const
{
    return d->material;
}

/*!
  \internal
  Sets the material for use with this effect.
*/
void QDeclarativeEffect::setMaterial(QGLMaterial *value)
{
    if (d->material != value) {
        if (d->material) {
            disconnect(d->material, SIGNAL(materialChanged()),
                       this, SIGNAL(effectChanged()));
        }
        d->material = value;
        if (d->material) {
            connect(d->material, SIGNAL(materialChanged()),
                    this, SIGNAL(effectChanged()));
        }
        emit effectChanged();
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
    QGLTexture2D *tex = texture2D();
    if (tex == NULL && d->material)
        tex = d->material->texture();
    if (d->useLighting) {
        if (tex && !tex->isNull()) {
            if (d->decal)
                painter->setStandardEffect(QGL::LitDecalTexture2D);
            else
                painter->setStandardEffect(QGL::LitModulateTexture2D);
            painter->glActiveTexture(GL_TEXTURE0);
            tex->bind();
        } else {
            painter->setStandardEffect(QGL::LitMaterial);
        }
        if (d->material)
            painter->setFaceMaterial(QGL::AllFaces, d->material);
        else
            painter->setFaceColor(QGL::AllFaces, d->color);
    } else {
        if (tex && !tex->isNull()) {
            if (d->decal)
                painter->setStandardEffect(QGL::FlatDecalTexture2D);
            else
                painter->setStandardEffect(QGL::FlatReplaceTexture2D);
            painter->glActiveTexture(GL_TEXTURE0);
            tex->bind();
            painter->setColor(d->color);
        } else {
            painter->setStandardEffect(QGL::FlatColor);
            painter->setColor(d->color);
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
  Returns the progress of remote resource loading.
  */
qreal QDeclarativeEffect::progress()
{
    return d->progress;
}

/*!
    \internal
    This function returns a \l QGLTexture2D based on the \c texture property of the \l Effect.
*/
QGLTexture2D *QDeclarativeEffect::texture2D()
{
    if (d->textureChanged) {
        delete d->texture2D;
        QGLTexture2D *newtex = new QGLTexture2D();
        if (!d->declarativePixmap.pixmap().isNull())
            newtex->setPixmap(d->declarativePixmap.pixmap());
        d->texture2D = newtex;
        d->textureChanged = false;
    }
    return d->texture2D;
}

QT_END_NAMESPACE
