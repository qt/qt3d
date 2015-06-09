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

#include "qtechnique.h"
#include "qtechnique_p.h"
#include "qparameter.h"
#include "qopenglfilter.h"
#include <Qt3DCore/qscenepropertychange.h>
#include <QDebug>

QT_BEGIN_NAMESPACE

namespace Qt3D {

/*!
    \class Qt3D::QTechniquePrivate
    \internal
*/
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
    QObject::connect(&d->m_openGLFilter, SIGNAL(openGLFilterChanged()), this, SLOT(_q_openGLFilterChanged()));
}

QTechnique::~QTechnique()
{
    QNode::cleanup();
}

/*! \internal */
QTechnique::QTechnique(QTechniquePrivate &dd, QNode *parent)
    : QNode(dd, parent)
{
    Q_D(QTechnique);
    QObject::connect(&d->m_openGLFilter, SIGNAL(openGLFilterChanged()), this, SLOT(_q_openGLFilterChanged()));
}

void QTechnique::copy(const QNode *ref)
{
    QNode::copy(ref);
    const QTechnique *tech = static_cast<const QTechnique*>(ref);
    d_func()->m_openGLFilter.copy(tech->d_func()->m_openGLFilter);

    Q_FOREACH (QAnnotation *annotation, tech->d_func()->m_annotationList)
        addAnnotation(qobject_cast<QAnnotation *>(QNode::clone(annotation)));
    Q_FOREACH (QRenderPass *pass, tech->d_func()->m_renderPasses)
        addPass(qobject_cast<QRenderPass *>(QNode::clone(pass)));
    Q_FOREACH (QParameter *p, tech->d_func()->m_parameters)
        addParameter(qobject_cast<QParameter *>(QNode::clone(p)));
}

void QTechniquePrivate::_q_openGLFilterChanged()
{
    if (m_changeArbiter != Q_NULLPTR) {
        QScenePropertyChangePtr change(new QScenePropertyChange(NodeUpdated, QSceneChange::Node, m_id));
        change->setPropertyName("openGLFilter");
        QOpenGLFilter *clone = new QOpenGLFilter();
        clone->copy(m_openGLFilter);
        change->setValue(QVariant::fromValue(clone));
        notifyObservers(change);
    }
}

void QTechnique::addAnnotation(QAnnotation *criterion)
{
    Q_D(QTechnique);
    if (!d->m_annotationList.contains(criterion)) {
        d->m_annotationList.append(criterion);

        // We need to add it as a child of the current node if it has been declared inline
        // Or not previously added as a child of the current node so that
        // 1) The backend gets notified about it's creation
        // 2) When the current node is destroyed, it gets destroyed as well
        if (!criterion->parent())
            criterion->setParent(this);

        if (d->m_changeArbiter != Q_NULLPTR) {
            QScenePropertyChangePtr change(new QScenePropertyChange(NodeAdded, QSceneChange::Node, id()));
            change->setPropertyName("annotation");
            change->setValue(QVariant::fromValue(criterion->id()));
            d->notifyObservers(change);
        }
    }
}

void QTechnique::removeAnnotation(QAnnotation *criterion)
{
    Q_D(QTechnique);
    if (d->m_changeArbiter != Q_NULLPTR) {
        QScenePropertyChangePtr change(new QScenePropertyChange(NodeRemoved, QSceneChange::Node, id()));
        change->setPropertyName("annotation");
        change->setValue(QVariant::fromValue(criterion->id()));
        d->notifyObservers(change);
    }
    d->m_annotationList.removeOne(criterion);
}

QList<QAnnotation *> QTechnique::annotations() const
{
    Q_D(const QTechnique);
    return d->m_annotationList;
}

void QTechnique::addParameter(QParameter *parameter)
{
    Q_D(QTechnique);
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

void QTechnique::removeParameter(QParameter *parameter)
{
    Q_D(QTechnique);

    if (d->m_changeArbiter != Q_NULLPTR) {
        QScenePropertyChangePtr change(new QScenePropertyChange(NodeRemoved, QSceneChange::Node, id()));
        change->setPropertyName("parameter");
        change->setValue(QVariant::fromValue(parameter->id()));
        d->notifyObservers(change);
    }
    d->m_parameters.removeOne(parameter);
}

/*!
 * Appends a \a pass to the technique. This posts a ComponentAdded
 * QScenePropertyChange notification to the QChangeArbiter with the
 * value being the \a pass and the property name being "pass".
 */
void QTechnique::addPass(QRenderPass *pass)
{
    Q_D(QTechnique);
    if (!d->m_renderPasses.contains(pass)) {
        d->m_renderPasses.append(pass);

        // We need to add it as a child of the current node if it has been declared inline
        // Or not previously added as a child of the current node so that
        // 1) The backend gets notified about it's creation
        // 2) When the current node is destroyed, it gets destroyed as well
        if (!pass->parent())
            pass->setParent(this);

        if (d->m_changeArbiter != Q_NULLPTR) {
            QScenePropertyChangePtr e(new QScenePropertyChange(NodeAdded, QSceneChange::Node, id()));
            e->setPropertyName("pass");
            e->setValue(QVariant::fromValue(pass->id()));
            d->notifyObservers(e);
        }
    }
}

/*!
 * Removes a \a pass from the technique. This posts a ComponentRemoved
 * QScenePropertyChange notification to the QChangeArbiter with the value
 * being the id of \a pass and the property name being "pass".
 */
void QTechnique::removePass(QRenderPass *pass)
{
    Q_D(QTechnique);
    if (d->m_changeArbiter) {
        QScenePropertyChangePtr e(new QScenePropertyChange(NodeRemoved, QSceneChange::Node, id()));
        e->setPropertyName("pass");
        e->setValue(QVariant::fromValue(pass->id()));
        d->notifyObservers(e);
    }
    d->m_renderPasses.removeOne(pass);
}

/*!
 * Returns the list of render passes contained in the technique.
 */
QList<QRenderPass *> QTechnique::renderPasses() const
{
    Q_D(const QTechnique);
    return d->m_renderPasses;
}

QList<QParameter *> QTechnique::parameters() const
{
    Q_D(const QTechnique);
    return d->m_parameters;
}

QOpenGLFilter *QTechnique::openGLFilter()
{
    Q_D(QTechnique);
    return &d->m_openGLFilter;
}

} // of namespace Qt3D

QT_END_NAMESPACE

#include "moc_qtechnique.cpp"
