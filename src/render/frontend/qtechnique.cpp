/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
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

#include "qtechnique.h"
#include "qtechnique_p.h"
#include "qparameter.h"
#include "qopenglfilter.h"
#include <Qt3DCore/qscenepropertychange.h>
#include <QDebug>

QT_BEGIN_NAMESPACE

namespace Qt3D {

QTechniquePrivate::QTechniquePrivate(QTechnique *qq)
    : QNodePrivate(qq)
    , m_openGLFilter(new QOpenGLFilter())
{
}

QTechniquePrivate::~QTechniquePrivate()
{
    delete m_openGLFilter;
}

QTechnique::QTechnique(QNode *parent)
    : QNode(*new QTechniquePrivate(this), parent)
{
    Q_D(QTechnique);
    QObject::connect(d->m_openGLFilter, SIGNAL(openGLFilterChanged()), this, SLOT(openGLFilterChanged()));
}

void QTechniquePrivate::copy(const QNodePrivate *ref)
{
    QNodePrivate::copy(ref);
    const QTechniquePrivate *tech = static_cast<const QTechniquePrivate *>(ref);
    m_openGLFilter->copy(tech->m_openGLFilter);

    Q_FOREACH (QCriterion *criterion, tech->m_criteriaList)
        q_func()->addCriterion(qobject_cast<QCriterion *>(QNodePrivate::get(criterion)->clone()));
    Q_FOREACH (QRenderPass *pass, tech->m_renderPasses)
        q_func()->addPass(qobject_cast<QRenderPass *>(QNodePrivate::get(pass)->clone()));
    Q_FOREACH (QParameter *p, tech->m_parameters)
        q_func()->addParameter(qobject_cast<QParameter *>(QNodePrivate::get(p)->clone()));
}

QTechnique::QTechnique(QTechniquePrivate &dd, QNode *parent)
    : QNode(dd, parent)
{
    Q_D(QTechnique);
    QObject::connect(d->m_openGLFilter, SIGNAL(openGLFilterChanged()), this, SLOT(openGLFilterChanged()));
}

void QTechnique::openGLFilterChanged()
{
    Q_D(QTechnique);
    if (d->m_changeArbiter != Q_NULLPTR) {
        QScenePropertyChangePtr change(new QScenePropertyChange(NodeUpdated, this));
        change->setPropertyName(QByteArrayLiteral("openGLFilter"));
        QOpenGLFilter *clone = new QOpenGLFilter();
        clone->copy(d->m_openGLFilter);
        change->setValue(QVariant::fromValue(clone));
        d->notifyObservers(change);
    }
}

void QTechnique::addCriterion(QCriterion *criterion)
{
    Q_D(QTechnique);
    if (!d->m_criteriaList.contains(criterion)) {
        d->m_criteriaList.append(criterion);

        // We need to add it as a child of the current node if it has been declared inline
        // Or not previously added as a child of the current node so that
        // 1) The backend gets notified about it's creation
        // 2) When the current node is destroyed, it gets destroyed as well
        if (!criterion->parent())
            criterion->setParent(this);

        if (d->m_changeArbiter != Q_NULLPTR) {
            QScenePropertyChangePtr change(new QScenePropertyChange(NodeAdded, this));
            change->setPropertyName(QByteArrayLiteral("criterion"));
            change->setValue(QVariant::fromValue(QNodePrivate::get(criterion)->clone()));
            d->notifyObservers(change);
        }
    }
}

void QTechnique::removeCriterion(QCriterion *criterion)
{
    Q_D(QTechnique);
    if (d->m_changeArbiter != Q_NULLPTR) {
        QScenePropertyChangePtr change(new QScenePropertyChange(NodeRemoved, this));
        change->setPropertyName(QByteArrayLiteral("criterion"));
        change->setValue(QVariant::fromValue(criterion->uuid()));
        d->notifyObservers(change);
    }
    d->m_criteriaList.removeOne(criterion);
}

QList<QCriterion *> QTechnique::criteria() const
{
    Q_D(const QTechnique);
    return d->m_criteriaList;
}

void QTechnique::clearCriteria()
{
    Q_D(QTechnique);
    while (d->m_criteriaList.size() > 0)
        removeCriterion(d->m_criteriaList.takeFirst());
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
            QScenePropertyChangePtr change(new QScenePropertyChange(NodeAdded, this));
            change->setPropertyName(QByteArrayLiteral("parameter"));
            change->setValue(QVariant::fromValue(QNodePrivate::get(parameter)->clone()));
            d->notifyObservers(change);
        }
    }
}

void QTechnique::removeParameter(QParameter *parameter)
{
    Q_D(QTechnique);

    if (d->m_changeArbiter != Q_NULLPTR) {
        QScenePropertyChangePtr change(new QScenePropertyChange(NodeRemoved, this));
        change->setPropertyName(QByteArrayLiteral("parameter"));
        change->setValue(QVariant::fromValue(QNodePrivate::get(parameter)->clone()));
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
            QScenePropertyChangePtr e(new QScenePropertyChange(NodeAdded, this));
            e->setPropertyName(QByteArrayLiteral("pass"));
            e->setValue(QVariant::fromValue(pass));
            d->notifyObservers(e);
        }
    }
}

/*!
 * Removes a \a pass from the technique. This posts a ComponentRemoved
 * QScenePropertyChange notification to the QChangeArbiter with the value
 * being the \a pass' uuid and the property name being "pass".
 */
void QTechnique::removePass(QRenderPass *pass)
{
    Q_D(QTechnique);
    if (d->m_changeArbiter) {
        QScenePropertyChangePtr e(new QScenePropertyChange(NodeRemoved, this));
        e->setPropertyName(QByteArrayLiteral("pass"));
        e->setValue(QVariant::fromValue(pass->uuid()));
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

QParameter *QTechnique::parameterByName(QString name) const
{
    Q_D(const QTechnique);
    foreach (QParameter* p, d->m_parameters) {
        if (p->name() == name)
            return p;
    }
    return NULL;
}

QOpenGLFilter *QTechnique::openGLFilter() const
{
    Q_D(const QTechnique);
    return d->m_openGLFilter;
}

} // of namespace Qt3D

QT_END_NAMESPACE
