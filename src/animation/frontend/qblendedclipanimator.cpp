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
#include "qblendedclipanimator.h"
#include "qblendedclipanimator_p.h"
#include <Qt3DAnimation/qabstractclipblendnode.h>
#include <Qt3DAnimation/qchannelmapper.h>

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {

QBlendedClipAnimatorPrivate::QBlendedClipAnimatorPrivate()
    : Qt3DCore::QComponentPrivate()
    , m_blendTreeRoot(nullptr)
    , m_mapper(nullptr)
    , m_running(false)
{
}

QBlendedClipAnimator::QBlendedClipAnimator(Qt3DCore::QNode *parent)
    : Qt3DCore::QComponent(*new QBlendedClipAnimatorPrivate, parent)
{
}

QBlendedClipAnimator::QBlendedClipAnimator(QBlendedClipAnimatorPrivate &dd, Qt3DCore::QNode *parent)
    : Qt3DCore::QComponent(dd, parent)
{
}

QBlendedClipAnimator::~QBlendedClipAnimator()
{
}

QAbstractClipBlendNode *QBlendedClipAnimator::blendTree() const
{
    Q_D(const QBlendedClipAnimator);
    return d->m_blendTreeRoot;
}

bool QBlendedClipAnimator::isRunning() const
{
    Q_D(const QBlendedClipAnimator);
    return d->m_running;
}

QChannelMapper *QBlendedClipAnimator::channelMapper() const
{
    Q_D(const QBlendedClipAnimator);
    return d->m_mapper;
}

void QBlendedClipAnimator::setBlendTree(QAbstractClipBlendNode *blendTree)
{
    Q_D(QBlendedClipAnimator);
    if (d->m_blendTreeRoot == blendTree)
        return;

    if (d->m_blendTreeRoot)
        d->unregisterDestructionHelper(d->m_blendTreeRoot);

    if (blendTree != nullptr && blendTree->parent() == nullptr)
        blendTree->setParent(this);

    d->m_blendTreeRoot = blendTree;

    if (d->m_blendTreeRoot)
        d->registerDestructionHelper(d->m_blendTreeRoot, &QBlendedClipAnimator::setBlendTree, d->m_blendTreeRoot);

    emit blendTreeChanged(blendTree);
}

void QBlendedClipAnimator::setRunning(bool running)
{
    Q_D(QBlendedClipAnimator);
    if (d->m_running == running)
        return;

    d->m_running = running;
    emit runningChanged(running);
}

void QBlendedClipAnimator::setChannelMapper(QChannelMapper *mapping)
{
    Q_D(QBlendedClipAnimator);
    if (d->m_mapper == mapping)
        return;

    if (d->m_mapper)
        d->unregisterDestructionHelper(d->m_mapper);

    if (mapping && !mapping->parent())
        mapping->setParent(this);
    d->m_mapper = mapping;

    if (d->m_mapper)
        d->registerDestructionHelper(d->m_mapper, &QBlendedClipAnimator::setChannelMapper, d->m_mapper);
    emit channelMapperChanged(mapping);
}

Qt3DCore::QNodeCreatedChangeBasePtr QBlendedClipAnimator::createNodeCreationChange() const
{
    auto creationChange = Qt3DCore::QNodeCreatedChangePtr<QBlendedClipAnimatorData>::create(this);
    QBlendedClipAnimatorData &data = creationChange->data;
    Q_D(const QBlendedClipAnimator);
    data.blendTreeRootId = Qt3DCore::qIdForNode(d->m_blendTreeRoot);
    data.mapperId = Qt3DCore::qIdForNode(d->m_mapper);
    data.running = d->m_running;
    return creationChange;
}

} // namespace Qt3DAnimation

QT_END_NAMESPACE
