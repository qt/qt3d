/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
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

#include "qtexture.h"

#include <texturedata.h>

#include <QImage>
#include <QDebug>
#include <Qt3DCore/private/qnode_p.h>
#include <Qt3DCore/qscenepropertychange.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

class QTexturePrivate : public QNodePrivate
{
public :
    QTexturePrivate(QTexture *qq)
        : QNodePrivate(qq)
        , m_target(QTexture::Target2D)
        , m_format(QTexture::RGBA8U)
        , m_width(1)
        , m_height(1)
        , m_depth(1)
        , m_autoMipMap(false)
        , m_minFilter(QTexture::Nearest)
        , m_magFilter(QTexture::Nearest)
        , m_status(QTexture::Loading)
        , m_maximumAnisotropy(1.0f)
    {}

    Q_DECLARE_PUBLIC(QTexture)

    QTexture::Target m_target;
    QTexture::TextureFormat m_format;
    int m_width, m_height, m_depth;
    bool m_autoMipMap;

    QList<TexImageDataPtr> m_data;

    QTexture::Filter m_minFilter, m_magFilter;
    // FIXME, store per direction
    QTextureWrapMode m_wrapMode;
    QTexture::Status m_status;
    float m_maximumAnisotropy;
};

void QTexture::copy(const QNode *ref)
{
    QNode::copy(ref);
    const QTexture *t = static_cast<const QTexture*>(ref);
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
    // TO DO: Copy TexImageDataPtr
}

QTexture::QTexture(QNode *parent)
    : QNode(*new QTexturePrivate(this), parent)
{
}

QTexture::QTexture(Target target, QNode *parent)
    : QNode(*new QTexturePrivate(this), parent)
{
    d_func()->m_target = target;
}

QTexture::QTexture(QTexture::Target target, QTexture::TextureFormat format,
                   int width, int height, int depth, bool mipMaps,
                   QTexture::Filter magnificationFilter, QTexture::Filter minificationFilter,
                   float maximumAnisotropy, QNode *parent)
    : QNode(*new QTexturePrivate(this), parent)
{
    d_func()->m_target = target;
    d_func()->m_format = format;
    d_func()->m_width = width;
    d_func()->m_height = height;
    d_func()->m_depth = depth;
    d_func()->m_autoMipMap = mipMaps;
    d_func()->m_magFilter = magnificationFilter;
    d_func()->m_minFilter = minificationFilter;
    d_func()->m_maximumAnisotropy = maximumAnisotropy;
}

QTexture::QTexture(QTexturePrivate &dd, QNode *parent)
    : QNode(dd, parent)
{
}

QTexture::~QTexture()
{
}

void QTexture::setSize(int w, int h, int d)
{
   setWidth(w);
   setHeight(h);
   setDepth(d);
}

void QTexture::setWidth(int width)
{
    Q_D(QTexture);
    if (d->m_width != width) {
        d->m_width = width;
        emit widthChanged();
    }
}

void QTexture::setHeight(int height)
{
    Q_D(QTexture);
    if (d->m_height != height) {
        d->m_height = height;
        emit heightChanged();
    }
}

void QTexture::setDepth(int depth)
{
    Q_D(QTexture);
    if (d->m_depth != depth) {
        d->m_depth = depth;
        emit depthChanged();
    }
}

int QTexture::width() const
{
    Q_D(const QTexture);
    return d->m_width;
}

int QTexture::height() const
{
    Q_D(const QTexture);
    return d->m_height;
}

int QTexture::depth() const
{
    Q_D(const QTexture);
    return d->m_depth;
}

void QTexture::setFormat(TextureFormat format)
{
    Q_D(QTexture);
    if (d->m_format != format) {
        d->m_format = format;
        emit formatChanged();
    }
}

QTexture::TextureFormat QTexture::format() const
{
    Q_D(const QTexture);
    return d->m_format;
}

void QTexture::setStatus(Status status)
{
    Q_D(QTexture);
    if (status != d->m_status) {
        d->m_status = status;
        emit statusChanged();
    }
}

QTexture::Status QTexture::status() const
{
    Q_D(const QTexture);
    return d->m_status;
}

QTexture::Target QTexture::target() const
{
    Q_D(const QTexture);
    return d->m_target;
}

bool QTexture::setFromQImage(QImage img, int layer)
{
    Q_D(QTexture);
    setSize(img.width(), img.height());

    if ((d->m_target != Target2D) &&
        (d->m_target != Target2DArray) &&
        (d->m_target == TargetRectangle)) {
        qWarning() << Q_FUNC_INFO << "invalid texture target";
        setStatus(Error);
        return false;
    }

    TexImageDataPtr dataPtr(new TexImageData(0, layer));
    dataPtr->setImage(img);
    addImageData(dataPtr);
    setStatus(Loaded);
    return true;
}

void QTexture::addImageData(TexImageDataPtr imgData)
{
    Q_D(QTexture);
    d->m_data.append(imgData);
}

QList<TexImageDataPtr> QTexture::imageData() const
{
    Q_D(const QTexture);
    return d->m_data;
}

void QTexture::setGenerateMipMaps(bool gen)
{
    Q_D(QTexture);
    if (d->m_autoMipMap != gen) {
        d->m_autoMipMap = gen;
        emit generateMipMapsChanged();
    }
}

bool QTexture::generateMipMaps() const
{
    Q_D(const QTexture);
    return d->m_autoMipMap;
}

void QTexture::setMinificationFilter(Filter f)
{
    Q_D(QTexture);
    if (d->m_minFilter != f) {
        d->m_minFilter = f;
        emit minificationFilterChanged();
    }
}

void QTexture::setMagnificationFilter(Filter f)
{
    Q_D(QTexture);
    if (d->m_magFilter != f) {
        d->m_magFilter = f;
        emit magnificationFilterChanged();
    }
}

QTexture::Filter QTexture::minificationFilter() const
{
    Q_D(const QTexture);
    return d->m_minFilter;
}

QTexture::Filter QTexture::magnificationFilter() const
{
    Q_D(const QTexture);
    return d->m_magFilter;
}

void QTexture::setWrapMode(const QTextureWrapMode &wrapMode)
{
    Q_D(QTexture);
    if (d->m_wrapMode.x() != wrapMode.x()) {
        d->m_wrapMode.setX(wrapMode.x());
        QScenePropertyChangePtr e(new QScenePropertyChange(NodeUpdated, this));
        e->setPropertyName(QByteArrayLiteral("wrapModeX"));
        e->setValue(static_cast<int>(d->m_wrapMode.x()));
        d->notifyObservers(e);
    }
    if (d->m_wrapMode.y() != wrapMode.y()) {
        d->m_wrapMode.setY(wrapMode.y());
        QScenePropertyChangePtr e(new QScenePropertyChange(NodeUpdated, this));
        e->setPropertyName(QByteArrayLiteral("wrapModeY"));
        e->setValue(static_cast<int>(d->m_wrapMode.y()));
        d->notifyObservers(e);
    }
    if (d->m_wrapMode.z() != wrapMode.z()) {
        d->m_wrapMode.setZ(wrapMode.z());
        QScenePropertyChangePtr e(new QScenePropertyChange(NodeUpdated, this));
        e->setPropertyName(QByteArrayLiteral("wrapModeZ"));
        e->setValue(static_cast<int>(d->m_wrapMode.z()));
        d->notifyObservers(e);
    }
}

QTextureWrapMode *QTexture::wrapMode()
{
    Q_D(QTexture);
    return &d->m_wrapMode;
}

void QTexture::setMaximumAnisotropy(float anisotropy)
{
    Q_D(QTexture);
    if (!qFuzzyCompare(d->m_maximumAnisotropy, anisotropy)) {
        d->m_maximumAnisotropy = anisotropy;
        emit maximumAnisotropyChanged();
    }
}

float QTexture::maximumAnisotropy() const
{
    Q_D(const QTexture);
    return d->m_maximumAnisotropy;
}

class QTextureWrapModePrivate : public QObjectPrivate
{
public:
    QTextureWrapModePrivate(QTextureWrapMode *qq)
        : QObjectPrivate()
        , q_ptr(qq)
        , m_x(QTextureWrapMode::ClampToEdge)
        , m_y(QTextureWrapMode::ClampToEdge)
        , m_z(QTextureWrapMode::ClampToEdge)
    {
    }

    Q_DECLARE_PUBLIC(QTextureWrapMode)
    QTextureWrapMode *q_ptr;
    QTextureWrapMode::WrapMode m_x;
    QTextureWrapMode::WrapMode m_y;
    QTextureWrapMode::WrapMode m_z;
};

QTextureWrapMode::QTextureWrapMode(WrapMode wrapMode, QObject *parent)
    : QObject(*new QTextureWrapModePrivate(this), parent)
{
    d_func()->m_x = wrapMode;
    d_func()->m_y = wrapMode;
    d_func()->m_z = wrapMode;
}

QTextureWrapMode::QTextureWrapMode(WrapMode x,WrapMode y, WrapMode z, QObject *parent)
    : QObject(*new QTextureWrapModePrivate(this), parent)
{
    d_func()->m_x = x;
    d_func()->m_y = y;
    d_func()->m_z = z;
}

void QTextureWrapMode::setX(WrapMode x)
{
    Q_D(QTextureWrapMode);
    if (d->m_x != x) {
        d->m_x = x;
        emit xChanged();
    }
}

QTextureWrapMode::WrapMode QTextureWrapMode::x() const
{
    Q_D(const QTextureWrapMode);
    return d->m_x;
}

void QTextureWrapMode::setY(WrapMode y)
{
    Q_D(QTextureWrapMode);
    if (d->m_y != y) {
        d->m_y = y;
        emit yChanged();
    }
}

QTextureWrapMode::WrapMode QTextureWrapMode::y() const
{
    Q_D(const QTextureWrapMode);
    return d->m_y;
}

void QTextureWrapMode::setZ(WrapMode z)
{
    Q_D(QTextureWrapMode);
    if (d->m_z != z) {
        d->m_z = z;
        emit zChanged();
    }
}

QTextureWrapMode::WrapMode QTextureWrapMode::z() const
{
    Q_D(const QTextureWrapMode);
    return d->m_z;
}

} // namespace Qt3D

QT_END_NAMESPACE
