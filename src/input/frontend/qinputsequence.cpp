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
****************************************************************************/

#include "qinputsequence.h"
#include <Qt3DCore/private/qnode_p.h>
#include <Qt3DInput/qabstractphysicaldevice.h>
#include <Qt3DInput/QAbstractAggregateActionInput>
#include <Qt3DInput/private/qabstractaggregateactioninput_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {

class QInputSequencePrivate : public Qt3DInput::QAbstractAggregateActionInputPrivate
{
public:
    QInputSequencePrivate()
        : Qt3DInput::QAbstractAggregateActionInputPrivate()
        , m_timeout(0)
        , m_interval(0)
        , m_sequential(true)
    {}

    int m_timeout;
    int m_interval;
    bool m_sequential;
};

QInputSequence::QInputSequence(Qt3DCore::QNode *parent)
    : Qt3DInput::QAbstractAggregateActionInput(*new QInputSequencePrivate(), parent)
{

}

QInputSequence::~QInputSequence()
{
    QNode::cleanup();
}

int QInputSequence::timeout() const
{
    Q_D(const QInputSequence);
    return d->m_timeout;
}

int QInputSequence::interval() const
{
    Q_D(const QInputSequence);
    return d->m_interval;
}

bool QInputSequence::sequential() const
{
    Q_D(const QInputSequence);
    return d->m_sequential;
}

void QInputSequence::setTimeout(int timeout)
{
    Q_D(QInputSequence);
    if (d->m_timeout != timeout) {
        d->m_timeout = timeout;
        emit timeoutChanged(timeout);
    }
}

void QInputSequence::setInterval(int interval)
{
    Q_D(QInputSequence);
    if (d->m_interval != interval) {
        d->m_interval = interval;
        emit intervalChanged(interval);
    }
}

void QInputSequence::setSequential(bool sequential)
{
    Q_D(QInputSequence);
    if (d->m_sequential != sequential) {
        d->m_sequential = sequential;
        emit sequentialChanged(sequential);
    }
}

void QInputSequence::copy(const Qt3DCore::QNode *ref)
{
    QAbstractAggregateActionInput::copy(ref);
    const QInputSequence *input = static_cast<const QInputSequence *>(ref);
    d_func()->m_timeout = input->d_func()->m_timeout;
    d_func()->m_interval = input->d_func()->m_interval;
    d_func()->m_sequential = input->d_func()->m_sequential;
}

} // Qt3DInput

QT_END_NAMESPACE
