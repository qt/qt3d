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

#include "technique.h"
#include "parameter.h"
#include <Qt3DCore/qscenepropertychange.h>
#include <QDebug>

QT_BEGIN_NAMESPACE

namespace Qt3D {

class TechniquePrivate
{
public :
    TechniquePrivate(Technique *qq)
        : q_ptr(qq)
    {}

    QList<TechniqueCriterion *> m_criteriaList;
    QList<Parameter *> m_parameters;
    Q_DECLARE_PUBLIC(Technique)
    Technique *q_ptr;
};

Technique::Technique(Node *parent)
    : QAbstractTechnique(parent)
    , d_ptr(new TechniquePrivate(this))
{
}

void Technique::addCriterion(TechniqueCriterion *criterion)
{
    Q_D(Technique);
    if (!d->m_criteriaList.contains(criterion))
        d->m_criteriaList.append(criterion);
}

void Technique::removeCriterion(TechniqueCriterion *criterion)
{
    Q_D(Technique);
    d->m_criteriaList.removeOne(criterion);
}

QVariant Technique::criterionValue(const QString &customTypeName) const
{
    Q_D(const Technique);
    Q_FOREACH (TechniqueCriterion *criterion, d->m_criteriaList)
        if (criterion->criterionCustomType() == customTypeName)
            return criterion->criterionValue();
    return QVariant();
}

QVariant Technique::criterionValue(TechniqueCriterion::CriterionType type) const
{
    Q_D(const Technique);
    Q_FOREACH (TechniqueCriterion *criterion, d->m_criteriaList)
        if (criterion->criterionType() == type)
            return criterion->criterionValue();
    return QVariant();
}

QList<TechniqueCriterion *> Technique::criteria() const
{
    Q_D(const Technique);
    return d->m_criteriaList;
}

void Technique::clearCriteria()
{
    Q_D(Technique);
    d->m_criteriaList.clear();
}

bool Technique::containsCriterion(const QString &customTypeName) const
{
    Q_D(const Technique);
    Q_FOREACH (TechniqueCriterion *criterion, d->m_criteriaList)
        if (criterion->criterionCustomType() == customTypeName)
            return true;
    return false;
}

bool Technique::containsCriterion(TechniqueCriterion::CriterionType type) const
{
    Q_D(const Technique);
    Q_FOREACH (TechniqueCriterion *criterion, d->m_criteriaList)
        if (criterion->criterionType() == type)
            return true;
    return false;
}

void Technique::addPass(QAbstractRenderPass *pass)
{
    Q_CHECK_PTR(pass);
    pass->setParent(this);
    QAbstractTechnique::addPass(pass);
}

void Technique::addParameter(Parameter *parameter)
{
    Q_D(Technique);
    if (!d->m_parameters.contains(parameter)) {
        d->m_parameters.append(parameter);
        QScenePropertyChangePtr change(new QScenePropertyChange(ComponentAdded, this));
        change->m_propertyName = QByteArrayLiteral("parameter");
        change->m_value = QVariant::fromValue(parameter);
        notifyObservers(change);
    }
}

void Technique::removeParameter(Parameter *parameter)
{
    Q_D(Technique);
    d->m_parameters.removeOne(parameter);
    QScenePropertyChangePtr change(new QScenePropertyChange(ComponentRemoved, this));
    change->m_propertyName = QByteArrayLiteral("parameter");
    change->m_value = QVariant::fromValue(parameter);
    notifyObservers(change);
}

QList<Parameter *> Technique::parameters() const
{
    Q_D(const Technique);
    return d->m_parameters;
}

Parameter *Technique::parameterByName(QString name) const
{
    Q_D(const Technique);
    foreach (Parameter* p, d->m_parameters) {
        if (p->name() == name)
            return p;
    }
    return NULL;
}

} // of namespace Qt3D

QT_END_NAMESPACE
