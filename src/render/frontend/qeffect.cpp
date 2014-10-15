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

#include "qeffect.h"
#include "qeffect_p.h"
#include "qtechnique.h"
#include "qparameter.h"

#include <Qt3DCore/qscenepropertychange.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

QEffectPrivate::QEffectPrivate(QEffect *qq)
    : QNodePrivate(qq)
{}

QEffect::QEffect(QNode *parent)
    : QNode(*new QEffectPrivate(this), parent)
{
}

void QEffectPrivate::copy(const QNodePrivate *ref)
{
    QNodePrivate::copy(ref);
    const QEffectPrivate *effect = static_cast<const QEffectPrivate*>(ref);
    Q_FOREACH (QParameter *p, effect->m_parameters)
        q_func()->addParameter(qobject_cast<QParameter *>(QNodePrivate::get(p)->clone()));

    Q_FOREACH (QTechnique *t, effect->m_techniques)
        q_func()->addTechnique(qobject_cast<QTechnique *>(QNodePrivate::get(t)->clone()));
}

QEffect::QEffect(QEffectPrivate &dd, QNode *parent)
    : QNode(dd, parent)
{
}

void QEffect::addParameter(QParameter *parameter)
{
    Q_D(QEffect);
    if (!d->m_parameters.contains(parameter)) {
        d->m_parameters.append(parameter);

        // We need to add it as a child of the current node if it has been declared inline
        // Or not previously added as a child of the current node so that
        // 1) The backend gets notified about it's creation
        // 2) When the current node is destroyed, it gets destroyed as well
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

void QEffect::removeParameter(QParameter *parameter)
{
    Q_D(QEffect);

    if (d->m_changeArbiter != Q_NULLPTR) {
        QScenePropertyChangePtr change(new QScenePropertyChange(NodeRemoved, this));
        change->setPropertyName(QByteArrayLiteral("parameter"));
        change->setValue(QVariant::fromValue(QNodePrivate::get(parameter)->clone()));
        d->notifyObservers(change);
    }
    d->m_parameters.removeOne(parameter);
}

QList<QParameter *> QEffect::parameters() const
{
    Q_D(const QEffect);
    return d->m_parameters;
}

/*!
 * Adds a new technique \a t to the effect. This posts a CommponentAdded
 * QScenePropertyChange notification to the QChangeArbiter, the value is
 * the added technique and the property name is "technique".
 */
void QEffect::addTechnique(QTechnique *t)
{
    Q_ASSERT(t);
    Q_D(QEffect);
    if (!d->m_techniques.contains(t)) {
        d->m_techniques.append(t);

        // We need to add it as a child of the current node if it has been declared inline
        // Or not previously added as a child of the current node so that
        // 1) The backend gets notified about it's creation
        // 2) When the current node is destroyed, tit gets destroyed as well
        if (!t->parent())
            t->setParent(this);

        if (d->m_changeArbiter != Q_NULLPTR) {
            QScenePropertyChangePtr e(new QScenePropertyChange(NodeAdded, this));
            e->setPropertyName(QByteArrayLiteral("technique"));
            e->setValue(QVariant::fromValue(t));
            d->notifyObservers(e);
        }
    }
}

/*!
 * Removes a technique \t from the effect. This posts a ComponentRemoved
 * QScenePropertyChange notification to the QChangeArbiter, the value is
 * the removed technique's uuid and the property name is "technique".
 */
void QEffect::removeTechnique(QTechnique *t)
{
    Q_D(QEffect);
    if (d->m_changeArbiter != Q_NULLPTR) {
        QScenePropertyChangePtr e(new QScenePropertyChange(NodeRemoved, this));
        e->setPropertyName(QByteArrayLiteral("technique"));
        e->setValue(QVariant::fromValue(t->uuid()));
        d->notifyObservers(e);
    }
    d->m_techniques.removeOne(t);
}

/*!
 * Returns the list of techniques used by the effect.
 */
QList<QTechnique *> QEffect::techniques() const
{
    Q_D(const QEffect);
    return d->m_techniques;
}


} // Qt3D

QT_END_NAMESPACE
