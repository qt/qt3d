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

#include "qrenderpassfilter.h"
#include "qrenderpassfilter_p.h"

#include <Qt3DCore/qscenepropertychange.h>
#include <Qt3DRenderer/qannotation.h>
#include <Qt3DRenderer/qparameter.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

/*!
    \class Qt3D::QRenderPassFilterPrivate
    \internal
*/

QRenderPassFilter::QRenderPassFilter(QNode *parent)
    : QFrameGraphNode(*new QRenderPassFilterPrivate, parent)
{}

QRenderPassFilter::~QRenderPassFilter()
{
    QNode::cleanup();
}

/*! \internal */
QRenderPassFilter::QRenderPassFilter(QRenderPassFilterPrivate &dd, QNode *parent)
    : QFrameGraphNode(dd, parent)
{
}

QList<QAnnotation *> QRenderPassFilter::includes() const
{
    Q_D(const QRenderPassFilter);
    return d->m_includeList;
}

void QRenderPassFilter::addInclude(QAnnotation *annotation)
{
    Q_D(QRenderPassFilter);
    if (!d->m_includeList.contains(annotation)) {
        d->m_includeList.append(annotation);

        // We need to add it as a child of the current node if it has been declared inline
        // Or not previously added as a child of the current node so that
        // 1) The backend gets notified about it's creation
        // 2) When the current node is destroyed, it gets destroyed as well
        if (!annotation->parent())
            annotation->setParent(this);

        if (d->m_changeArbiter != Q_NULLPTR) {
            QScenePropertyChangePtr propertyChange(new QScenePropertyChange(NodeAdded, QSceneChange::Node, id()));
            propertyChange->setPropertyName("include");
            propertyChange->setValue(QVariant::fromValue(annotation));
            d->notifyObservers(propertyChange);
        }
    }
}

void QRenderPassFilter::removeInclude(QAnnotation *annotation)
{
    Q_D(QRenderPassFilter);
    if (d->m_changeArbiter != Q_NULLPTR) {
        QScenePropertyChangePtr propertyChange(new QScenePropertyChange(NodeRemoved, QSceneChange::Node, id()));
        propertyChange->setPropertyName("include");
        propertyChange->setValue(QVariant::fromValue(annotation));
        d->notifyObservers(propertyChange);
    }
    d->m_includeList.removeOne(annotation);
}

void QRenderPassFilter::copy(const QNode *ref)
{
    QFrameGraphNode::copy(ref);
    const QRenderPassFilter *other = static_cast<const QRenderPassFilter*>(ref);
    Q_FOREACH (QAnnotation *c, other->d_func()->m_includeList)
        addInclude(qobject_cast<QAnnotation *>(QNode::clone(c)));
}

void QRenderPassFilter::addParameter(QParameter *parameter)
{
    Q_D(QRenderPassFilter);
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

void QRenderPassFilter::removeParameter(QParameter *parameter)
{
    Q_D(QRenderPassFilter);

    if (d->m_changeArbiter != Q_NULLPTR) {
        QScenePropertyChangePtr change(new QScenePropertyChange(NodeRemoved, QSceneChange::Node, id()));
        change->setPropertyName("parameter");
        change->setValue(QVariant::fromValue(parameter->id()));
        d->notifyObservers(change);
    }
    d->m_parameters.removeOne(parameter);
}

QList<QParameter *> QRenderPassFilter::parameters() const
{
    Q_D(const QRenderPassFilter);
    return d->m_parameters;
}

} // Qt3D

QT_END_NAMESPACE
