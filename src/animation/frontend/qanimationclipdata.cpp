// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qanimationclipdata.h"

#include <QtCore/qlist.h>

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {

class QAnimationClipDataPrivate
{
public:
    QVector<QChannel> m_channels;
    QString m_name;
};

/*!
    \class Qt3DAnimation::QAnimationClipData
    \inmodule Qt3DAnimation
    \brief Class containing the animation data.
*/
QAnimationClipData::QAnimationClipData()
    : d(new QAnimationClipDataPrivate)
{
}

QAnimationClipData::QAnimationClipData(const QAnimationClipData &rhs)
    : d(new QAnimationClipDataPrivate)
{
    *d = *(rhs.d);
}

QAnimationClipData &QAnimationClipData::operator=(const QAnimationClipData &rhs)
{
    if (this != &rhs)
        *d = *(rhs.d);
    return *this;
}

QAnimationClipData::~QAnimationClipData()
{
}

void QAnimationClipData::setName(const QString &name)
{
    d->m_name = name;
}

QString QAnimationClipData::name() const
{
    return d->m_name;
}

int QAnimationClipData::channelCount() const
{
    return d->m_channels.size();
}

void QAnimationClipData::appendChannel(const QChannel &c)
{
    d->m_channels.append(c);
}

void QAnimationClipData::insertChannel(int index, const QChannel &c)
{
    d->m_channels.insert(index, c);
}

void QAnimationClipData::removeChannel(int index)
{
    d->m_channels.remove(index);
}

void QAnimationClipData::clearChannels()
{
    d->m_channels.clear();
}

bool QAnimationClipData::isValid() const noexcept
{
    // TODO: Perform more thorough checks
    return !d->m_channels.isEmpty();
}

QAnimationClipData::const_iterator QAnimationClipData::begin() const noexcept
{
    return d->m_channels.cbegin().operator->();
}

QAnimationClipData::const_iterator QAnimationClipData::end() const noexcept
{
    return d->m_channels.cend().operator->();
}


bool operator==(const QAnimationClipData &lhs, const QAnimationClipData &rhs) noexcept
{
    return lhs.d->m_name == rhs.d->m_name &&
           lhs.d->m_channels == rhs.d->m_channels;
}

bool operator!=(const QAnimationClipData &lhs, const QAnimationClipData &rhs) noexcept
{
    return lhs.d->m_name != rhs.d->m_name ||
           lhs.d->m_channels != rhs.d->m_channels;
}

} // namespace Qt3DAnimation

QT_END_NAMESPACE
