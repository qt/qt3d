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
    : Qt3DAnimation::QAbstractClipAnimatorPrivate()
    , m_blendTreeRoot(nullptr)
{
}

/*!
    \qmltype BlendedClipAnimator
    \instantiates Qt3DAnimation::QBlendedClipAnimator
    \inqmlmodule Qt3D.Animation

    \brief Performs an animation based on a tree of blend nodes

    \note The blend node tree should only be edited when the clip is not
    running

    \since 5.9
*/

/*!
    \class Qt3DAnimation::QBlendedClipAnimator
    \inmodule Qt3DAnimation
    \inherits Qt3DCore::QComponent

    \brief Performs an animation based on a tree of blend nodes

    \note The blend node tree should only be edited when the clip is not
    running

    \since 5.9
*/
QBlendedClipAnimator::QBlendedClipAnimator(Qt3DCore::QNode *parent)
    : Qt3DAnimation::QAbstractClipAnimator(*new QBlendedClipAnimatorPrivate, parent)
{
}

QBlendedClipAnimator::QBlendedClipAnimator(QBlendedClipAnimatorPrivate &dd, Qt3DCore::QNode *parent)
    : Qt3DAnimation::QAbstractClipAnimator(dd, parent)
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

Qt3DCore::QNodeCreatedChangeBasePtr QBlendedClipAnimator::createNodeCreationChange() const
{
    auto creationChange = Qt3DCore::QNodeCreatedChangePtr<QBlendedClipAnimatorData>::create(this);
    QBlendedClipAnimatorData &data = creationChange->data;
    Q_D(const QBlendedClipAnimator);
    data.blendTreeRootId = Qt3DCore::qIdForNode(d->m_blendTreeRoot);
    data.mapperId = Qt3DCore::qIdForNode(d->m_mapper);
    data.running = d->m_running;
    data.loops = d->m_loops;
    return creationChange;
}

} // namespace Qt3DAnimation

QT_END_NAMESPACE
