/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qabstracttextureprovider.h"
#include "qabstracttextureprovider_p.h"
#include <Qt3DCore/qscenepropertychange.h>
#include <Qt3DRenderer/qabstracttextureimage.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

/*!
    \class Qt3D::QAbstractTextureProviderPrivate
    \internal
*/
QAbstractTextureProviderPrivate::QAbstractTextureProviderPrivate()
    : QNodePrivate()
    , m_target(QAbstractTextureProvider::Target2D)
    , m_format(QAbstractTextureProvider::Automatic)
    , m_width(1)
    , m_height(1)
    , m_depth(1)
    , m_autoMipMap(false)
    , m_minFilter(QAbstractTextureProvider::Nearest)
    , m_magFilter(QAbstractTextureProvider::Nearest)
    , m_status(QAbstractTextureProvider::Loading)
    , m_maximumAnisotropy(1.0f)
    , m_comparisonFunction(QAbstractTextureProvider::CompareLessEqual)
    , m_comparisonMode(QAbstractTextureProvider::CompareNone)
    , m_maximumLayers(1)
    , m_unique(false)
{
}

void QAbstractTextureProvider::copy(const QNode *ref)
{
    QNode::copy(ref);
    const QAbstractTextureProvider *t = static_cast<const QAbstractTextureProvider*>(ref);
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
    \class Qt3D::QAbstractTextureProvider
    \since 5.5
    \brief A base class to be used to provide textures.

    Qt3D::QAbstractTextureProvider shouldn't be used directly but rather
    through one of its subclasses. Each subclass implements a given texture
    target (2D, 2DArray, 3D, CubeMap ...) Each subclass provides a set of
    functors for each layer, cube map face and mipmap level. In turn the
    backend uses those functor to properly fill a corresponding OpenGL texture
    with data.
 */


QAbstractTextureProvider::QAbstractTextureProvider(QNode *parent)
    : QNode(*new QAbstractTextureProviderPrivate, parent)
{
}

QAbstractTextureProvider::QAbstractTextureProvider(Target target, QNode *parent)
    : QNode(*new QAbstractTextureProviderPrivate, parent)
{
    d_func()->m_target = target;
}

/*! \internal */
QAbstractTextureProvider::QAbstractTextureProvider(QAbstractTextureProviderPrivate &dd, QNode *parent)
    : QNode(dd, parent)
{
}

QAbstractTextureProvider::~QAbstractTextureProvider()
{
    // The subclasses of QAbstractTextureProvider are only there to set the type on a QTextureProvider
    // QNode::cleanup needs to be handled here and not in the subclasses.
    QNode::cleanup();
}

/*!
    Sets the size of the texture provider to width \a w, height \a h and depth \a d.
 */
void QAbstractTextureProvider::setSize(int w, int h, int d)
{
    setWidth(w);
    setHeight(h);
    setDepth(d);
}

/*!
    \property Qt3D::QAbstractTextureProvider::width

    Holds the width of the texture provider.
 */
void QAbstractTextureProvider::setWidth(int width)
{
    Q_D(QAbstractTextureProvider);
    if (d->m_width != width) {
        d->m_width = width;
        emit widthChanged();
    }
}

/*!
    \property Qt3D::QAbstractTextureProvider::height

    Holds the height of the texture provider.
 */
void QAbstractTextureProvider::setHeight(int height)
{
    Q_D(QAbstractTextureProvider);
    if (d->m_height != height) {
        d->m_height = height;
        emit heightChanged();
    }
}

/*!
    \property Qt3D::QAbstractTextureProvider::depth

    Holds the depth of the texture provider.
 */
void QAbstractTextureProvider::setDepth(int depth)
{
    Q_D(QAbstractTextureProvider);
    if (d->m_depth != depth) {
        d->m_depth = depth;
        emit depthChanged();
    }
}

int QAbstractTextureProvider::width() const
{
    Q_D(const QAbstractTextureProvider);
    return d->m_width;
}

int QAbstractTextureProvider::height() const
{
    Q_D(const QAbstractTextureProvider);
    return d->m_height;
}

int QAbstractTextureProvider::depth() const
{
    Q_D(const QAbstractTextureProvider);
    return d->m_depth;
}

/*!
    \property Qt3D::QAbstractTextureProvider::maximumLayers

    Holds the maximum layer count of the texture provider. By default, the
    maximum layer count is 1.

    \note this has a meaning only for texture providers that have 3D or
    array target formats.
 */
void QAbstractTextureProvider::setMaximumLayers(int maximumLayers)
{
    Q_D(QAbstractTextureProvider);
    if (d->m_maximumLayers != maximumLayers) {
        d->m_maximumLayers = maximumLayers;
        emit maximumLayersChanged();
    }
}

/*!
    Returns the maximum number of layers for the texture provider.

    \note this has a meaning only for texture providers that have 3D or
     array target formats.
 */
int QAbstractTextureProvider::maximumLayers() const
{
    Q_D(const QAbstractTextureProvider);
    return d->m_maximumLayers;
}

/*!
    \property Qt3D::QAbstractTextureProvider::unique

    Sets whether this texture provider should be unique by reading \a unique.
    By default this is false.

    By default the renderer based on the attributes of texture providers is
    able to compare them and detect duplicates (same size, format and image
    sources) and smartly avoid unnecessary computations. In some cases however
    (texture providers used as Qt3D::RenderTarget attachments) you don't want
    the renderer to perform these comparison, in which case you can set is
    unique to true.
 */
void QAbstractTextureProvider::setUnique(bool unique)
{
    Q_D(QAbstractTextureProvider);
    if (d->m_unique != unique) {
        d->m_unique = unique;
        emit uniqueChanged();
    }
}

/*!
    Returns whether the texture should be shared with other textures in the
    renderer or not. Defaults to false as this allow in most cases the renderer
    to be faster.
 */
bool QAbstractTextureProvider::isUnique() const
{
    Q_D(const QAbstractTextureProvider);
    return d->m_unique;
}

/*!
    \property Qt3D::QAbstractTextureProvider::format

    Holds the format of the texture provider.
 */
void QAbstractTextureProvider::setFormat(TextureFormat format)
{
    Q_D(QAbstractTextureProvider);
    if (d->m_format != format) {
        d->m_format = format;
        emit formatChanged();
    }
}

/*!
    Return the texture provider's format.
 */
QAbstractTextureProvider::TextureFormat QAbstractTextureProvider::format() const
{
    Q_D(const QAbstractTextureProvider);
    return d->m_format;
}

/*!
    \property Qt3D::QAbstractTextureProvider::status readonly

    Holds the current status of the texture provider.
 */
void QAbstractTextureProvider::setStatus(Status status)
{
    Q_D(QAbstractTextureProvider);
    if (status != d->m_status) {
        d->m_status = status;
        emit statusChanged();
    }
}

QAbstractTextureProvider::Status QAbstractTextureProvider::status() const
{
    Q_D(const QAbstractTextureProvider);
    return d->m_status;
}

/*!
    \property Qt3D::QAbstractTextureProvider::target readonly

    Holds the target format of the texture provider.

    \note The target format can only be set once.
 */
QAbstractTextureProvider::Target QAbstractTextureProvider::target() const
{
    Q_D(const QAbstractTextureProvider);
    return d->m_target;
}

/*!
    Adds a new Qt3D::QAbstractTextureImage \a textureImage to the texture provider.

    \note Qt3D::QAbstractTextureImage should never be shared between multiple
    Qt3D::QAbstractTextureProvider instances.
 */
void QAbstractTextureProvider::addTextureImage(QAbstractTextureImage *textureImage)
{
    Q_D(QAbstractTextureProvider);
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
    Removes a Qt3D::QAbstractTextureImage \a textureImage from the texture provider.
 */
void QAbstractTextureProvider::removeTextureImage(QAbstractTextureImage *textureImage)
{
    Q_D(QAbstractTextureProvider);
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
QList<QAbstractTextureImage *> QAbstractTextureProvider::textureImages() const
{
    Q_D(const QAbstractTextureProvider);
    return d->m_textureImages;
}

/*!
    \property Qt3D::QAbstractTextureProvider::generateMipMaps

    Holds whether the texture provider should auto generate mipmaps.
 */
void QAbstractTextureProvider::setGenerateMipMaps(bool gen)
{
    Q_D(QAbstractTextureProvider);
    if (d->m_autoMipMap != gen) {
        d->m_autoMipMap = gen;
        emit generateMipMapsChanged();
    }
}

bool QAbstractTextureProvider::generateMipMaps() const
{
    Q_D(const QAbstractTextureProvider);
    return d->m_autoMipMap;
}

/*!
    \property Qt3D::QAbstractTextureProvider::minificationFilter

    Holds the minification filter of the texture provider.
 */
void QAbstractTextureProvider::setMinificationFilter(Filter f)
{
    Q_D(QAbstractTextureProvider);
    if (d->m_minFilter != f) {
        d->m_minFilter = f;
        emit minificationFilterChanged();
    }
}

/*!
    \property Qt3D::QAbstractTextureProvider::magnificationFilter

    Holds the magnification filter of the texture provider.
 */
void QAbstractTextureProvider::setMagnificationFilter(Filter f)
{
    Q_D(QAbstractTextureProvider);
    if (d->m_magFilter != f) {
        d->m_magFilter = f;
        emit magnificationFilterChanged();
    }
}

QAbstractTextureProvider::Filter QAbstractTextureProvider::minificationFilter() const
{
    Q_D(const QAbstractTextureProvider);
    return d->m_minFilter;
}

QAbstractTextureProvider::Filter QAbstractTextureProvider::magnificationFilter() const
{
    Q_D(const QAbstractTextureProvider);
    return d->m_magFilter;
}

/*!
    \property Qt3D::QAbstractTextureProvider::wrapMode

    Holds the wrap mode of the texture provider.
 */
void QAbstractTextureProvider::setWrapMode(const QTextureWrapMode &wrapMode)
{
    Q_D(QAbstractTextureProvider);
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

QTextureWrapMode *QAbstractTextureProvider::wrapMode()
{
    Q_D(QAbstractTextureProvider);
    return &d->m_wrapMode;
}

/*!
    \property Qt3D::QAbstractTextureProvider::maximumAnisotropy

    Holds the maximum anisotropy of the texture provider.
 */
void QAbstractTextureProvider::setMaximumAnisotropy(float anisotropy)
{
    Q_D(QAbstractTextureProvider);
    if (!qFuzzyCompare(d->m_maximumAnisotropy, anisotropy)) {
        d->m_maximumAnisotropy = anisotropy;
        emit maximumAnisotropyChanged();
    }
}

float QAbstractTextureProvider::maximumAnisotropy() const
{
    Q_D(const QAbstractTextureProvider);
    return d->m_maximumAnisotropy;
}

/*!
    \property Qt3D::QAbstractTextureProvider::comparisonFunction

    Holds the comparison function of the texture provider.
 */
void QAbstractTextureProvider::setComparisonFunction(QAbstractTextureProvider::ComparisonFunction function)
{
    Q_D(QAbstractTextureProvider);
    if (d->m_comparisonFunction != function) {
        d->m_comparisonFunction = function;
        emit comparisonFunctionChanged();
    }
}

QAbstractTextureProvider::ComparisonFunction QAbstractTextureProvider::comparisonFunction() const
{
    Q_D(const QAbstractTextureProvider);
    return d->m_comparisonFunction;
}

/*!
    \property Qt3D::QAbstractTextureProvider::comparisonMode

    Holds the comparison mode of the texture provider.
 */
void QAbstractTextureProvider::setComparisonMode(QAbstractTextureProvider::ComparisonMode mode)
{
    Q_D(QAbstractTextureProvider);
    if (d->m_comparisonMode != mode) {
        d->m_comparisonMode = mode;
        emit comparisonModeChanged();
    }
}

QAbstractTextureProvider::ComparisonMode QAbstractTextureProvider::comparisonMode() const
{
    Q_D(const QAbstractTextureProvider);
    return d->m_comparisonMode;
}

} // Qt3D

QT_END_NAMESPACE
