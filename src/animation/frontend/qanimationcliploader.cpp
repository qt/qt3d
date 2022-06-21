/****************************************************************************
**
** Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
****************************************************************************/

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

// TODO Unused remove in Qt6
void QAnimationClipLoader::sceneChangeEvent(const Qt3DCore::QSceneChangePtr &)
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

Qt3DCore::QNodeCreatedChangeBasePtr QAnimationClipLoader::createNodeCreationChange() const
{
    auto creationChange = Qt3DCore::QNodeCreatedChangePtr<QAnimationClipLoaderData>::create(this);
    auto &data = creationChange->data;
    Q_D(const QAnimationClipLoader);
    data.source = d->m_source;
    return creationChange;
}

} // namespace Qt3DAnimation

QT_END_NAMESPACE

#include "moc_qanimationcliploader.cpp"
