/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qabstracttexture.h"
#include "qabstracttexture_p.h"
#include <Qt3DCore/qscenepropertychange.h>
#include <Qt3DRender/qabstracttextureimage.h>

QT_BEGIN_NAMESPACE

using namespace Qt3DCore;

namespace Qt3DRender {

QAbstractTexturePrivate::QAbstractTexturePrivate()
    : QNodePrivate()
    , m_target(QAbstractTexture::Target2D)
    , m_format(QAbstractTexture::Automatic)
    , m_width(1)
    , m_height(1)
    , m_depth(1)
    , m_autoMipMap(false)
    , m_minFilter(QAbstractTexture::Nearest)
    , m_magFilter(QAbstractTexture::Nearest)
    , m_status(QAbstractTexture::Loading)
    , m_maximumAnisotropy(1.0f)
    , m_comparisonFunction(QAbstractTexture::CompareLessEqual)
    , m_comparisonMode(QAbstractTexture::CompareNone)
    , m_maximumLayers(1)
    , m_unique(false)
{
}

void QAbstractTexture::copy(const QNode *ref)
{
    QNode::copy(ref);
    const QAbstractTexture *t = static_cast<const QAbstractTexture*>(ref);
    d_func()->m_target = t->d_func()->m_target;
    d_func()->m_width = t->d_func()->m_width;
    d_func()->m_height = t->d_func()->m_height;
    d_func()->m_depth = t->d_func()->m_depth;
    d_func()->m_format = t->d_func()->m_format;
    d_func()->m_wrapMode.setX(t->d_func()->m_wrapMode.x());
    d_func()->m_wrapMode.setY(t->d_func()->m_wrapMode.y());
    d_func()->m_wrapMode.setZ(t->d_func()->m_wrapMode.z());
    d_func()->m_minFilter = t->d_func()->m_minFilter;
    d_func()->m_magFilter = t->d_func()->m_magFilter;
    d_func()->m_autoMipMap = t->d_func()->m_autoMipMap;
    d_func()->m_maximumAnisotropy = t->d_func()->m_maximumAnisotropy;
    d_func()->m_comparisonFunction = t->d_func()->m_comparisonFunction;
    d_func()->m_comparisonMode = t->d_func()->m_comparisonMode;
    d_func()->m_maximumLayers = t->d_func()->m_maximumLayers;
}

/*!
    \class Qt3DRender::QAbstractTexture
    \inmodule Qt3DRender
    \since 5.5
    \brief A base class to be used to provide textures.

    The QAbstractTexture class shouldn't be used directly but rather
    through one of its subclasses. Each subclass implements a given texture
    target (2D, 2DArray, 3D, CubeMap ...) Each subclass provides a set of
    functors for each layer, cube map face and mipmap level. In turn the
    backend uses those functor to properly fill a corresponding OpenGL texture
    with data.
 */

QAbstractTexture::QAbstractTexture(QNode *parent)
    : QNode(*new QAbstractTexturePrivate, parent)
{
}

QAbstractTexture::QAbstractTexture(Target target, QNode *parent)
    : QNode(*new QAbstractTexturePrivate, parent)
{
    d_func()->m_target = target;
}

/*! \internal */
QAbstractTexture::QAbstractTexture(QAbstractTexturePrivate &dd, QNode *parent)
    : QNode(dd, parent)
{
}

QAbstractTexture::~QAbstractTexture()
{
    // The subclasses of QAbstractTexture are only there to set the type on a QTextureProvider
    // QNode::cleanup needs to be handled here and not in the subclasses.
    QNode::cleanup();
}

/*!
    Sets the size of the texture provider to width \a w, height \a h and depth \a d.
 */
void QAbstractTexture::setSize(int w, int h, int d)
{
    setWidth(w);
    setHeight(h);
    setDepth(d);
}

/*!
    \property Qt3DRender::QAbstractTexture::width

    Holds the width of the texture provider.
 */
void QAbstractTexture::setWidth(int width)
{
    Q_D(QAbstractTexture);
    if (d->m_width != width) {
        d->m_width = width;
        emit widthChanged(width);
    }
}

/*!
    \property Qt3DRender::QAbstractTexture::height

    Holds the height of the texture provider.
 */
void QAbstractTexture::setHeight(int height)
{
    Q_D(QAbstractTexture);
    if (d->m_height != height) {
        d->m_height = height;
        emit heightChanged(height);
    }
}

/*!
    \property Qt3DRender::QAbstractTexture::depth

    Holds the depth of the texture provider.
 */
void QAbstractTexture::setDepth(int depth)
{
    Q_D(QAbstractTexture);
    if (d->m_depth != depth) {
        d->m_depth = depth;
        emit depthChanged(depth);
    }
}

int QAbstractTexture::width() const
{
    Q_D(const QAbstractTexture);
    return d->m_width;
}

int QAbstractTexture::height() const
{
    Q_D(const QAbstractTexture);
    return d->m_height;
}

int QAbstractTexture::depth() const
{
    Q_D(const QAbstractTexture);
    return d->m_depth;
}

/*!
    \property Qt3DRender::QAbstractTexture::maximumLayers

    Holds the maximum layer count of the texture provider. By default, the
    maximum layer count is 1.

    \note this has a meaning only for texture providers that have 3D or
    array target formats.
 */
void QAbstractTexture::setMaximumLayers(int maximumLayers)
{
    Q_D(QAbstractTexture);
    if (d->m_maximumLayers != maximumLayers) {
        d->m_maximumLayers = maximumLayers;
        emit maximumLayersChanged(maximumLayers);
    }
}

/*!
    Returns the maximum number of layers for the texture provider.

    \note this has a meaning only for texture providers that have 3D or
     array target formats.
 */
int QAbstractTexture::maximumLayers() const
{
    Q_D(const QAbstractTexture);
    return d->m_maximumLayers;
}

/*!
    \property Qt3DRender::QAbstractTexture::unique

    Sets whether this texture provider should be unique by reading \a unique.
    By default this is false.

    By default the renderer based on the attributes of texture providers is
    able to compare them and detect duplicates (same size, format and image
    sources) and smartly avoid unnecessary computations. In some cases however
    (texture providers used as Qt3DCore::RenderTarget attachments) you don't want
    the renderer to perform these comparison, in which case you can set is
    unique to true.
 */
void QAbstractTexture::setUnique(bool unique)
{
    Q_D(QAbstractTexture);
    if (d->m_unique != unique) {
        d->m_unique = unique;
        emit uniqueChanged(unique);
    }
}

/*!
    Returns whether the texture should be shared with other textures in the
    renderer or not. Defaults to false as this allow in most cases the renderer
    to be faster.
 */
bool QAbstractTexture::isUnique() const
{
    Q_D(const QAbstractTexture);
    return d->m_unique;
}

/*!
    \property Qt3DRender::QAbstractTexture::format

    Holds the format of the texture provider.
 */
void QAbstractTexture::setFormat(TextureFormat format)
{
    Q_D(QAbstractTexture);
    if (d->m_format != format) {
        d->m_format = format;
        emit formatChanged(format);
    }
}

/*!
    Return the texture provider's format.
 */
QAbstractTexture::TextureFormat QAbstractTexture::format() const
{
    Q_D(const QAbstractTexture);
    return d->m_format;
}

/*!
    \property Qt3DRender::QAbstractTexture::status readonly

    Holds the current status of the texture provider.
 */
void QAbstractTexture::setStatus(Status status)
{
    Q_D(QAbstractTexture);
    if (status != d->m_status) {
        d->m_status = status;
        emit statusChanged(status);
    }
}

QAbstractTexture::Status QAbstractTexture::status() const
{
    Q_D(const QAbstractTexture);
    return d->m_status;
}

/*!
    \property Qt3DRender::QAbstractTexture::target readonly

    Holds the target format of the texture provider.

    \note The target format can only be set once.
 */
QAbstractTexture::Target QAbstractTexture::target() const
{
    Q_D(const QAbstractTexture);
    return d->m_target;
}

/*!
    Adds a new Qt3DCore::QAbstractTextureImage \a textureImage to the texture provider.

    \note Qt3DRender::QAbstractTextureImage should never be shared between multiple
    Qt3DRender::QAbstractTexture instances.
 */
void QAbstractTexture::addTextureImage(QAbstractTextureImage *textureImage)
{
    Q_D(QAbstractTexture);
    if (!d->m_textureImages.contains(textureImage)) {
        d->m_textureImages.append(textureImage);


        if (textureImage->parent() && textureImage->parent() != this)
            qWarning() << "A QAbstractTextureImage was shared, expect a crash, undefined behavior at best";
        // We need to add it as a child of the current node if it has been declared inline
        // Or not previously added as a child of the current node so that
        // 1) The backend gets notified about it's creation
        // 2) When the current node is destroyed, it gets destroyed as well
        if (!textureImage->parent())
            textureImage->setParent(this);

        if (d->m_changeArbiter != Q_NULLPTR) {
            QScenePropertyChangePtr change(new QScenePropertyChange(NodeAdded, QSceneChange::Node, id()));
            change->setPropertyName("textureImage");
            change->setValue(QVariant::fromValue(textureImage->id()));
            d->notifyObservers(change);
        }
    }
}

/*!
    Removes a Qt3DCore::QAbstractTextureImage \a textureImage from the texture provider.
 */
void QAbstractTexture::removeTextureImage(QAbstractTextureImage *textureImage)
{
    Q_D(QAbstractTexture);
    if (d->m_changeArbiter != Q_NULLPTR) {
        QScenePropertyChangePtr change(new QScenePropertyChange(NodeRemoved, QSceneChange::Node, id()));
        change->setPropertyName("textureImage");
        change->setValue(QVariant::fromValue(textureImage->id()));
        d->notifyObservers(change);
    }
    d->m_textureImages.removeOne(textureImage);
}

/*!
    Returns a list of pointers to QAbstractTextureImage objects contained in
    the texture provider.
 */
QList<QAbstractTextureImage *> QAbstractTexture::textureImages() const
{
    Q_D(const QAbstractTexture);
    return d->m_textureImages;
}

/*!
    \property Qt3DRender::QAbstractTexture::generateMipMaps

    Holds whether the texture provider should auto generate mipmaps.
 */
void QAbstractTexture::setGenerateMipMaps(bool gen)
{
    Q_D(QAbstractTexture);
    if (d->m_autoMipMap != gen) {
        d->m_autoMipMap = gen;
        emit generateMipMapsChanged(gen);
    }
}

bool QAbstractTexture::generateMipMaps() const
{
    Q_D(const QAbstractTexture);
    return d->m_autoMipMap;
}

/*!
    \property Qt3DRender::QAbstractTexture::minificationFilter

    Holds the minification filter of the texture provider.
 */
void QAbstractTexture::setMinificationFilter(Filter f)
{
    Q_D(QAbstractTexture);
    if (d->m_minFilter != f) {
        d->m_minFilter = f;
        emit minificationFilterChanged(f);
    }
}

/*!
    \property Qt3DRender::QAbstractTexture::magnificationFilter

    Holds the magnification filter of the texture provider.
 */
void QAbstractTexture::setMagnificationFilter(Filter f)
{
    Q_D(QAbstractTexture);
    if (d->m_magFilter != f) {
        d->m_magFilter = f;
        emit magnificationFilterChanged(f);
    }
}

QAbstractTexture::Filter QAbstractTexture::minificationFilter() const
{
    Q_D(const QAbstractTexture);
    return d->m_minFilter;
}

QAbstractTexture::Filter QAbstractTexture::magnificationFilter() const
{
    Q_D(const QAbstractTexture);
    return d->m_magFilter;
}

/*!
    \property Qt3DRender::QAbstractTexture::wrapMode

    Holds the wrap mode of the texture provider.
 */
void QAbstractTexture::setWrapMode(const QTextureWrapMode &wrapMode)
{
    Q_D(QAbstractTexture);
    if (d->m_wrapMode.x() != wrapMode.x()) {
        d->m_wrapMode.setX(wrapMode.x());
        QScenePropertyChangePtr e(new QScenePropertyChange(NodeUpdated, QSceneChange::Node, id()));
        e->setPropertyName("wrapModeX");
        e->setValue(static_cast<int>(d->m_wrapMode.x()));
        d->notifyObservers(e);
    }
    if (d->m_wrapMode.y() != wrapMode.y()) {
        d->m_wrapMode.setY(wrapMode.y());
        QScenePropertyChangePtr e(new QScenePropertyChange(NodeUpdated, QSceneChange::Node, id()));
        e->setPropertyName("wrapModeY");
        e->setValue(static_cast<int>(d->m_wrapMode.y()));
        d->notifyObservers(e);
    }
    if (d->m_wrapMode.z() != wrapMode.z()) {
        d->m_wrapMode.setZ(wrapMode.z());
        QScenePropertyChangePtr e(new QScenePropertyChange(NodeUpdated, QSceneChange::Node, id()));
        e->setPropertyName("wrapModeZ");
        e->setValue(static_cast<int>(d->m_wrapMode.z()));
        d->notifyObservers(e);
    }
}

QTextureWrapMode *QAbstractTexture::wrapMode()
{
    Q_D(QAbstractTexture);
    return &d->m_wrapMode;
}

/*!
    \property Qt3DRender::QAbstractTexture::maximumAnisotropy

    Holds the maximum anisotropy of the texture provider.
 */
void QAbstractTexture::setMaximumAnisotropy(float anisotropy)
{
    Q_D(QAbstractTexture);
    if (!qFuzzyCompare(d->m_maximumAnisotropy, anisotropy)) {
        d->m_maximumAnisotropy = anisotropy;
        emit maximumAnisotropyChanged(anisotropy);
    }
}

float QAbstractTexture::maximumAnisotropy() const
{
    Q_D(const QAbstractTexture);
    return d->m_maximumAnisotropy;
}

/*!
    \property Qt3DRender::QAbstractTexture::comparisonFunction

    Holds the comparison function of the texture provider.
 */
void QAbstractTexture::setComparisonFunction(QAbstractTexture::ComparisonFunction function)
{
    Q_D(QAbstractTexture);
    if (d->m_comparisonFunction != function) {
        d->m_comparisonFunction = function;
        emit comparisonFunctionChanged(function);
    }
}

QAbstractTexture::ComparisonFunction QAbstractTexture::comparisonFunction() const
{
    Q_D(const QAbstractTexture);
    return d->m_comparisonFunction;
}

/*!
    \property Qt3DRender::QAbstractTexture::comparisonMode

    Holds the comparison mode of the texture provider.
 */
void QAbstractTexture::setComparisonMode(QAbstractTexture::ComparisonMode mode)
{
    Q_D(QAbstractTexture);
    if (d->m_comparisonMode != mode) {
        d->m_comparisonMode = mode;
        emit comparisonModeChanged(mode);
    }
}

QAbstractTexture::ComparisonMode QAbstractTexture::comparisonMode() const
{
    Q_D(const QAbstractTexture);
    return d->m_comparisonMode;
}

QTextureImageDataGeneratorPtr QAbstractTexture::dataGenerator() const
{
    Q_D(const QAbstractTexture);
    return d->m_dataFunctor;
}

} // namespace Qt3DRender

QT_END_NAMESPACE
