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

#include "qmaterial.h"
#include "qmaterial_p.h"
#include "qtexture.h"
#include "qeffect.h"
#include <Qt3DRender/private/renderlogging_p.h>
#include "qparameter.h"
#include <Qt3DCore/qpropertyupdatedchange.h>
#include <Qt3DCore/qpropertynodeaddedchange.h>
#include <Qt3DCore/qpropertynoderemovedchange.h>

/*!
 * \qmltype Material
 * \instantiates Qt3DRender::QMaterial
 * \inqmlmodule Qt3D.Render
 * \brief Non-creatable abstract base for materials.
 * \inherits Component
 */

/*!
 * \class Qt3DRender::QMaterial
 * \inmodule Qt3DRender
 *
 *
 * \brief Provides an abstract class that should be the base of all
 * Material component classes in a scene.
 *
 * QMaterial provides a way to specify the rendering of an Entity.
 * Any aspect can define its own subclass of QMaterial so that a
 * Material can be used to describe a visual element; for example, the way
 * sound should reflect off an element, the temperature of a surface,
 * and so on.
 *
 * \sa QEffect, QMesh, {Qt3DCore::}{QComponent}
 */

QT_BEGIN_NAMESPACE

using namespace Qt3DCore;

namespace Qt3DRender {

QMaterialPrivate::QMaterialPrivate()
    : QComponentPrivate()
    , m_effect(nullptr)
{
}

QMaterialPrivate::~QMaterialPrivate()
{
}

/*!
  \fn Qt3DRender::QMaterial::QMaterial(Qt3DCore::QNode *parent)
  Constructs a new QMaterial with the specified \a parent.
 */
QMaterial::QMaterial(QNode *parent)
    : QComponent(*new QMaterialPrivate, parent)
{
}

/*! \internal */
QMaterial::~QMaterial()
{
}

/*! \internal */
QMaterial::QMaterial(QMaterialPrivate &dd, QNode *parent)
    : QComponent(dd, parent)
{
}

/*!
  \qmlproperty QEffect Qt3D.Render::Material::effect
    Specifies the effect to be used with the material
*/

/*!
  \property Qt3DRender::QMaterial::effect
    Specifies the effect to be used with the material
 */
/*!
 * Sets the \a effect to be used with the Material.
 */
void QMaterial::setEffect(QEffect *effect)
{
    Q_D(QMaterial);
    if (effect != d->m_effect) {

        if (d->m_effect)
            d->unregisterDestructionHelper(d->m_effect);

        // We need to add it as a child of the current node if it has been declared inline
        // Or not previously added as a child of the current node so that
        // 1) The backend gets notified about it's creation
        // 2) When the current node is destroyed, it gets destroyed as well
        if (effect && !effect->parent())
            effect->setParent(this);
        d->m_effect = effect;

        // Ensures proper bookkeeping
        if (d->m_effect)
            d->registerDestructionHelper(d->m_effect, &QMaterial::setEffect, d->m_effect);

        emit effectChanged(effect);
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

/*!
 * Add \a parameter to the Materials parameter.
 */
void QMaterial::addParameter(QParameter *parameter)
{
    Q_ASSERT(parameter);
    Q_D(QMaterial);
    if (!d->m_parameters.contains(parameter)) {
        d->m_parameters.append(parameter);

        // Ensures proper bookkeeping
        d->registerDestructionHelper(parameter, &QMaterial::removeParameter, d->m_parameters);

        // We need to add it as a child of the current node if it has been declared inline
        // Or not previously added as a child of the current node so that
        // 1) The backend gets notified about it's creation
        // 2) When the current node is destroyed, it gets destroyed as well
        if (!parameter->parent())
            parameter->setParent(this);

        if (d->m_changeArbiter != nullptr) {
            const auto change = QPropertyNodeAddedChangePtr::create(id(), parameter);
            change->setPropertyName("parameter");
            d->notifyObservers(change);
        }
    }
}

/*!
 * Remove \a parameter from the Material parameters.
 */
void QMaterial::removeParameter(QParameter *parameter)
{
    Q_ASSERT(parameter);
    Q_D(QMaterial);
    if (d->m_changeArbiter != nullptr) {
        const auto change = QPropertyNodeRemovedChangePtr::create(id(), parameter);
        change->setPropertyName("parameter");
        d->notifyObservers(change);
    }
    d->m_parameters.removeOne(parameter);
}

/*!
 * Returns a vector of the materials current parameters
 */
QVector<QParameter *> QMaterial::parameters() const
{
    Q_D(const QMaterial);
    return d->m_parameters;
}

Qt3DCore::QNodeCreatedChangeBasePtr QMaterial::createNodeCreationChange() const
{
    auto creationChange = Qt3DCore::QNodeCreatedChangePtr<QMaterialData>::create(this);
    auto &data = creationChange->data;
    Q_D(const QMaterial);
    data.parameterIds = qIdsForNodes(d->m_parameters);
    data.effectId = qIdForNode(d->m_effect);
    return creationChange;
}

} // namespace Qt3DRender

QT_END_NAMESPACE
