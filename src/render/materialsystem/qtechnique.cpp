/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
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
**
****************************************************************************/

#include "qtechnique.h"
#include "qtechnique_p.h"
#include "qparameter.h"
#include "qgraphicsapifilter.h"
#include <Qt3DCore/qpropertyupdatedchange.h>
#include <Qt3DCore/qpropertynodeaddedchange.h>
#include <Qt3DCore/qpropertynoderemovedchange.h>

QT_BEGIN_NAMESPACE

using namespace Qt3DCore;

namespace Qt3DRender {

QTechniquePrivate::QTechniquePrivate()
    : QNodePrivate()
{
}

QTechniquePrivate::~QTechniquePrivate()
{
}

QTechnique::QTechnique(QNode *parent)
    : QNode(*new QTechniquePrivate, parent)
{
    Q_D(QTechnique);
    QObject::connect(&d->m_graphicsApiFilter, SIGNAL(graphicsApiFilterChanged()), this, SLOT(_q_graphicsApiFilterChanged()));
}

/*! \internal */
QTechnique::QTechnique(QTechniquePrivate &dd, QNode *parent)
    : QNode(dd, parent)
{
    Q_D(QTechnique);
    QObject::connect(&d->m_graphicsApiFilter, SIGNAL(graphicsApiFilterChanged()), this, SLOT(_q_graphicsApiFilterChanged()));
}

void QTechniquePrivate::_q_graphicsApiFilterChanged()
{
    if (m_changeArbiter != nullptr) {
        QPropertyUpdatedChangePtr change(new QPropertyUpdatedChange(m_id));
        change->setPropertyName("graphicsApiFilterData");
        change->setValue(QVariant::fromValue(QGraphicsApiFilterPrivate::get(const_cast<QGraphicsApiFilter *>(&m_graphicsApiFilter))->m_data));
        notifyObservers(change);
    }
}

void QTechnique::addFilterKey(QFilterKey *filterKey)
{
    Q_ASSERT(filterKey);
    Q_D(QTechnique);
    if (!d->m_filterKeys.contains(filterKey)) {
        d->m_filterKeys.append(filterKey);

        // We need to add it as a child of the current node if it has been declared inline
        // Or not previously added as a child of the current node so that
        // 1) The backend gets notified about it's creation
        // 2) When the current node is destroyed, it gets destroyed as well
        if (!filterKey->parent())
            filterKey->setParent(this);

        if (d->m_changeArbiter != nullptr) {
            const auto change = QPropertyNodeAddedChangePtr::create(id(), filterKey);
            change->setPropertyName("filterKeys");
            d->notifyObservers(change);
        }
    }
}

void QTechnique::removeFilterKey(QFilterKey *filterKey)
{
    Q_ASSERT(filterKey);
    Q_D(QTechnique);
    if (d->m_changeArbiter != nullptr) {
        const auto change = QPropertyNodeRemovedChangePtr::create(id(), filterKey);
        change->setPropertyName("filterKeys");
        d->notifyObservers(change);
    }
    d->m_filterKeys.removeOne(filterKey);
}

QVector<QFilterKey *> QTechnique::filterKeys() const
{
    Q_D(const QTechnique);
    return d->m_filterKeys;
}

void QTechnique::addParameter(QParameter *parameter)
{
    Q_ASSERT(parameter);
    Q_D(QTechnique);
    if (!d->m_parameters.contains(parameter)) {
        d->m_parameters.append(parameter);

        // We need to add it as a child of the current node if it has been declared inline
        // Or not previously added as a child of the current node so that
        // 1) The backend gets notified about it's creation
        // 2) When the current node is destroyed, the child parameters get destroyed as well
        if (!parameter->parent())
            parameter->setParent(this);

        if (d->m_changeArbiter != nullptr) {
            const auto change = QPropertyNodeAddedChangePtr::create(id(), parameter);
            change->setPropertyName("parameter");
            d->notifyObservers(change);
        }
    }
}

void QTechnique::removeParameter(QParameter *parameter)
{
    Q_ASSERT(parameter);
    Q_D(QTechnique);
    if (d->m_changeArbiter != nullptr) {
        const auto change = QPropertyNodeRemovedChangePtr::create(id(), parameter);
        change->setPropertyName("parameter");
        d->notifyObservers(change);
    }
    d->m_parameters.removeOne(parameter);
}

/*!
 * Appends a \a pass to the technique. This posts a ComponentAdded
 * QScenePropertyChange notification to the QChangeArbiter with the
 * value being the \a pass and the property name being "pass".
 */
void QTechnique::addRenderPass(QRenderPass *pass)
{
    Q_ASSERT(pass);
    Q_D(QTechnique);
    if (!d->m_renderPasses.contains(pass)) {
        d->m_renderPasses.append(pass);

        // We need to add it as a child of the current node if it has been declared inline
        // Or not previously added as a child of the current node so that
        // 1) The backend gets notified about it's creation
        // 2) When the current node is destroyed, it gets destroyed as well
        if (!pass->parent())
            pass->setParent(this);

        if (d->m_changeArbiter != nullptr) {
            const auto change = QPropertyNodeAddedChangePtr::create(id(), pass);
            change->setPropertyName("pass");
            d->notifyObservers(change);
        }
    }
}

/*!
 * Removes a \a pass from the technique. This posts a ComponentRemoved
 * QScenePropertyChange notification to the QChangeArbiter with the value
 * being the id of \a pass and the property name being "pass".
 */
void QTechnique::removeRenderPass(QRenderPass *pass)
{
    Q_ASSERT(pass);
    Q_D(QTechnique);
    if (d->m_changeArbiter) {
        const auto change = QPropertyNodeAddedChangePtr::create(id(), pass);
        change->setPropertyName("pass");
        d->notifyObservers(change);
    }
    d->m_renderPasses.removeOne(pass);
}

/*!
 * Returns the list of render passes contained in the technique.
 */
QVector<QRenderPass *> QTechnique::renderPasses() const
{
    Q_D(const QTechnique);
    return d->m_renderPasses;
}

QVector<QParameter *> QTechnique::parameters() const
{
    Q_D(const QTechnique);
    return d->m_parameters;
}

QGraphicsApiFilter *QTechnique::graphicsApiFilter()
{
    Q_D(QTechnique);
    return &d->m_graphicsApiFilter;
}

Qt3DCore::QNodeCreatedChangeBasePtr QTechnique::createNodeCreationChange() const
{
    auto creationChange = Qt3DCore::QNodeCreatedChangePtr<QTechniqueData>::create(this);
    QTechniqueData &data = creationChange->data;

    Q_D(const QTechnique);
    data.graphicsApiFilterData = QGraphicsApiFilterPrivate::get(const_cast<QGraphicsApiFilter *>(&d->m_graphicsApiFilter))->m_data;
    data.filterKeyIds = qIdsForNodes(d->m_filterKeys);
    data.parameterIds = qIdsForNodes(d->m_parameters);
    data.renderPassIds = qIdsForNodes(d->m_renderPasses);

    return creationChange;
}

} // of namespace Qt3DRender

QT_END_NAMESPACE

#include "moc_qtechnique.cpp"
