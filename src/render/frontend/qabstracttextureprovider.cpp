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

QAbstractTextureProviderPrivate::QAbstractTextureProviderPrivate(QAbstractTextureProvider *qq)
    : QNodePrivate(qq)
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
    \brief Qt3D::QAbstractTextureProvider is a base class to be used to provide textures.

    Qt3D::QAbstractTextureProvider shouldn't be used directly but rather
    through one of its subclasses. Each subclass implements a given texture
    target (2D, 2DArray, 3D, CubeMap ...) Each subclass provides a set of
    functors for each layer, cube map face and mipmap level. In turn the
    backend uses those functor to properly fill a corresponding OpenGL texture
    with data.
 */


QAbstractTextureProvider::QAbstractTextureProvider(QNode *parent)
    : QNode(*new QAbstractTextureProviderPrivate(this), parent)
{
}

QAbstractTextureProvider::QAbstractTextureProvider(Target target, QNode *parent)
    : QNode(*new QAbstractTextureProviderPrivate(this), parent)
{
    d_func()->m_target = target;
}

QAbstractTextureProvider::QAbstractTextureProvider(QAbstractTextureProviderPrivate &dd, QNode *parent)
    : QNode(dd, parent)
{
}

QAbstractTextureProvider::~QAbstractTextureProvider()
{
}

/*!
    Sets the size of the texture provider to width \a, height \a h and depth \a d.
 */
void QAbstractTextureProvider::setSize(int w, int h, int d)
{
   setWidth(w);
   setHeight(h);
   setDepth(d);
}

/*!
    Sets the width of the texture provider to \a width.
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
    Sets the height of the texture provider to \a height.
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
    Sets the depth of the texture provider to \a depth.
 */
void QAbstractTextureProvider::setDepth(int depth)
{
    Q_D(QAbstractTextureProvider);
    if (d->m_depth != depth) {
        d->m_depth = depth;
        emit depthChanged();
    }
}

/*!
    \returns the width of the texture provider.
 */
int QAbstractTextureProvider::width() const
{
    Q_D(const QAbstractTextureProvider);
    return d->m_width;
}

/*!
    \returns the height of the texture provider.
 */
int QAbstractTextureProvider::height() const
{
    Q_D(const QAbstractTextureProvider);
    return d->m_height;
}

/*!
    \returns the depth of the texture provider.
 */
int QAbstractTextureProvider::depth() const
{
    Q_D(const QAbstractTextureProvider);
    return d->m_depth;
}

/*!
    Sets the maximum layers count to \a maximumLayers, by defaults, the maximum
    layer count is 1.

    \note this has a meaning only for texture providers that
    have 3D or array target formats.
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
    \return return the maximum layers count of the texture provider.
    \note this has a meaning only for texture providers that have 3D or array target formats.
 */
int QAbstractTextureProvider::maximumLayers() const
{
    Q_D(const QAbstractTextureProvider);
    return d->m_maximumLayers;
}

/*!
    Sets the format of the texture provider to \a format.
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
    \returns the format of the texture provider.
 */
QAbstractTextureProvider::TextureFormat QAbstractTextureProvider::format() const
{
    Q_D(const QAbstractTextureProvider);
    return d->m_format;
}

/*!
    Sets the current status of the texture provider to \a status.
 */
void QAbstractTextureProvider::setStatus(Status status)
{
    Q_D(QAbstractTextureProvider);
    if (status != d->m_status) {
        d->m_status = status;
        emit statusChanged();
    }
}

/*!
    \returns the current status of the texture provider.
 */
QAbstractTextureProvider::Status QAbstractTextureProvider::status() const
{
    Q_D(const QAbstractTextureProvider);
    return d->m_status;
}

/*!
    \returns the target format of the texture provider.
    \note: The target format can only be set once.
 */
QAbstractTextureProvider::Target QAbstractTextureProvider::target() const
{
    Q_D(const QAbstractTextureProvider);
    return d->m_target;
}

/*!
    Adds a new Qt3D::QAbstractTextureImage \a texture image to the texture provider.
    \note: Qt3D::QAbstractTextureImage should never be shared by several Qt3D::QAbstractTextureProvider
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
    Removes a Qt3D::QAbstractTextureImage \a texture image form the texture provider.
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
\return the list of QAbstractTextureImage * contained in the texture provider.
 */
QList<QAbstractTextureImage *> QAbstractTextureProvider::textureImages() const
{
    Q_D(const QAbstractTextureProvider);
    return d->m_textureImages;
}

/*!
    Sets whether the texture provider should auto generate mipmaps.
 */
void QAbstractTextureProvider::setGenerateMipMaps(bool gen)
{
    Q_D(QAbstractTextureProvider);
    if (d->m_autoMipMap != gen) {
        d->m_autoMipMap = gen;
        emit generateMipMapsChanged();
    }
}

/*!
    \returns whether texture provider auto generates mipmaps.
 */
bool QAbstractTextureProvider::generateMipMaps() const
{
    Q_D(const QAbstractTextureProvider);
    return d->m_autoMipMap;
}

/*!
    Sets the minification filter of the texture provider to \a f.
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
    Sets the magnification filter of the texture provider to \a f.
 */
void QAbstractTextureProvider::setMagnificationFilter(Filter f)
{
    Q_D(QAbstractTextureProvider);
    if (d->m_magFilter != f) {
        d->m_magFilter = f;
        emit magnificationFilterChanged();
    }
}

/*!
    \returns the minification filter of the texture provider.
 */
QAbstractTextureProvider::Filter QAbstractTextureProvider::minificationFilter() const
{
    Q_D(const QAbstractTextureProvider);
    return d->m_minFilter;
}

/*!
    \returns the magnification filter of the texture provider.
 */
QAbstractTextureProvider::Filter QAbstractTextureProvider::magnificationFilter() const
{
    Q_D(const QAbstractTextureProvider);
    return d->m_magFilter;
}

/*!
    Sets the wrap mode of the texture provider to \a wrapMode.
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

/*!
    \returns the wrap mode of the texture provider.
 */
QTextureWrapMode *QAbstractTextureProvider::wrapMode()
{
    Q_D(QAbstractTextureProvider);
    return &d->m_wrapMode;
}

/*!
    Sets the maximum anisotropy of the texture provider to \a anisotropy.
 */
void QAbstractTextureProvider::setMaximumAnisotropy(float anisotropy)
{
    Q_D(QAbstractTextureProvider);
    if (!qFuzzyCompare(d->m_maximumAnisotropy, anisotropy)) {
        d->m_maximumAnisotropy = anisotropy;
        emit maximumAnisotropyChanged();
    }
}

/*!
    \returns the maximum anisotropy of the texture provider.
 */
float QAbstractTextureProvider::maximumAnisotropy() const
{
    Q_D(const QAbstractTextureProvider);
    return d->m_maximumAnisotropy;
}

/*!
    Sets the comparison function of the texture provider to \a function.
 */
void QAbstractTextureProvider::setComparisonFunction(QAbstractTextureProvider::ComparisonFunction function)
{
    Q_D(QAbstractTextureProvider);
    if (d->m_comparisonFunction != function) {
        d->m_comparisonFunction = function;
        emit comparisonFunctionChanged();
    }
}

/*!
    \returns the comparison function of the texture provider.
 */
QAbstractTextureProvider::ComparisonFunction QAbstractTextureProvider::comparisonFunction() const
{
    Q_D(const QAbstractTextureProvider);
    return d->m_comparisonFunction;
}

/*!
    Sets the comparison mode of the texture provider to \a mode.
 */
void QAbstractTextureProvider::setComparisonMode(QAbstractTextureProvider::ComparisonMode mode)
{
    Q_D(QAbstractTextureProvider);
    if (d->m_comparisonMode != mode) {
        d->m_comparisonMode = mode;
        emit comparisonModeChanged();
    }
}

/*!
   \returns the comparison mode of the texture provider.
 */
QAbstractTextureProvider::ComparisonMode QAbstractTextureProvider::comparisonMode() const
{
    Q_D(const QAbstractTextureProvider);
    return d->m_comparisonMode;
}

} // Qt3D

QT_END_NAMESPACE
