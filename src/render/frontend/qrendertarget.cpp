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

#include "qrendertarget.h"
#include "qrendertarget_p.h"
#include "qrendertargetoutput.h"
#include <Qt3DCore/qscenepropertychange.h>

QT_BEGIN_NAMESPACE

using namespace Qt3DCore;

namespace Qt3DRender {

QRenderTargetPrivate::QRenderTargetPrivate()
    : QComponentPrivate()
{
}

void QRenderTarget::copy(const QNode *ref)
{
    QNode::copy(ref);
    const QRenderTarget *other = static_cast<const QRenderTarget*>(ref);
    Q_FOREACH (QRenderTargetOutput *output, other->d_func()->m_outputs)
        addOutput(qobject_cast<QRenderTargetOutput *>(QNode::clone(output)));
}

QRenderTarget::QRenderTarget(QNode *parent)
    : QComponent(*new QRenderTargetPrivate, parent)
{
}

QRenderTarget::~QRenderTarget()
{
    QNode::cleanup();
}

/*! \internal */
QRenderTarget::QRenderTarget(QRenderTargetPrivate &dd, QNode *parent)
    : QComponent(dd, parent)
{
}

void QRenderTarget::addOutput(QRenderTargetOutput *output)
{
    Q_D(QRenderTarget);
    if (!d->m_outputs.contains(output)) {
        d->m_outputs.append(output);

        if (!output->parent())
            output->setParent(this);

        if (d->m_changeArbiter != Q_NULLPTR) {
            QScenePropertyChangePtr change(new QScenePropertyChange(NodeAdded, QSceneChange::Node, id()));
            change->setPropertyName("output");
            change->setValue(QVariant::fromValue(output->id()));
            d->notifyObservers(change);
        }
    }
}

void QRenderTarget::removeOutput(QRenderTargetOutput *output)
{
    Q_D(QRenderTarget);

    if (d->m_changeArbiter != Q_NULLPTR) {
        QScenePropertyChangePtr change(new QScenePropertyChange(NodeRemoved, QSceneChange::Node, id()));
        change->setPropertyName("output");
        change->setValue(QVariant::fromValue(output->id()));
        d->notifyObservers(change);
    }
    d->m_outputs.removeOne(output);
}

QVector<QRenderTargetOutput *> QRenderTarget::outputs() const
{
    Q_D(const QRenderTarget);
    return d->m_outputs;
}

} // namespace Qt3DRender

QT_END_NAMESPACE
