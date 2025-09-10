// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qchannelcomponent.h"

#include <QtCore/qlist.h>

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {

class QChannelComponentPrivate
{
public:
    QVector<QKeyFrame> m_keyFrames;
    QString m_name;
};

QChannelComponent::QChannelComponent()
    : d(new QChannelComponentPrivate)
{
}

QChannelComponent::QChannelComponent(const QString &name)
    : d(new QChannelComponentPrivate)
{
    d->m_name = name;
}

QChannelComponent::QChannelComponent(const QChannelComponent &rhs)
    : d(new QChannelComponentPrivate)
{
    *d = *(rhs.d);
}

QChannelComponent &QChannelComponent::operator=(const QChannelComponent &rhs)
{
    if (this != &rhs)
        *d = *(rhs.d);
    return *this;
}

QChannelComponent::~QChannelComponent()
{
}

void QChannelComponent::setName(const QString &name)
{
    d->m_name = name;
}

QString QChannelComponent::name() const
{
    return d->m_name;
}

int QChannelComponent::keyFrameCount() const
{
    return d->m_keyFrames.size();
}

void QChannelComponent::appendKeyFrame(const QKeyFrame &kf)
{
    d->m_keyFrames.append(kf);
}

void QChannelComponent::insertKeyFrame(int index, const QKeyFrame &kf)
{
    d->m_keyFrames.insert(index, kf);
}

void QChannelComponent::removeKeyFrame(int index)
{
    d->m_keyFrames.remove(index);
}

void QChannelComponent::clearKeyFrames()
{
    d->m_keyFrames.clear();
}

QChannelComponent::const_iterator QChannelComponent::begin() const noexcept
{
    return d->m_keyFrames.cbegin().operator->();
}

QChannelComponent::const_iterator QChannelComponent::end() const noexcept
{
    return d->m_keyFrames.cend().operator->();
}

bool operator==(const QChannelComponent &lhs, const QChannelComponent &rhs) noexcept
{
    return lhs.d->m_name == rhs.d->m_name &&
           lhs.d->m_keyFrames == rhs.d->m_keyFrames;
}

bool operator!=(const QChannelComponent &lhs, const QChannelComponent &rhs) noexcept
{
    return lhs.d->m_name != rhs.d->m_name ||
           lhs.d->m_keyFrames != rhs.d->m_keyFrames;
}

} // namespace Qt3DAnimation

QT_END_NAMESPACE
