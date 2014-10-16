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
#include "qannotation.h"
#include "qparametermapper.h"
#include "qscenepropertychange.h"
#include "qrenderstate.h"
#include "qparametermapper_p.h"
#include "private/qnode_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3D {

QRenderPassPrivate::QRenderPassPrivate(QRenderPass *qq)
    : QNodePrivate(qq)
    , m_shader(Q_NULLPTR)
{
}

void QRenderPass::copy(const QNode *ref)
{
    QNode::copy(ref);
    const QRenderPass *other = static_cast<const QRenderPass*>(ref);
    d_func()->m_shader = qobject_cast<QShaderProgram *>(QNodePrivate::get(other->d_func()->m_shader)->clone());

    Q_FOREACH (QAnnotation *crit, other->d_func()->m_annotationList)
        addAnnotation(qobject_cast<QAnnotation *>(QNodePrivate::get(crit)->clone()));
    Q_FOREACH (QParameterMapper *binding, other->d_func()->m_bindings)
        addBinding(qobject_cast<QParameterMapper *>(QNodePrivate::get(binding)->clone()));
    Q_FOREACH (QRenderState *renderState, other->d_func()->m_renderStates)
        addRenderState(qobject_cast<QRenderState *>(QNodePrivate::get(renderState)->clone()));
}

QRenderPass::QRenderPass(QNode *parent)
    : QNode(*new QRenderPassPrivate(this), parent)
{
}

QRenderPass::QRenderPass(QRenderPassPrivate &dd, QNode *parent)
    : QNode(dd, parent)
{
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

/*!
 * Sets the pass's \a shaderProgram. This posts a ComponentUpdated
 * QScenePropertyChange to the QChangeArbiter. The value is set to
 * the \a ShaderProgram and the property name to "shaderProgram".
 */
void QRenderPass::setShaderProgram(QShaderProgram *shaderProgram)
{
    Q_D(QRenderPass);
    if (d->m_shader != shaderProgram) {

        if (d->m_shader != Q_NULLPTR && d->m_changeArbiter != Q_NULLPTR) {
            QScenePropertyChangePtr e(new QScenePropertyChange(NodeRemoved, this));
            e->setPropertyName(QByteArrayLiteral("shaderProgram"));
            e->setValue(QVariant::fromValue(d->m_shader->uuid()));
            d->notifyObservers(e);
        }

        d->m_shader = shaderProgram;
        emit shaderProgramChanged();

        // We need to add it as a child of the current node if it has been declared inline
        // Or not previously added as a child of the current node so that
        // 1) The backend gets notified about it's creation
        // 2) When the current node is destroyed, it gets destroyed as well
        if (!shaderProgram->parent())
            shaderProgram->setParent(this);

        if (d->m_changeArbiter != Q_NULLPTR) {
            QScenePropertyChangePtr e(new QScenePropertyChange(NodeAdded, this));
            e->setPropertyName(QByteArrayLiteral("shaderProgram"));
            e->setValue(QVariant::fromValue(shaderProgram->uuid()));
            d->notifyObservers(e);
        }
    }
}

QShaderProgram *QRenderPass::shaderProgram() const
{
    Q_D(const QRenderPass);
    return d->m_shader;
}

void QRenderPass::addAnnotation(QAnnotation *annotation)
{
    Q_D(QRenderPass);
    if (!d->m_annotationList.contains(annotation)) {
        d->m_annotationList.append(annotation);

        // We need to add it as a child of the current node if it has been declared inline
        // Or not previously added as a child of the current node so that
        // 1) The backend gets notified about it's creation
        // 2) When the current node is destroyed, it gets destroyed as well
        if (!annotation->parent())
            annotation->setParent(this);

        if (d->m_changeArbiter != Q_NULLPTR) {
            QScenePropertyChangePtr change(new QScenePropertyChange(NodeAdded, this));
            change->setPropertyName(QByteArrayLiteral("criterion"));
            change->setValue(QVariant::fromValue(qobject_cast<QAnnotation *>(QNodePrivate::get(annotation)->clone())));
            d->notifyObservers(change);
        }
    }
}

void QRenderPass::removeAnnotation(QAnnotation *annotation)
{
    Q_D(QRenderPass);
    if (d->m_changeArbiter != Q_NULLPTR) {
        QScenePropertyChangePtr change(new QScenePropertyChange(NodeRemoved, this));
        change->setPropertyName(QByteArrayLiteral("criterion"));
        change->setValue(QVariant::fromValue(annotation->uuid()));
        d->notifyObservers(change);
    }
    d->m_annotationList.removeOne(annotation);
}

QList<QAnnotation *> QRenderPass::annotations() const
{
    Q_D(const QRenderPass);
    return d->m_annotationList;
}

void QRenderPass::addBinding(QParameterMapper *binding)
{
    Q_D(QRenderPass);
    if (!d->m_bindings.contains(binding)) {
        d->m_bindings.append(binding);

        if (!binding->parent())
            binding->setParent(this);

        if (d->m_changeArbiter != Q_NULLPTR) {
            QScenePropertyChangePtr change(new QScenePropertyChange(NodeAdded, this));
            change->setPropertyName(QByteArrayLiteral("binding"));
            change->setValue(QVariant::fromValue(QNodePrivate::get(binding)->clone()));
            d->notifyObservers(change);
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
        d->notifyObservers(change);
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

        if (!state->parent())
            state->setParent(this);

        if (d->m_changeArbiter != Q_NULLPTR) {
            QScenePropertyChangePtr change(new QScenePropertyChange(NodeAdded, this));
            change->setPropertyName(QByteArrayLiteral("renderState"));
            change->setValue(QVariant::fromValue(QNodePrivate::get(state)->clone()));
            d->notifyObservers(change);
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
        d->notifyObservers(change);
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
