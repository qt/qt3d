// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qanimationcliploader.h"
#include "qanimationcliploader_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {

QAnimationClipLoaderPrivate::QAnimationClipLoaderPrivate()
    : QAbstractAnimationClipPrivate()
    , m_source()
    , m_status(QAnimationClipLoader::NotReady)
{
}

void QAnimationClipLoaderPrivate::setStatus(QAnimationClipLoader::Status status)
{
    Q_Q(QAnimationClipLoader);
    if (status != m_status) {
        m_status = status;
        const bool blocked = q->blockNotifications(true);
        emit q->statusChanged(m_status);
        q->blockNotifications(blocked);
    }
}

/*!
    \enum Qt3DAnimation::QAnimationClipLoader::Status

    This enum identifies the status of animation clip.

    \value NotReady              The clip has not been loaded yet
    \value Ready                 The clip was successfully loaded
    \value Error                 An error occurred while loading the clip
*/
/*!
    \property Qt3DAnimation::QAnimationClipLoader::status

    This property contains the status of the animation clip.
*/
/*!
    \property Qt3DAnimation::QAnimationClipLoader::source

    Holds the source URL from which to load the animation clip. Currently
    glTF2 and the native Qt 3D json animation file formats are supported.

    In the case where a file contains multiple animations, it is possible
    to select which animation should be loaded by way of query parameters
    on the source url. The accepted query parameters are animationIndex and
    animationName. If both are specified, animationName is ignored.

    If a file contains only a single animation, there is no need to specify
    the animationIndex or animationName. We simply use the one available
    animation.
*/
/*!
    \class Qt3DAnimation::QAnimationClipLoader
    \inherits QAbstractAnimationClip
    \inmodule Qt3DAnimation
    \brief Enables loading key frame animation data from a file.
*/

QAnimationClipLoader::QAnimationClipLoader(Qt3DCore::QNode *parent)
    : QAbstractAnimationClip(*new QAnimationClipLoaderPrivate, parent)
{
}

QAnimationClipLoader::QAnimationClipLoader(const QUrl &source,
                                           Qt3DCore::QNode *parent)
    : QAbstractAnimationClip(*new QAnimationClipLoaderPrivate, parent)
{
    setSource(source);
}

QAnimationClipLoader::QAnimationClipLoader(QAnimationClipLoaderPrivate &dd, Qt3DCore::QNode *parent)
    : QAbstractAnimationClip(dd, parent)
{
}

QAnimationClipLoader::~QAnimationClipLoader()
{
}

QUrl QAnimationClipLoader::source() const
{
    Q_D(const QAnimationClipLoader);
    return d->m_source;
}

/*!
    Returns the status of the animation clip.
*/
QAnimationClipLoader::Status QAnimationClipLoader::status() const
{
    Q_D(const QAnimationClipLoader);
    return d->m_status;
}

void QAnimationClipLoader::setSource(const QUrl &source)
{
    Q_D(QAnimationClipLoader);
    if (d->m_source == source)
        return;

    d->m_source = source;
    emit sourceChanged(source);
}

} // namespace Qt3DAnimation

QT_END_NAMESPACE

#include "moc_qanimationcliploader.cpp"
