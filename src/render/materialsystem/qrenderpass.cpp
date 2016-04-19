/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Copyright (C) 2016 The Qt Company Ltd and/or its subsidiary(-ies).
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

#include "qrenderpass.h"
#include "qrenderpass_p.h"
#include "qparameter.h"
#include "qfilterkey.h"
#include "qrenderstate.h"
#include <Qt3DCore/qnodepropertychange.h>
#include <Qt3DCore/qnodeaddedpropertychange.h>
#include <Qt3DCore/qnoderemovedpropertychange.h>
#include <Qt3DCore/private/qnode_p.h>

QT_BEGIN_NAMESPACE

using namespace Qt3DCore;

namespace Qt3DRender {

QRenderPassPrivate::QRenderPassPrivate()
    : QNodePrivate()
    , m_shader(Q_NULLPTR)
{
}

QRenderPass::QRenderPass(QNode *parent)
    : QNode(*new QRenderPassPrivate, parent)
{
}

/*! \internal */
QRenderPass::QRenderPass(QRenderPassPrivate &dd, QNode *parent)
    : QNode(dd, parent)
{
}

/*!
 * Sets the pass's \a shaderProgram. This posts a ComponentUpdated
 * QScenePropertyChange to the QChangeArbiter. The value is set to
 * the \a shaderProgram and the property name to \c {"shaderProgram"}.
 */
void QRenderPass::setShaderProgram(QShaderProgram *shaderProgram)
{
    Q_D(QRenderPass);
    if (d->m_shader != shaderProgram) {

        if (d->m_shader != Q_NULLPTR && d->m_changeArbiter != Q_NULLPTR) {
            const auto change = QNodeRemovedPropertyChangePtr::create(id(), d->m_shader->id());
            change->setPropertyName("shaderProgram");
            d->notifyObservers(change);
        }

        d->m_shader = shaderProgram;
        emit shaderProgramChanged(shaderProgram);

        // We need to add it as a child of the current node if it has been declared inline
        // Or not previously added as a child of the current node so that
        // 1) The backend gets notified about it's creation
        // 2) When the current node is destroyed, it gets destroyed as well
        if (!shaderProgram->parent())
            shaderProgram->setParent(this);

        if (d->m_shader && d->m_changeArbiter != Q_NULLPTR) {
            QNodePropertyChangePtr e(new QNodePropertyChange(NodeAdded, QSceneChange::Node, id()));
            const auto change = QNodeAddedPropertyChangePtr::create(id(), d->m_shader->id());
            change->setPropertyName("shaderProgram");
            d->notifyObservers(change);
        }
    }
}

QShaderProgram *QRenderPass::shaderProgram() const
{
    Q_D(const QRenderPass);
    return d->m_shader;
}

void QRenderPass::addFilterKey(QFilterKey *filterKey)
{
    Q_ASSERT(filterKey);
    Q_D(QRenderPass);
    if (!d->m_filterKeyList.contains(filterKey)) {
        d->m_filterKeyList.append(filterKey);

        // We need to add it as a child of the current node if it has been declared inline
        // Or not previously added as a child of the current node so that
        // 1) The backend gets notified about it's creation
        // 2) When the current node is destroyed, it gets destroyed as well
        if (!filterKey->parent())
            filterKey->setParent(this);

        if (d->m_changeArbiter != Q_NULLPTR) {
            const auto change = QNodeAddedPropertyChangePtr::create(id(), filterKey->id());
            change->setPropertyName("filterKeys");
            d->notifyObservers(change);
        }
    }
}

void QRenderPass::removeFilterKey(QFilterKey *filterKey)
{
    Q_ASSERT(filterKey);
    Q_D(QRenderPass);
    if (d->m_changeArbiter != Q_NULLPTR) {
        const auto change = QNodeRemovedPropertyChangePtr::create(id(), filterKey->id());
        change->setPropertyName("filterKeys");
        d->notifyObservers(change);
    }
    d->m_filterKeyList.removeOne(filterKey);
}

QVector<QFilterKey *> QRenderPass::filterKeys() const
{
    Q_D(const QRenderPass);
    return d->m_filterKeyList;
}

/*!
 * Adds a Qt3DCore::QRenderState \a state to the rendering pass. That implies that
 * when the pass is executed at render time, the globally set render state will
 * be modifed by the states defined locally by the Qt3DRender::QRenderPass.
 *
 * \note not defining any Qt3DCore::QRenderState in a pass will result in the pass using
 * the globally set render state for a given FrameGraph branch execution path.
 */
void QRenderPass::addRenderState(QRenderState *state)
{
    Q_ASSERT(state);
    Q_D(QRenderPass);
    if (!d->m_renderStates.contains(state)) {
        d->m_renderStates.append(state);

        if (!state->parent())
            state->setParent(this);

        if (d->m_changeArbiter != Q_NULLPTR) {
            const auto change = QNodeAddedPropertyChangePtr::create(id(), state->id());
            change->setPropertyName("renderState");
            d->notifyObservers(change);
        }
    }
}

/*!
 * Removes \a state from the Qt3DRender::QRenderPass local render state.
 */
void QRenderPass::removeRenderState(QRenderState *state)
{
    Q_ASSERT(state);
    Q_D(QRenderPass);
    if (d->m_changeArbiter != Q_NULLPTR) {
        const auto change = QNodeRemovedPropertyChangePtr::create(id(), state->id());
        change->setPropertyName("renderState");
        d->notifyObservers(change);
    }
    d->m_renderStates.removeOne(state);
}

/*!
 * Returns the list of Qt3DCore::QRenderState state objects making up the render
 * state of the Qt3DRender::QRenderPass.
 */
QVector<QRenderState *> QRenderPass::renderStates() const
{
    Q_D(const QRenderPass);
    return d->m_renderStates;
}

void QRenderPass::addParameter(QParameter *parameter)
{
    Q_ASSERT(parameter);
    Q_D(QRenderPass);
    if (!d->m_parameters.contains(parameter)) {
        d->m_parameters.append(parameter);

        // We need to add it as a child of the current node if it has been declared inline
        // Or not previously added as a child of the current node so that
        // 1) The backend gets notified about it's creation
        // 2) When the current node is destroyed, the child parameters get destroyed as well
        if (!parameter->parent())
            parameter->setParent(this);

        if (d->m_changeArbiter != Q_NULLPTR) {
            const auto change = QNodeAddedPropertyChangePtr::create(id(), parameter->id());
            change->setPropertyName("parameter");
            d->notifyObservers(change);
        }
    }
}

void QRenderPass::removeParameter(QParameter *parameter)
{
    Q_ASSERT(parameter);
    Q_D(QRenderPass);
    if (d->m_changeArbiter != Q_NULLPTR) {
        const auto change = QNodeAddedPropertyChangePtr::create(id(), parameter->id());
        change->setPropertyName("parameter");
        d->notifyObservers(change);
    }
    d->m_parameters.removeOne(parameter);
}


QVector<QParameter *> QRenderPass::parameters() const
{
    Q_D(const QRenderPass);
    return d->m_parameters;
}

Qt3DCore::QNodeCreatedChangeBasePtr QRenderPass::createNodeCreationChange() const
{
    auto creationChange = Qt3DCore::QNodeCreatedChangePtr<QRenderPassData>::create(this);
    auto &data = creationChange->data;
    Q_D(const QRenderPass);
    data.filterKeyIds = qIdsForNodes(d->m_filterKeyList);
    data.parameterIds = qIdsForNodes(d->m_parameters);
    data.renderStateIds = qIdsForNodes(d->m_renderStates);
    data.shaderId = qIdForNode(d->m_shader);
    return creationChange;
}

} // namespace Qt3DRender

QT_END_NAMESPACE
