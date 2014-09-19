/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Copyright (C) 2014 Digia Plc and/or its subsidiary(-ies).
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

#include "qrenderpass.h"
#include "qrenderpass_p.h"
#include "qparameter.h"
#include "qcriterion.h"
#include "qparametermapper.h"
#include "qscenepropertychange.h"
#include "qrenderstate.h"

QT_BEGIN_NAMESPACE

namespace Qt3D {

QRenderPassPrivate::QRenderPassPrivate(QRenderPass *qq)
    : QAbstractRenderPassPrivate(qq)
{
}

QRenderPass::QRenderPass(QNode *parent)
    : QAbstractRenderPass(*new QRenderPassPrivate(this), parent)
{
}

QRenderPass::QRenderPass(QRenderPassPrivate &dd, QNode *parent)
    : QAbstractRenderPass(dd, parent)
{
}

QRenderPass *QRenderPass::doClone(bool isClone) const
{
    Q_D(const QRenderPass);
    QRenderPass *pass = new QRenderPass();

    pass->copy(this);
    pass->d_func()->m_isClone = isClone;

    Q_FOREACH (QCriterion *crit, d->m_criteriaList)
        pass->addCriterion(qobject_cast<QCriterion *>(crit->clone(isClone)));
    Q_FOREACH (QParameterMapper *binding, d->m_bindings)
        pass->addBinding(qobject_cast<QParameterMapper *>(binding->clone(isClone)));
    Q_FOREACH (QRenderState *renderState, d->m_renderStates)
        pass->addRenderState(qobject_cast<QRenderState *>(renderState->clone(isClone)));
    pass->d_func()->m_shader = qobject_cast<QShaderProgram *>(d->m_shader->clone(isClone));

    return pass;
}

ParameterList QRenderPass::attributes() const
{
    Q_D(const QRenderPass);
    return d->m_attributes;
}

ParameterList QRenderPass::uniforms() const
{
    Q_D(const QRenderPass);
    return d->m_uniforms;
}

void QRenderPass::addCriterion(QCriterion *criterion)
{
    Q_D(QRenderPass);
    if (!d->m_criteriaList.contains(criterion)) {
        d->m_criteriaList.append(criterion);

        // We need to add it as a child of the current node if it has been declared inline
        // Or not previously added as a child of the current node so that
        // 1) The backend gets notified about it's creation
        // 2) When the current node is destroyed, it gets destroyed as well
        if (!criterion->parent() || criterion->parent() == this)
            QNode::addChild(criterion);

        if (d->m_changeArbiter != Q_NULLPTR) {
            QScenePropertyChangePtr change(new QScenePropertyChange(NodeAdded, this));
            change->setPropertyName(QByteArrayLiteral("criterion"));
            change->setValue(QVariant::fromValue(criterion->clone()));
            notifyObservers(change);
        }
    }
}

void QRenderPass::removeCriterion(QCriterion *criterion)
{
    Q_D(QRenderPass);
    if (d->m_changeArbiter != Q_NULLPTR) {
        QScenePropertyChangePtr change(new QScenePropertyChange(NodeRemoved, this));
        change->setPropertyName(QByteArrayLiteral("criterion"));
        change->setValue(QVariant::fromValue(criterion->uuid()));
        notifyObservers(change);
    }
    d->m_criteriaList.removeOne(criterion);
}

QList<QCriterion *> QRenderPass::criteria() const
{
    Q_D(const QRenderPass);
    return d->m_criteriaList;
}

void QRenderPass::clearCriteria()
{
    Q_D(QRenderPass);
    while (d->m_criteriaList.size() > 0)
        removeCriterion(d->m_criteriaList.takeFirst());
}

void QRenderPass::addBinding(QParameterMapper *binding)
{
    Q_D(QRenderPass);
    if (!d->m_bindings.contains(binding)) {
        d->m_bindings.append(binding);

        if (!binding->parent() || binding->parent() == this)
            QNode::addChild(binding);

        if (d->m_changeArbiter != Q_NULLPTR) {
            QScenePropertyChangePtr change(new QScenePropertyChange(NodeAdded, this));
            change->setPropertyName(QByteArrayLiteral("binding"));
            change->setValue(QVariant::fromValue(binding->clone()));
            notifyObservers(change);
        }
    }
}

void QRenderPass::removeBinding(QParameterMapper *binding)
{
    Q_D(QRenderPass);
    if (d->m_changeArbiter != Q_NULLPTR) {
        QScenePropertyChangePtr change(new QScenePropertyChange(NodeRemoved, this));
        change->setPropertyName(QByteArrayLiteral("binding"));
        change->setValue(QVariant::fromValue(binding->uuid()));
        notifyObservers(change);
    }
    d->m_bindings.removeOne(binding);
}

QList<QParameterMapper *> QRenderPass::bindings() const
{
    Q_D(const QRenderPass);
    return d->m_bindings;
}

void QRenderPass::addRenderState(QRenderState *state)
{
    Q_D(QRenderPass);

    if (!d->m_renderStates.contains(state)) {
        d->m_renderStates.append(state);

        if (!state->parent() || state->parent() == this)
            QNode::addChild(state);

        if (d->m_changeArbiter != Q_NULLPTR) {
            QScenePropertyChangePtr change(new QScenePropertyChange(NodeAdded, this));
            change->setPropertyName(QByteArrayLiteral("renderState"));
            change->setValue(QVariant::fromValue(state->clone()));
            notifyObservers(change);
        }
    }
}

void QRenderPass::removeRenderState(QRenderState *state)
{
    Q_D(QRenderPass);
    if (d->m_changeArbiter != Q_NULLPTR) {
        QScenePropertyChangePtr change(new QScenePropertyChange(NodeRemoved, this));
        change->setPropertyName(QByteArrayLiteral("renderState"));
        change->setValue(QVariant::fromValue(state->uuid()));
        notifyObservers(change);
    }
    d->m_renderStates.removeOne(state);
}

QList<QRenderState *> QRenderPass::renderStates() const
{
    Q_D(const QRenderPass);
    return d->m_renderStates;
}

} // namespace Qt3D

QT_END_NAMESPACE
