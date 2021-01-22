/****************************************************************************
**
** Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
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

#include "qanimationclipdata.h"

#include <QtCore/qvector.h>

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

bool QAnimationClipData::isValid() const Q_DECL_NOTHROW
{
    // TODO: Perform more thorough checks
    return !d->m_channels.isEmpty();
}

QAnimationClipData::const_iterator QAnimationClipData::begin() const Q_DECL_NOTHROW
{
    return d->m_channels.cbegin();
}

QAnimationClipData::const_iterator QAnimationClipData::end() const Q_DECL_NOTHROW
{
    return d->m_channels.cend();
}


bool operator==(const QAnimationClipData &lhs, const QAnimationClipData &rhs) Q_DECL_NOTHROW
{
    return lhs.d->m_name == rhs.d->m_name &&
           lhs.d->m_channels == rhs.d->m_channels;
}

bool operator!=(const QAnimationClipData &lhs, const QAnimationClipData &rhs) Q_DECL_NOTHROW
{
    return lhs.d->m_name != rhs.d->m_name ||
           lhs.d->m_channels != rhs.d->m_channels;
}

} // namespace Qt3DAnimation

QT_END_NAMESPACE
