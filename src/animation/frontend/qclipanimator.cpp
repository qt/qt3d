/****************************************************************************
**
** Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
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

#include "qclipanimator.h"
#include "qclipanimator_p.h"
#include <Qt3DAnimation/qanimationclip.h>
#include <Qt3DAnimation/qchannelmapper.h>

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {

QClipAnimatorPrivate::QClipAnimatorPrivate()
    : Qt3DCore::QComponentPrivate()
    , m_clip(nullptr)
    , m_mapper(nullptr)
    , m_running(false)
{
}

QClipAnimator::QClipAnimator(Qt3DCore::QNode *parent)
    : Qt3DCore::QComponent(*new QClipAnimatorPrivate, parent)
{
}

QClipAnimator::QClipAnimator(QClipAnimatorPrivate &dd, Qt3DCore::QNode *parent)
    : Qt3DCore::QComponent(dd, parent)
{
}

QClipAnimator::~QClipAnimator()
{
}

QAnimationClip *QClipAnimator::clip() const
{
    Q_D(const QClipAnimator);
    return d->m_clip;
}

bool QClipAnimator::isRunning() const
{
    Q_D(const QClipAnimator);
    return d->m_running;
}

QChannelMapper *QClipAnimator::channelMapper() const
{
    Q_D(const QClipAnimator);
    return d->m_mapper;
}

void QClipAnimator::setClip(QAnimationClip *clip)
{
    Q_D(QClipAnimator);
    if (d->m_clip == clip)
        return;

    if (d->m_clip)
        d->unregisterDestructionHelper(d->m_clip);

    if (clip && !clip->parent())
        clip->setParent(this);
    d->m_clip = clip;

    // Ensures proper bookkeeping
    if (d->m_clip)
        d->registerDestructionHelper(d->m_clip, &QClipAnimator::setClip, d->m_clip);
    emit clipChanged(clip);
}

void QClipAnimator::setRunning(bool running)
{
    Q_D(QClipAnimator);
    if (d->m_running == running)
        return;

    d->m_running = running;
    emit runningChanged(running);
}

void QClipAnimator::setChannelMapper(QChannelMapper *mapping)
{
    Q_D(QClipAnimator);
    if (d->m_mapper == mapping)
        return;

    if (d->m_mapper)
        d->unregisterDestructionHelper(d->m_mapper);

    if (mapping && !mapping->parent())
        mapping->setParent(this);
    d->m_mapper = mapping;

    // Ensures proper bookkeeping
    if (d->m_mapper)
        d->registerDestructionHelper(d->m_mapper, &QClipAnimator::setChannelMapper, d->m_mapper);
    emit channelMapperChanged(mapping);
}

Qt3DCore::QNodeCreatedChangeBasePtr QClipAnimator::createNodeCreationChange() const
{
    auto creationChange = Qt3DCore::QNodeCreatedChangePtr<QClipAnimatorData>::create(this);
    auto &data = creationChange->data;
    Q_D(const QClipAnimator);
    data.clipId = Qt3DCore::qIdForNode(d->m_clip);
    data.mapperId = Qt3DCore::qIdForNode(d->m_mapper);
    data.running = d->m_running;
    return creationChange;
}

} // namespace Qt3DAnimation

QT_END_NAMESPACE
