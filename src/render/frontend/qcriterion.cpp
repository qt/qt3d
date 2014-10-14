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

#include "qcriterion.h"
#include "qcriterion_p.h"
#include <private/qnode_p.h>
#include <Qt3DCore/qscenepropertychange.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {


QCriterionPrivate::QCriterionPrivate(QCriterion *qq)
    : QNodePrivate(qq)
{
}

void QCriterionPrivate::copy(const QNodePrivate *ref)
{
    QNodePrivate::copy(ref);
    const QCriterionPrivate *criterion = static_cast<const QCriterionPrivate *>(ref);
    m_name = criterion->m_name;
    m_value = criterion->m_value;
}

QCriterion::QCriterion(QNode *parent)
    : QNode(*new QCriterionPrivate(this), parent)
{
}


QCriterion *QCriterion::doClone() const
{
    QCriterion *clone = new QCriterion();
    clone->d_func()->copy(d_func());
    return clone;
}

void QCriterion::setValue(const QVariant &value)
{
    Q_D(QCriterion);
    if (value != d->m_value) {
        d->m_value = value;
        emit valueChanged();
        QScenePropertyChangePtr change(new QScenePropertyChange(NodeUpdated, this));
        change->setPropertyName(QByteArrayLiteral("criterionValue"));
        change->setValue(value);
        d->notifyObservers(change);
    }
}

void QCriterion::setName(const QString &name)
{
    Q_D(QCriterion);
    if (name != d->m_name) {
        d->m_name = name;
        emit nameChanged();
        QScenePropertyChangePtr change(new QScenePropertyChange(NodeUpdated, this));
        change->setPropertyName(QByteArrayLiteral("criterionName"));
        change->setValue(QVariant(name));
        d->notifyObservers(change);
    }
}


QVariant QCriterion::value() const
{
    Q_D(const QCriterion);
    return d->m_value;
}

QString QCriterion::name() const
{
    Q_D(const QCriterion);
    return d->m_name;
}

} // Qt3D

QT_END_NAMESPACE
