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

#include "qtechniquecriterion.h"
#include <private/qnode_p.h>
#include <Qt3DCore/qscenepropertychange.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

class QTechniqueCriterionPrivate : public QNodePrivate
{
public:
    QTechniqueCriterionPrivate(QTechniqueCriterion *qq)
        : QNodePrivate(qq)
        , m_criterionType(QTechniqueCriterion::CustomType)
    {
    }

    Q_DECLARE_PUBLIC(QTechniqueCriterion)
    QTechniqueCriterion::CriterionType m_criterionType;
    QString m_criterionCustomType;
    QVariant m_criterionValue;
};

QTechniqueCriterion::QTechniqueCriterion(QNode *parent)
    : QNode(*new QTechniqueCriterionPrivate(this), parent)
{
}

void QTechniqueCriterion::setCriterionType(QTechniqueCriterion::CriterionType type)
{
    Q_D(QTechniqueCriterion);
    if (type != d->m_criterionType) {
        d->m_criterionType = type;
        emit criterionTypeChanged();
        QScenePropertyChangePtr change(new QScenePropertyChange(ComponentAdded, this));
        change->setPropertyName(QByteArrayLiteral("criterionType"));
        change->setValue(QVariant::fromValue(static_cast<int>(type)));
        notifyObservers(change);
    }
}

void QTechniqueCriterion::setCriterionValue(const QVariant &value)
{
    Q_D(QTechniqueCriterion);
    if (value != d->m_criterionValue) {
        d->m_criterionValue = value;
        emit criterionValueChanged();
        QScenePropertyChangePtr change(new QScenePropertyChange(ComponentAdded, this));
        change->setPropertyName(QByteArrayLiteral("criterionValue"));
        change->setValue(value);
        notifyObservers(change);
    }
}

void QTechniqueCriterion::setCriterionCustomType(const QString &customType)
{
    Q_D(QTechniqueCriterion);
    if (customType != d->m_criterionCustomType) {
        d->m_criterionCustomType = customType;
        emit criterionCustomTypeChanged();
        QScenePropertyChangePtr change(new QScenePropertyChange(ComponentAdded, this));
        change->setPropertyName(QByteArrayLiteral("criterionCustomType"));
        change->setValue(QVariant(customType));
        notifyObservers(change);
    }
}

QTechniqueCriterion::CriterionType QTechniqueCriterion::criterionType() const
{
    Q_D(const QTechniqueCriterion);
    return d->m_criterionType;
}

QVariant QTechniqueCriterion::criterionValue() const
{
    Q_D(const QTechniqueCriterion);
    return d->m_criterionValue;
}

QString QTechniqueCriterion::criterionCustomType() const
{
    Q_D(const QTechniqueCriterion);
    return d->m_criterionCustomType;
}

} // Qt3D

QT_END_NAMESPACE
