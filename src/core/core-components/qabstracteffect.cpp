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

#include "qabstracteffect.h"
#include "qabstracteffect_p.h"
#include "qabstracttechnique.h"
#include <Qt3DCore/qscenepropertychange.h>

/*!
 * \class QAbstractEffect
 * \namespace Qt3D
 *
 * \brief Provides an abstract class that should be the base of all Effect element
 * classes in a scene
 *
 * QAbstractEffect is bound to be linked by a Material. It is responsible for holding all the
 * techniques that can be used to perform rendering.
 *
 * \sa QAbstractTechnique, Component
 */


QT_BEGIN_NAMESPACE

namespace Qt3D {


QAbstractEffectPrivate::QAbstractEffectPrivate(QAbstractEffect *qq)
        : NodePrivate(qq)
    {}

QAbstractEffect::QAbstractEffect(Node *parent)
    : Node(*new QAbstractEffectPrivate(this), parent)
{
}

QAbstractEffect::QAbstractEffect(QAbstractEffectPrivate &dd, Node *parent)
    : Node(dd, parent)
{
}

/*!
 * Adds a new technique \a t to the effect. This posts a CommponentAdded
 * QScenePropertyChange notification to the QChangeArbiter, the value is
 * the added technique and the property name is "technique".
 */
void QAbstractEffect::addTechnique(QAbstractTechnique *t)
{
    Q_ASSERT(t);
    Q_D(QAbstractEffect);
    if (!d->m_techniques.contains(t)) {
        d->m_techniques.append(t);
        QScenePropertyChangePtr e(new QScenePropertyChange(ComponentAdded, this));
        e->m_propertyName = QByteArrayLiteral("technique");
        e->m_value = QVariant::fromValue(t);
        notifyObservers(e);
    }
}

/*!
 * Removes a technique \t from the effect. This posts a ComponentRemoved
 * QScenePropertyChange notification to the QChangeArbiter, the valus is
 * the removed technique and the property name is "technique".
 */
void QAbstractEffect::removeTechnique(QAbstractTechnique *t)
{
    Q_D(QAbstractEffect);
    d->m_techniques.removeOne(t);
    QScenePropertyChangePtr e(new QScenePropertyChange(ComponentRemoved, this));
    e->m_propertyName = QByteArrayLiteral("technique");
    e->m_value = QVariant::fromValue(t);
    notifyObservers(e);
}

/*!
 * Returns the list of techniques used by the effect.
 */
QList<QAbstractTechnique *> QAbstractEffect::techniques() const
{
    Q_D(const QAbstractEffect);
    return d->m_techniques;
}

/*!
 * Clears the list of techniques used by the effet.
 * No notification is posted when using this method.
 */
void QAbstractEffect::clearTechniques()
{
    Q_D(QAbstractEffect);
    qDeleteAll(d->m_techniques);
    d->m_techniques.clear();
}

} // namespace Qt3D

QT_END_NAMESPACE
