/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
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

#include "qstateset.h"
#include "qstateset_p.h"

#include <Qt3DCore/qscenepropertychange.h>
#include <Qt3DRender/qrenderstate.h>

QT_BEGIN_NAMESPACE

using namespace Qt3DCore;

namespace Qt3DRender {

QStateSetPrivate::QStateSetPrivate()
    : QFrameGraphNodePrivate()
{
}

/*!
 * \class Qt3DRender::QStateSet
 * \inmodule Qt3DRender
 *
 * \brief The QStateSet \l {QFrameGraphNode}{FrameGraph} node offers a way of
 * specifying a set of QRenderState objects to be applied during the execution
 * of a framegraph branch.
 *
 * States set on a QStateSet are set globally, contrary to the per-material
 * states that can be set on a QRenderPass. By default, an empty
 * QStateSet will result in all render states being disabled when
 * executed. Adding a QRenderState state explicitly enables that render
 * state at runtime.
 *
 * \since 5.5
 *
 * \sa QRenderState, QRenderPass
 */

QStateSet::QStateSet(QNode *parent)
    : QFrameGraphNode(*new QStateSetPrivate, parent)
{
}

QStateSet::QStateSet(QStateSetPrivate &dd, QNode *parent)
    : QFrameGraphNode(dd, parent)
{
}

QStateSet::~QStateSet()
{
    QNode::cleanup();
}

void QStateSet::copy(const QNode *ref)
{
    QFrameGraphNode::copy(ref);
    const QStateSet *other = static_cast<const QStateSet*>(ref);

    Q_FOREACH (QRenderState *renderState, other->d_func()->m_renderStates)
        addRenderState(qobject_cast<QRenderState *>(QNode::clone(renderState)));
}

/*!
 * Adds a new QRenderState \a state to the QStateSet instance.
 *
 * \note Not setting any QRenderState state on a QStateSet instance
 * implies all the render states will be disabled at render time.
 */
void QStateSet::addRenderState(QRenderState *state)
{
    Q_D(QStateSet);

    if (!d->m_renderStates.contains(state)) {
        d->m_renderStates.append(state);

        if (!state->parent())
            state->setParent(this);

        if (d->m_changeArbiter != Q_NULLPTR) {
            QScenePropertyChangePtr change(new QScenePropertyChange(NodeAdded, QSceneChange::Node, id()));
            change->setPropertyName("renderState");
            // Since we have no RenderState managers, we need to send a clone
            change->setValue(QVariant::fromValue(QNodePtr(QNode::clone(state))));
            d->notifyObservers(change);
        }
    }
}

/*!
 * Removes the QRenderState \a state from the QStateSet instance.
 */
void QStateSet::removeRenderState(QRenderState *state)
{
    Q_D(QStateSet);
    if (d->m_changeArbiter != Q_NULLPTR) {
        QScenePropertyChangePtr change(new QScenePropertyChange(NodeRemoved, QSceneChange::Node, id()));
        change->setPropertyName("renderState");
        change->setValue(QVariant::fromValue(state->id()));
        d->notifyObservers(change);
    }
    d->m_renderStates.removeOne(state);
}

/*!
 * Returns the list of QRenderState objects that compose the QStateSet instance.
 */
QList<QRenderState *> QStateSet::renderStates() const
{
    Q_D(const QStateSet);
    return d->m_renderStates;
}

} // namespace Qt3DRender

QT_END_NAMESPACE
