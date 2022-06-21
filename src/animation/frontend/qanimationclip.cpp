// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qanimationclip.h"
#include "qanimationclip_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {

QAnimationClipPrivate::QAnimationClipPrivate()
    : QAbstractAnimationClipPrivate()
    , m_clipData()
{
}

/*!
    \class Qt3DAnimation::QAnimationClip
    \inherits Qt3dAnimation::QAbstractAnimationClip
    \inmodule Qt3DAnimation
    \brief Specifies key frame animation data.
*/
QAnimationClip::QAnimationClip(Qt3DCore::QNode *parent)
    : QAbstractAnimationClip(*new QAnimationClipPrivate, parent)
{
}

QAnimationClip::QAnimationClip(QAnimationClipPrivate &dd, Qt3DCore::QNode *parent)
    : QAbstractAnimationClip(dd, parent)
{
}

QAnimationClip::~QAnimationClip()
{
}

/*!
    Returns the data describing the animation clip.
*/
QAnimationClipData QAnimationClip::clipData() const
{
    Q_D(const QAnimationClip);
    return d->m_clipData;
}

/*!
    Set the parameters of the clip according to the \a clipData provided.
*/
void QAnimationClip::setClipData(const Qt3DAnimation::QAnimationClipData &clipData)
{
    Q_D(QAnimationClip);
    if (d->m_clipData == clipData)
        return;

    d->m_clipData = clipData;
    emit clipDataChanged(clipData);
}

} // namespace Qt3DAnimation

QT_END_NAMESPACE

#include "moc_qanimationclip.cpp"
