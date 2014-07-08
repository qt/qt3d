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

#include "qtechnique.h"
#include "qtechnique_p.h"
#include "qparameter.h"
#include <Qt3DCore/qscenepropertychange.h>
#include <QDebug>

QT_BEGIN_NAMESPACE

namespace Qt3D {

QTechniquePrivate::QTechniquePrivate(QTechnique *qq)
    : QAbstractTechniquePrivate(qq)
{
}

QTechnique::QTechnique(QNode *parent)
    : QAbstractTechnique(*new QTechniquePrivate(this), parent)
{
}

QTechnique::QTechnique(QTechniquePrivate &dd, QNode *parent)
    : QAbstractTechnique(dd, parent)
{
}

void QTechnique::addCriterion(TechniqueCriterion *criterion)
{
    Q_D(QTechnique);
    if (!d->m_criteriaList.contains(criterion))
        d->m_criteriaList.append(criterion);
}

void QTechnique::removeCriterion(TechniqueCriterion *criterion)
{
    Q_D(QTechnique);
    d->m_criteriaList.removeOne(criterion);
}

QVariant QTechnique::criterionValue(const QString &customTypeName) const
{
    Q_D(const QTechnique);
    Q_FOREACH (TechniqueCriterion *criterion, d->m_criteriaList)
        if (criterion->criterionCustomType() == customTypeName)
            return criterion->criterionValue();
    return QVariant();
}

QVariant QTechnique::criterionValue(TechniqueCriterion::CriterionType type) const
{
    Q_D(const QTechnique);
    Q_FOREACH (TechniqueCriterion *criterion, d->m_criteriaList)
        if (criterion->criterionType() == type)
            return criterion->criterionValue();
    return QVariant();
}

QList<TechniqueCriterion *> QTechnique::criteria() const
{
    Q_D(const QTechnique);
    return d->m_criteriaList;
}

void QTechnique::clearCriteria()
{
    Q_D(QTechnique);
    d->m_criteriaList.clear();
}

bool QTechnique::containsCriterion(const QString &customTypeName) const
{
    Q_D(const QTechnique);
    Q_FOREACH (TechniqueCriterion *criterion, d->m_criteriaList)
        if (criterion->criterionCustomType() == customTypeName)
            return true;
    return false;
}

bool QTechnique::containsCriterion(TechniqueCriterion::CriterionType type) const
{
    Q_D(const QTechnique);
    Q_FOREACH (TechniqueCriterion *criterion, d->m_criteriaList)
        if (criterion->criterionType() == type)
            return true;
    return false;
}

void QTechnique::addPass(QAbstractRenderPass *pass)
{
    Q_CHECK_PTR(pass);
    pass->setParent(this);
    QAbstractTechnique::addPass(pass);
}

void QTechnique::addParameter(QParameter *parameter)
{
    Q_D(QTechnique);
    if (!d->m_parameters.contains(parameter)) {
        d->m_parameters.append(parameter);
        QScenePropertyChangePtr change(new QScenePropertyChange(ComponentAdded, this));
        change->setPropertyName(QByteArrayLiteral("parameter"));
        change->setValue(QVariant::fromValue(parameter));
        notifyObservers(change);
    }
}

void QTechnique::removeParameter(QParameter *parameter)
{
    Q_D(QTechnique);
    d->m_parameters.removeOne(parameter);
    QScenePropertyChangePtr change(new QScenePropertyChange(ComponentRemoved, this));
    change->setPropertyName(QByteArrayLiteral("parameter"));
    change->setValue(QVariant::fromValue(parameter));
    notifyObservers(change);
}

QList<QParameter *> QTechnique::parameters() const
{
    Q_D(const QTechnique);
    return d->m_parameters;
}

QParameter *QTechnique::parameterByName(QString name) const
{
    Q_D(const QTechnique);
    foreach (QParameter* p, d->m_parameters) {
        if (p->name() == name)
            return p;
    }
    return NULL;
}

} // of namespace Qt3D

QT_END_NAMESPACE
