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

#include "qanimationclip.h"
#include "qanimationclip_p.h"
#include <Qt3DCore/qpropertyupdatedchange.h>

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {

QAnimationClipPrivate::QAnimationClipPrivate()
    : Qt3DCore::QNodePrivate()
    , m_duration(0.0f)
{
}

void QAnimationClipPrivate::setDuration(float duration)
{
    if (qFuzzyCompare(duration, m_duration))
        return;

    Q_Q(QAnimationClip);
    bool wasBlocked = q->blockNotifications(true);
    m_duration = duration;
    emit q->durationChanged(duration);
    q->blockNotifications(wasBlocked);
}

QAnimationClip::QAnimationClip(Qt3DCore::QNode *parent)
    : Qt3DCore::QNode(*new QAnimationClipPrivate, parent)
{
}

QAnimationClip::QAnimationClip(QAnimationClipPrivate &dd, Qt3DCore::QNode *parent)
    : Qt3DCore::QNode(dd, parent)
{
}

QAnimationClip::~QAnimationClip()
{
}

QUrl QAnimationClip::source() const
{
    Q_D(const QAnimationClip);
    return d->m_source;
}

float QAnimationClip::duration() const
{
    Q_D(const QAnimationClip);
    return d->m_duration;
}

void QAnimationClip::setSource(QUrl source)
{
    Q_D(QAnimationClip);
    if (d->m_source == source)
        return;

    d->m_source = source;
    emit sourceChanged(source);
}

void QAnimationClip::sceneChangeEvent(const Qt3DCore::QSceneChangePtr &change)
{
    Q_D(QAnimationClip);
    if (change->type() == Qt3DCore::PropertyUpdated) {
        Qt3DCore::QPropertyUpdatedChangePtr e = qSharedPointerCast<Qt3DCore::QPropertyUpdatedChange>(change);
        if (e->propertyName() == QByteArrayLiteral("duration"))
            d->setDuration(e->value().toFloat());
    }
}

Qt3DCore::QNodeCreatedChangeBasePtr QAnimationClip::createNodeCreationChange() const
{
    auto creationChange = Qt3DCore::QNodeCreatedChangePtr<QAnimationClipData>::create(this);
    auto &data = creationChange->data;
    Q_D(const QAnimationClip);
    data.source = d->m_source;
    return creationChange;
}

} // namespace Qt3DAnimation

QT_END_NAMESPACE
