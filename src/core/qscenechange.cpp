/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qscenechange.h"
#include "qscenechange_p.h"
#include <QDateTime>

QT_BEGIN_NAMESPACE

namespace Qt3D {

QSceneChangePrivate::QSceneChangePrivate(QSceneChange *qq)
    : q_ptr(qq)
{
}

QSceneChange::QSceneChange(ChangeFlag type, QObservableInterface *observable, QSceneChange::Priority priority)
    : d_ptr(new QSceneChangePrivate(this))
{
    Q_D(QSceneChange);
    d->m_type = type;
    d->m_priority = priority;
    d->m_timestamp = QDateTime::currentMSecsSinceEpoch();
    d->m_subject.m_observable = observable;
    d->m_subjectType = Observable;
}

QSceneChange::QSceneChange(ChangeFlag type, QNode *node, QSceneChange::Priority priority)
    : d_ptr(new QSceneChangePrivate(this))
{
    Q_D(QSceneChange);
    d->m_type = type;
    d->m_priority = priority;
    d->m_timestamp = QDateTime::currentMSecsSinceEpoch();
    d->m_subject.m_node = node;
    d->m_subjectType = Node;
}

QSceneChange::~QSceneChange()
{
    delete d_ptr;
}

QSceneChange::QSceneChange(QSceneChangePrivate &dd)
    : d_ptr(&dd)
{
}

QSceneChange::QSceneChange(QSceneChangePrivate &dd, ChangeFlag type, QObservableInterface *observable, QSceneChange::Priority priority)
    : d_ptr(&dd)
{
    Q_D(QSceneChange);
    d->m_type = type;
    d->m_priority = priority;
    d->m_timestamp = QDateTime::currentMSecsSinceEpoch();
    d->m_subject.m_observable = observable;
    d->m_subjectType = Observable;
}

QSceneChange::QSceneChange(QSceneChangePrivate &dd, ChangeFlag type, QNode *node, QSceneChange::Priority priority)
    : d_ptr(&dd)
{
    Q_D(QSceneChange);
    d->m_type = type;
    d->m_priority = priority;
    d->m_timestamp = QDateTime::currentMSecsSinceEpoch();
    d->m_subject.m_node = node;
    d->m_subjectType = Node;
}

ChangeFlag QSceneChange::type() const
{
    Q_D(const QSceneChange);
    return d->m_type;
}

QSceneChange::Priority QSceneChange::priority() const
{
    Q_D(const QSceneChange);
    return d->m_priority;
}

qint64 QSceneChange::timestamp() const
{
    Q_D(const QSceneChange);
    return d->m_timestamp;
}

QSceneChange::ObservableType QSceneChange::observableType() const
{
    Q_D(const QSceneChange);
    return d->m_subjectType;
}

QSceneChange::SubjectUnion QSceneChange::subject() const
{
    Q_D(const QSceneChange);
    return d->m_subject;
}

} // Qt3D

QT_END_NAMESPACE
