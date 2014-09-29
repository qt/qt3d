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
        , m_wrapMode(QTexture::ClampToEdge)
        , m_status(QTexture::Loading)
    {}

    void copy(const QNodePrivate *ref) Q_DECL_OVERRIDE;

    Q_DECLARE_PUBLIC(QTexture)

    QTexture::Target m_target;
    QTexture::TextureFormat m_format;
    int m_width, m_height, m_depth;
    bool m_autoMipMap;

    QList<TexImageDataPtr> m_data;

    QTexture::Filter m_minFilter, m_magFilter;
    // FIXME, store per direction
    QTexture::WrapMode m_wrapMode;
    QTexture::Status m_status;
};

void QTexturePrivate::copy(const QNodePrivate *ref)
{
    QNodePrivate::copy(ref);
    const QTexturePrivate *t = static_cast<const QTexturePrivate *>(ref);
    m_target = t->m_target;
    m_width = t->m_width;
    m_height = t->m_height;
    m_depth = t->m_depth;
    m_format = t->m_format;
    m_wrapMode = t->m_wrapMode;
    m_minFilter = t->m_minFilter;
    m_magFilter = t->m_magFilter;
    m_autoMipMap = t->m_autoMipMap;
}

QTexture::QTexture(QNode *parent)
    : QNode(*new QTexturePrivate(this), parent)
{
}

QTexture::QTexture(QTexturePrivate &dd, QNode *parent)
    : QNode(dd, parent)
{
}

QTexture::~QTexture()
{
}


void QTexture::setTarget(Target target)
{
    Q_D(QTexture);
    if (d->m_target != target) {
        d->m_target = target;
        emit targetChanged();

        if (d->m_changeArbiter != Q_NULLPTR) {
            QScenePropertyChangePtr change(new QScenePropertyChange(NodeUpdated, this));
            change->setPropertyName(QByteArrayLiteral("target"));
            change->setValue(target);
            d->notifyObservers(change);
        }
    }
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

        if (d->m_changeArbiter != Q_NULLPTR) {
            QScenePropertyChangePtr change(new QScenePropertyChange(NodeUpdated, this));
            change->setPropertyName(QByteArrayLiteral("width"));
            change->setValue(width);
            d->notifyObservers(change);
        }
    }
}

void QTexture::setHeight(int height)
{
    Q_D(QTexture);
    if (d->m_height != height) {
        d->m_height = height;
        emit heightChanged();

        if (d->m_changeArbiter != Q_NULLPTR) {
            QScenePropertyChangePtr change(new QScenePropertyChange(NodeUpdated, this));
            change->setPropertyName(QByteArrayLiteral("height"));
            change->setValue(height);
            d->notifyObservers(change);
        }
    }
}

void QTexture::setDepth(int depth)
{
    Q_D(QTexture);
    if (d->m_depth != depth) {
        d->m_depth = depth;
        emit depthChanged();

        if (d->m_changeArbiter != Q_NULLPTR) {
            QScenePropertyChangePtr change(new QScenePropertyChange(NodeUpdated, this));
            change->setPropertyName(QByteArrayLiteral("depth"));
            change->setValue(depth);
            d->notifyObservers(change);
        }
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

        if (d->m_changeArbiter != Q_NULLPTR) {
            QScenePropertyChangePtr change(new QScenePropertyChange(NodeUpdated, this));
            change->setPropertyName(QByteArrayLiteral("format"));
            change->setValue(format);
            d->notifyObservers(change);
        }
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

QNode *QTexture::doClone() const
{
    // TO DO: Copy TexImageDataPtr
    QTexture *clone = new QTexture();
    clone->d_func()->copy(d_func());
    return clone;
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

        if (d->m_changeArbiter != Q_NULLPTR) {
            QScenePropertyChangePtr change(new QScenePropertyChange(NodeUpdated, this));
            change->setPropertyName(QByteArrayLiteral("mipmaps"));
            change->setValue(gen);
            d->notifyObservers(change);
        }
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

        if (d->m_changeArbiter != Q_NULLPTR) {
            QScenePropertyChangePtr change(new QScenePropertyChange(NodeUpdated, this));
            change->setPropertyName(QByteArrayLiteral("minificationFilter"));
            change->setValue(f);
            d->notifyObservers(change);
        }
    }
}

void QTexture::setMagnificationFilter(Filter f)
{
    Q_D(QTexture);
    if (d->m_magFilter != f) {
        d->m_magFilter = f;
        emit magnificationFilterChanged();

        if (d->m_changeArbiter != Q_NULLPTR) {
            QScenePropertyChangePtr change(new QScenePropertyChange(NodeUpdated, this));
            change->setPropertyName(QByteArrayLiteral("magnificationFilter"));
            change->setValue(f);
            d->notifyObservers(change);
        }
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

void QTexture::setWrapMode(WrapMode wrapMode)
{
    Q_D(QTexture);
    if (d->m_wrapMode != wrapMode) {
        d->m_wrapMode = wrapMode;
        emit wrapModeChanged();

        if (d->m_changeArbiter != Q_NULLPTR) {
            QScenePropertyChangePtr change(new QScenePropertyChange(NodeUpdated, this));
            change->setPropertyName(QByteArrayLiteral("wrapMode"));
            change->setValue(wrapMode);
            d->notifyObservers(change);
        }
    }
}

QTexture::WrapMode QTexture::wrapMode() const
{
    Q_D(const QTexture);
    return d->m_wrapMode;
}

} // namespace Qt3D

QT_END_NAMESPACE
