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

#include "qmaterial.h"
#include "qmaterial_p.h"
#include "qtexture.h"
#include "qeffect.h"
#include <Qt3DRenderer/private/renderlogging_p.h>
#include "qparameter.h"
#include <Qt3DCore/qscenepropertychange.h>

/*!
 * \class Qt3D::QMaterial
 *
 * \inherits Component
 *
 * \brief Provides an abstract class that should be the base of all
 * Material component classes in a scene.
 *
 * QAbstractMaterial provide a way to specify the rendering of an Entity.
 * Any aspect can define its own subclass of QAbstractMaterial so that a
 * Material can be used to describe a visual element; for example, the way
 * sound should reflect off an element, the temperature of a surface,
 * and so on.
 *
 * \sa QEffect, QMesh, {Qt3D::}{QComponent}
 */

QT_BEGIN_NAMESPACE

namespace Qt3D {

/*!
    \class Qt3D::QMaterialPrivate
    \internal
*/
QMaterialPrivate::QMaterialPrivate()
    : QComponentPrivate()
    , m_effect(Q_NULLPTR)
{
}

void QMaterial::copy(const QNode *ref)
{
    QComponent::copy(ref);
    const QMaterial *material = static_cast<const QMaterial*>(ref);
    Q_FOREACH (QParameter *p, material->d_func()->m_parameters)
        addParameter(qobject_cast<QParameter *>(QNode::clone(p)));

    // TO DO: We may want to copy the node id of the effect and only send a clone
    // when we are the parent of the effect
    if (material->d_func()->m_effect && material->d_func()->m_effect->parent() == ref)
        setEffect(qobject_cast<QEffect *>(QNode::clone(material->d_func()->m_effect)));
}

QMaterial::QMaterial(QNode *parent)
    : QComponent(*new QMaterialPrivate, parent)
{
}

QMaterial::~QMaterial()
{
    QNode::cleanup();
}

/*! \internal */
QMaterial::QMaterial(QMaterialPrivate &dd, QNode *parent)
    : QComponent(dd, parent)
{
}

/*!
 * Sets the \a effect to be used with the Material.
 */
void QMaterial::setEffect(QEffect *effect)
{
    Q_D(QMaterial);
    if (effect != d->m_effect) {

        if (d->m_effect && d->m_changeArbiter) {
            QScenePropertyChangePtr change(new QScenePropertyChange(NodeRemoved, QSceneChange::Node, id()));
            change->setPropertyName("effect");
            change->setValue(QVariant::fromValue(d->m_effect->id()));
            d->notifyObservers(change);
        }

        d->m_effect = effect;
        const bool blocked = blockNotifications(true);
        emit effectChanged();
        blockNotifications(blocked);
        // We need to add it as a child of the current node if it has been declared inline
        // Or not previously added as a child of the current node so that
        // 1) The backend gets notified about it's creation
        // 2) When the current node is destroyed, it gets destroyed as well
        if (effect && !effect->parent())
            effect->setParent(this);

        if (d->m_effect && d->m_changeArbiter) {
            QScenePropertyChangePtr change(new QScenePropertyChange(NodeAdded, QSceneChange::Node, id()));
            change->setPropertyName("effect");
            change->setValue(QVariant::fromValue(effect->id()));
            d->notifyObservers(change);
        }
    }
}

/*!
 * Returns the effect used by the Material.
 */
QEffect *QMaterial::effect() const
{
    Q_D(const QMaterial);
    return d->m_effect;
}

void QMaterial::addParameter(QParameter *parameter)
{
    Q_D(QMaterial);
    if (!d->m_parameters.contains(parameter)) {
        d->m_parameters.append(parameter);

        // We need to add it as a child of the current node if it has been declared inline
        // Or not previously added as a child of the current node so that
        // 1) The backend gets notified about it's creation
        // 2) When the current node is destroyed, it gets destroyed as well
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

void QMaterial::removeParameter(QParameter *parameter)
{
    Q_D(QMaterial);
    if (d->m_changeArbiter != Q_NULLPTR) {
        QScenePropertyChangePtr change(new QScenePropertyChange(NodeRemoved, QSceneChange::Node, id()));
        change->setPropertyName("parameter");
        change->setValue(QVariant::fromValue(parameter->id()));
        d->notifyObservers(change);
    }
    d->m_parameters.removeOne(parameter);
}

QList<QParameter *> QMaterial::parameters() const
{
    Q_D(const QMaterial);
    return d->m_parameters;
}

TextureDict QMaterial::textureValues() const
{
    Q_D(const QMaterial);
    return d->m_textures;
}

// TO DO: Check if this is really needed
void QMaterial::setTextureParameter(QString name, QAbstractTextureProvider *tex)
{
    Q_D(QMaterial);
    d->m_textures[name] = tex;
}

} // namespace Qt3D

QT_END_NAMESPACE
