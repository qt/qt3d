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
    : QAbstractEffectPrivate(qq)
{}

QEffect::QEffect(QNode *parent)
    : QAbstractEffect(*new QEffectPrivate(this), parent)
{
}

void QEffect::copy(const QNode *ref)
{
    QAbstractEffect::copy(ref);
}

QEffect::QEffect(QEffectPrivate &dd, QNode *parent)
    : QAbstractEffect(dd, parent)
{
}

QEffect *QEffect::doClone(QNode *clonedParent) const
{
    Q_D(const QEffect);
    QEffect *effect = new QEffect(clonedParent);

    Q_FOREACH (QParameter *p, d->m_parameters)
        effect->addParameter(qobject_cast<QParameter *>(p->clone(effect)));

    Q_FOREACH (QAbstractTechnique *t, d->m_techniques)
        effect->addTechnique(qobject_cast<QAbstractTechnique *>(t->clone(effect)));

    return effect;
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
        if (!parameter->parent() || parameter->parent() == this)
            addChild(parameter);

        if (d->m_changeArbiter != Q_NULLPTR) {
            QScenePropertyChangePtr change(new QScenePropertyChange(NodeAdded, this));
            change->setPropertyName(QByteArrayLiteral("parameter"));
            change->setValue(QVariant::fromValue(parameter->clone()));
            notifyObservers(change);
        }
    }
}

void QEffect::removeParameter(QParameter *parameter)
{
    Q_D(QEffect);

    if (d->m_changeArbiter != Q_NULLPTR) {
        QScenePropertyChangePtr change(new QScenePropertyChange(NodeRemoved, this));
        change->setPropertyName(QByteArrayLiteral("parameter"));
        change->setValue(QVariant::fromValue(parameter->clone()));
        notifyObservers(change);
    }
    d->m_parameters.removeOne(parameter);
}

QList<QParameter *> QEffect::parameters() const
{
    Q_D(const QEffect);
    return d->m_parameters;
}

} // Qt3D

QT_END_NAMESPACE
