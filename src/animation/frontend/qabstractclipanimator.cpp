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

#include "qabstractclipanimator.h"
#include "qabstractclipanimator_p.h"
#include <Qt3DAnimation/qchannelmapper.h>

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {

QAbstractClipAnimatorPrivate::QAbstractClipAnimatorPrivate()
    : Qt3DCore::QComponentPrivate()
    , m_mapper(nullptr)
    , m_running(false)
    , m_loops(1)
{
}

QAbstractClipAnimator::QAbstractClipAnimator(Qt3DCore::QNode *parent)
    : Qt3DCore::QComponent(*new QAbstractClipAnimatorPrivate, parent)
{
}

QAbstractClipAnimator::QAbstractClipAnimator(QAbstractClipAnimatorPrivate &dd, Qt3DCore::QNode *parent)
    : Qt3DCore::QComponent(dd, parent)
{
}

QAbstractClipAnimator::~QAbstractClipAnimator()
{
}

bool QAbstractClipAnimator::isRunning() const
{
    Q_D(const QAbstractClipAnimator);
    return d->m_running;
}

QChannelMapper *QAbstractClipAnimator::channelMapper() const
{
    Q_D(const QAbstractClipAnimator);
    return d->m_mapper;
}

int QAbstractClipAnimator::loops() const
{
    Q_D(const QAbstractClipAnimator);
    return d->m_loops;
}

void QAbstractClipAnimator::setRunning(bool running)
{
    Q_D(QAbstractClipAnimator);
    if (d->m_running == running)
        return;

    d->m_running = running;
    emit runningChanged(running);
}

void QAbstractClipAnimator::setChannelMapper(QChannelMapper *mapping)
{
    Q_D(QAbstractClipAnimator);
    if (d->m_mapper == mapping)
        return;

    if (d->m_mapper)
        d->unregisterDestructionHelper(d->m_mapper);

    if (mapping && !mapping->parent())
        mapping->setParent(this);
    d->m_mapper = mapping;

    // Ensures proper bookkeeping
    if (d->m_mapper)
        d->registerDestructionHelper(d->m_mapper, &QAbstractClipAnimator::setChannelMapper, d->m_mapper);
    emit channelMapperChanged(mapping);
}

void QAbstractClipAnimator::setLoops(int loops)
{
    Q_D(QAbstractClipAnimator);
    if (d->m_loops == loops)
        return;

    d->m_loops = loops;
    emit loopsChanged(loops);
}

} // namespace Qt3DAnimation

QT_END_NAMESPACE
