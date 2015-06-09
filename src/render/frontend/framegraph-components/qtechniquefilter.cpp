
/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qtechniquefilter.h"
#include "qtechniquefilter_p.h"
#include <Qt3DRenderer/qannotation.h>
#include <Qt3DRenderer/qparameter.h>
#include <Qt3DCore/qscenepropertychange.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

/*!
    \class Qt3D::QTechniqueFilterPrivate
    \internal
*/
QTechniqueFilterPrivate::QTechniqueFilterPrivate()
    : QFrameGraphNodePrivate()
{
}

void QTechniqueFilter::copy(const QNode *ref)
{
    QFrameGraphNode::copy(ref);
    const QTechniqueFilter *other = static_cast<const QTechniqueFilter*>(ref);

    Q_FOREACH (QAnnotation *crit, other->d_func()->m_requireList)
        addRequirement(qobject_cast<QAnnotation *>(QNode::clone(crit)));
}

QTechniqueFilter::QTechniqueFilter(QNode *parent)
    : QFrameGraphNode(*new QTechniqueFilterPrivate, parent)
{
}

QTechniqueFilter::~QTechniqueFilter()
{
    QNode::cleanup();
}

/*! \internal */
QTechniqueFilter::QTechniqueFilter(QTechniqueFilterPrivate &dd, QNode *parent)
    : QFrameGraphNode(dd, parent)
{
}

QList<QAnnotation *> QTechniqueFilter::criteria() const
{
    Q_D(const QTechniqueFilter);
    return d->m_requireList;
}

void QTechniqueFilter::addRequirement(QAnnotation *criterion)
{
    Q_D(QTechniqueFilter);
    d->m_requireList.append(criterion);

    // We need to add it as a child of the current node if it has been declared inline
    // Or not previously added as a child of the current node so that
    // 1) The backend gets notified about it's creation
    // 2) When the current node is destroyed, it gets destroyed as well
    if (!criterion->parent())
        criterion->setParent(this);

    if (d->m_changeArbiter != Q_NULLPTR) {
        QScenePropertyChangePtr propertyChange(new QScenePropertyChange(NodeAdded, QSceneChange::Node, id()));
        propertyChange->setPropertyName("require");
        propertyChange->setValue(QVariant::fromValue(criterion));
        d->notifyObservers(propertyChange);
    }
}

void QTechniqueFilter::removeRequirement(QAnnotation *criterion)
{
    Q_D(QTechniqueFilter);
    if (d->m_changeArbiter != Q_NULLPTR) {
        QScenePropertyChangePtr propertyChange(new QScenePropertyChange(NodeRemoved, QSceneChange::Node, id()));
        propertyChange->setPropertyName("require");
        propertyChange->setValue(QVariant::fromValue(criterion->id()));
        d->notifyObservers(propertyChange);
    }
    d->m_requireList.removeOne(criterion);
}

void QTechniqueFilter::addParameter(QParameter *parameter)
{
    Q_D(QTechniqueFilter);
    if (!d->m_parameters.contains(parameter)) {
        d->m_parameters.append(parameter);

        // We need to add it as a child of the current node if it has been declared inline
        // Or not previously added as a child of the current node so that
        // 1) The backend gets notified about it's creation
        // 2) When the current node is destroyed, the child parameters get destroyed as well
        if (!parameter->parent())
            parameter->setParent(this);

        if (d->m_changeArbiter != Q_NULLPTR) {
            QScenePropertyChangePtr change(new QScenePropertyChange(NodeAdded, QSceneChange::Node, id()));
            change->setPropertyName("parameter");
            change->setValue(QVariant::fromValue(parameter->id()));
            d->notifyObservers(change);
        }
    }
}

void QTechniqueFilter::removeParameter(QParameter *parameter)
{
    Q_D(QTechniqueFilter);

    if (d->m_changeArbiter != Q_NULLPTR) {
        QScenePropertyChangePtr change(new QScenePropertyChange(NodeRemoved, QSceneChange::Node, id()));
        change->setPropertyName("parameter");
        change->setValue(QVariant::fromValue(parameter->id()));
        d->notifyObservers(change);
    }
    d->m_parameters.removeOne(parameter);
}

QList<QParameter *> QTechniqueFilter::parameters() const
{
    Q_D(const QTechniqueFilter);
    return d->m_parameters;
}

} // Qt3D

QT_END_NAMESPACE
