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

#include "qabstractmaterial.h"
#include "qabstractmaterial_p.h"
#include "qscenepropertychange.h"
#include "qabstracteffect.h"

/*!
 * \class QAbstractMaterial
 * \namespace Qt3D
 *
 * \inherits Component
 *
 * \brief Provides an abstract class that should be the base of all
 * Material component classes in a scene.
 *
 * QAbstractMaterial provide a way to specify the rendering of an Entity.
 * Any aspect can define its own subclass of QAbstractMaterial so that a
 * Material can be used to describe a visual element, the way sound should
 * reflect on an element, the temperature of a surface and so on.
 *
 * \sa QAbstractEffect, QAbstractMesh, Component
 */

QT_BEGIN_NAMESPACE

namespace Qt3D {

QAbstractMaterialPrivate::QAbstractMaterialPrivate(QAbstractMaterial *qq)
    : QComponentPrivate(qq)
    , m_effect(Q_NULLPTR)
{}

QAbstractMaterial::QAbstractMaterial(QNode *parent)
    : QComponent(*new QAbstractMaterialPrivate(this), parent)
{
}

QAbstractMaterial::QAbstractMaterial(QAbstractMaterialPrivate &dd, QNode *parent)
    : QComponent(dd, parent)
{
}

/*!
 * Sets the \a effect to be used with the Material.
 */
void QAbstractMaterial::setEffect(QAbstractEffect *effect)
{
    Q_D(QAbstractMaterial);
    if (effect != d->m_effect) {

        if (d->m_effect != Q_NULLPTR && d->m_changeArbiter != Q_NULLPTR) {
            QScenePropertyChangePtr change(new QScenePropertyChange(NodeRemoved, this));
            change->setPropertyName(QByteArrayLiteral("effect"));
            change->setValue(QVariant::fromValue(d->m_effect));
            notifyObservers(change);
        }

        d->m_effect = effect;
        emit effectChanged();

        // We need to add it as a child of the current node if it has been declared inline
        // Or not previously added as a child of the current node so that
        // 1) The backend gets notified about it's creation
        // 2) When the current node is destroyed, it gets destroyed as well
        if (!effect->parent() || effect->parent() == this)
            QNode::addChild(effect);

        if (d->m_changeArbiter != Q_NULLPTR) {
            QScenePropertyChangePtr change(new QScenePropertyChange(NodeAdded, this));
            change->setPropertyName(QByteArrayLiteral("effect"));
            change->setValue(QVariant::fromValue(effect));
            notifyObservers(change);
        }
    }
}

/*!
 * Returns the effect used by the Material.
 */
QAbstractEffect *QAbstractMaterial::effect() const
{
    Q_D(const QAbstractMaterial);
    return d->m_effect;
}

} // Qt3D

QT_END_NAMESPACE
