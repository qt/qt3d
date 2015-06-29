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
#include <Qt3DRenderer/qrenderstate.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

QStateSetPrivate::QStateSetPrivate()
    : QFrameGraphNodePrivate()
{
}

/*!
 * \class Qt3D::QStateSet
 *
 * \brief The Qt3D::QStateSet FrameGraph node offers a way of specifying a set
 * of Qt3D::QRenderState to be applied during the execution of a FrameGraph
 * branch.
 *
 * States set on a Qt3D::QStateSet are set globally, contrary to the per
 * Material states that can be set on a Qt3D::QRenderPass. By default an empty
 * Qt3D::QStateSet will result in all render states being disabled when
 * executed. Adding a Qt3D::QRenderState state explicitly enables that render
 * state at runtime.
 *
 * \since 5.5
 *
 * \sa Qt3D::QRenderState, Qt3D::QRenderPass
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
 * Adds a new Qt3D::QRenderState \a state to the Qt3D::QStateSet instance.
 *
 * \note Not setting any Qt3D::QRenderState state on a Qt3D::QStateSet instance
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
            change->setValue(QVariant::fromValue(QNodePtr(QNode::clone(state))));
            d->notifyObservers(change);
        }
    }
}

/*!
 * Removes the Qt3D::QRenderState \a state from the Qt3D::QStateSet instance.
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
 * Returns the list of Qt3D::QRenderState that compose the Qt3D::QStateSet instance.
 */
QList<QRenderState *> QStateSet::renderStates() const
{
    Q_D(const QStateSet);
    return d->m_renderStates;
}

} // Qt3D

QT_END_NAMESPACE
