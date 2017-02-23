/****************************************************************************
**
** Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
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

#include "qadditiveclipblend.h"
#include "qadditiveclipblend_p.h"
#include <Qt3DAnimation/qclipblendnodecreatedchange.h>

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {


/*!
    \qmltype AdditiveClipBlend
    \instantiates Qt3DAnimation::QAdditiveClipBlend
    \inqmlmodule Qt3D.Animation

    \brief Performs an addition of two animation clips based on a
    normalized factor

    \since 5.9

    AdditiveClipBlend can be useful to create advanced animation effects based on
    individual animation clips. For instance, given a player character, additive
    blending could be used to combine a walking animation clip with an injured
    animation clip based on a blend factor that increases the more the player
    gets injured. This would then allow with blend factor == 0 to have a non
    injured walking player, with blend factor == 1 a fully injured player, with
    blend factor == 0.5 a partially walking and injured player.

    \sa BlendedClipAnimator
*/

/*!
    \class Qt3DAnimation::QAdditiveClipBlend
    \inmodule Qt3DAnimation
    \inherits Qt3DAnimation::QAbstractClipBlendNode

    \brief Performs an addition of two animation clips based on a
    normalized factor

    \since 5.9

    QAdditiveClipBlend can be useful to create advanced animation effects based on
    individual animation clips. For instance, given a player character, additive
    blending could be used to combine a walking animation clip with an injured
    animation clip based on a blend factor that increases the more the player
    gets injured. This would then allow with blend factor == 0 to have a non
    injured walking player, with blend factor == 1 a fully injured player, with
    blend factor == 0.5 a partially walking and injured player.

    \sa QBlendedClipAnimator
*/

QAdditiveClipBlendPrivate::QAdditiveClipBlendPrivate()
    : QAbstractClipBlendNodePrivate()
    , m_baseClip(nullptr)
    , m_additiveClip(nullptr)
    , m_additiveFactor(0.0f)
{
}

QAdditiveClipBlend::QAdditiveClipBlend(Qt3DCore::QNode *parent)
    : QAbstractClipBlendNode(*new QAdditiveClipBlendPrivate(), parent)
{
}

QAdditiveClipBlend::QAdditiveClipBlend(QAdditiveClipBlendPrivate &dd, Qt3DCore::QNode *parent)
    : QAbstractClipBlendNode(dd, parent)
{
}

QAdditiveClipBlend::~QAdditiveClipBlend()
{
}

Qt3DCore::QNodeCreatedChangeBasePtr QAdditiveClipBlend::createNodeCreationChange() const
{
    Q_D(const QAdditiveClipBlend);
    auto creationChange = QClipBlendNodeCreatedChangePtr<QAdditiveClipBlendData>::create(this);
    QAdditiveClipBlendData &data = creationChange->data;
    data.baseClipId = Qt3DCore::qIdForNode(d->m_baseClip);
    data.additiveClipId = Qt3DCore::qIdForNode(d->m_additiveClip);
    data.additiveFactor = d->m_additiveFactor;
    return creationChange;
}

/*!
    \qmlproperty real AdditiveClipBlend::additiveFactor

    Specifies the blending factor between 0 and 1 to control the blending of
    two animation clips.
*/
/*!
    \property QAdditiveClipBlend::additiveFactor

    Specifies the blending factor between 0 and 1 to control the blending of
    two animation clips.
 */
float QAdditiveClipBlend::additiveFactor() const
{
    Q_D(const QAdditiveClipBlend);
    return d->m_additiveFactor;
}

QAbstractClipBlendNode *QAdditiveClipBlend::baseClip() const
{
    Q_D(const QAdditiveClipBlend);
    return d->m_baseClip;
}

QAbstractClipBlendNode *QAdditiveClipBlend::additiveClip() const
{
    Q_D(const QAdditiveClipBlend);
    return d->m_additiveClip;
}

void QAdditiveClipBlend::setAdditiveFactor(float additiveFactor)
{
    Q_D(QAdditiveClipBlend);
    if (d->m_additiveFactor == additiveFactor)
        return;

    d->m_additiveFactor = additiveFactor;
    emit additiveFactorChanged(additiveFactor);
}

void QAdditiveClipBlend::setBaseClip(QAbstractClipBlendNode *baseClip)
{
    Q_D(QAdditiveClipBlend);
    if (d->m_baseClip == baseClip)
        return;

    if (d->m_baseClip)
        d->unregisterDestructionHelper(d->m_baseClip);

    if (baseClip && !baseClip->parent())
        baseClip->setParent(this);
    d->m_baseClip = baseClip;

    // Ensures proper bookkeeping
    if (d->m_baseClip)
        d->registerDestructionHelper(d->m_baseClip, &QAdditiveClipBlend::setBaseClip, d->m_baseClip);

    emit baseClipChanged(baseClip);
}

void QAdditiveClipBlend::setAdditiveClip(QAbstractClipBlendNode *additiveClip)
{
    Q_D(QAdditiveClipBlend);
    if (d->m_additiveClip == additiveClip)
        return;

    if (d->m_additiveClip)
        d->unregisterDestructionHelper(d->m_additiveClip);

    if (additiveClip && !additiveClip->parent())
        additiveClip->setParent(this);
    d->m_additiveClip = additiveClip;

    // Ensures proper bookkeeping
    if (d->m_additiveClip)
        d->registerDestructionHelper(d->m_additiveClip, &QAdditiveClipBlend::setAdditiveClip, d->m_additiveClip);
    emit additiveClipChanged(additiveClip);
}

/*!
    \qmlproperty list<AnimationClip> AdditiveClipBlend::clips

    Holds the list of AnimationClip nodes against which the blending is performed.

    \note Only the two first AnimationClip are used, subsequent ones are ignored
*/


/*!
    \fn void QAdditiveClipBlend::addClip(QAnimationClip *clip);
    Adds a \a clip to the blending node's clips list.

    \note Only the two first AnimationClip are used, subsequent ones are ignored
 */

/*!
    \fn void QAdditiveClipBlend::removeClip(QAnimationClip *clip);
    Removes a \a clip from the blending node's clips list.
 */

/*!
    \fn QVector<QAnimationClip *> QAdditiveClipBlend::clips() const;
    Returns the list of QAnimationClip against which the blending is performed.
 */


} // Qt3DAnimation

QT_END_NAMESPACE
