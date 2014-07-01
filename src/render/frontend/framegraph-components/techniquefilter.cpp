
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

#include "techniquefilter.h"
#include "techniquefilter_p.h"

#include <Qt3DRenderer/techniquecriterion.h>
#include <Qt3DCore/qscenepropertychange.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

TechniqueFilterPrivate::TechniqueFilterPrivate(TechniqueFilter *qq)
    : FrameGraphItemPrivate(qq)
{
}

TechniqueFilter::TechniqueFilter(Node *parent)
    : FrameGraphItem(*new TechniqueFilterPrivate(this), parent)
{
}

TechniqueFilter::TechniqueFilter(TechniqueFilterPrivate &dd, Node *parent)
    : FrameGraphItem(dd, parent)
{
}

QList<TechniqueCriterion *> TechniqueFilter::criteria() const
{
    Q_D(const TechniqueFilter);
    return d->m_criteriaList;
}

void TechniqueFilter::addCriterion(TechniqueCriterion *criterion)
{
    Q_D(TechniqueFilter);
    d->m_criteriaList.append(criterion);
    emit criteriaChanged();
    QScenePropertyChangePtr propertyChange(new QScenePropertyChange(ComponentAdded, this));
    propertyChange->m_propertyName = QByteArrayLiteral("techniqueCriteria");
    propertyChange->m_value = QVariant::fromValue(criterion);
    notifyObservers(propertyChange);
}

void TechniqueFilter::removeCriterion(TechniqueCriterion *criterion)
{
    Q_D(TechniqueFilter);
    d->m_criteriaList.removeOne(criterion);
    emit criteriaChanged();
    QScenePropertyChangePtr propertyChange(new QScenePropertyChange(ComponentRemoved, this));
    propertyChange->m_propertyName = QByteArrayLiteral("techniqueCriteria");
    propertyChange->m_value = QVariant::fromValue(criterion);
    notifyObservers(propertyChange);
}

} // Qt3D

QT_END_NAMESPACE
