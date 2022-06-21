/****************************************************************************
**
** Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
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
**
**
**
****************************************************************************/

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

Qt3DCore::QNodeCreatedChangeBasePtr QAnimationClip::createNodeCreationChange() const
{
    auto creationChange = Qt3DCore::QNodeCreatedChangePtr<QAnimationClipChangeData>::create(this);
    auto &data = creationChange->data;
    Q_D(const QAnimationClip);
    data.clipData = d->m_clipData;
    return creationChange;
}

} // namespace Qt3DAnimation

QT_END_NAMESPACE

#include "moc_qanimationclip.cpp"
