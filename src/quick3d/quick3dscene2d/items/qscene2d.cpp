/****************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
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

#include "qscene2d.h"
#include "qscene2d_p.h"
#include "scene2d_p.h"
#include "scene2dmanager_p.h"
#include "scene2devent_p.h"

#include <Qt3DCore/qentity.h>
#include <Qt3DCore/qpropertynodeaddedchange.h>
#include <Qt3DCore/qpropertynoderemovedchange.h>

QT_BEGIN_NAMESPACE

using namespace Qt3DCore;

namespace Qt3DRender {

namespace Quick {

/*!
    \class Qt3DRender::Quick::QScene2D
    \inmodule Qt3DScene2D

    \brief This class enables rendering qml into a texture, which then can be
    used as a part of 3D scene.

    The component uses QQuickRenderControl to render the given QML source into an
    offscreen surface, which is attached to a texture provided by the user. This allows the
    component to directly render into the texture without intermediate copy and the user to
    freely specify how the texture is used in the 3D scene.

    \since 5.9
*/

/*!
    \qmltype Scene2D
    \inqmlmodule Qt3D.Scene2D
    \since
    \ingroup
    \instantiates Qt3DRender::Quick::QScene2D
    \brief Scene2D
 */

/*!
    \enum QScene2D::RenderPolicy

    This enum type describes types of render policies available.
    \value Continuous The Scene2D is rendering continuously. This is the default render policy.
    \value SingleShot The Scene2D renders to the texture only once after which the resources
                      allocated for rendering are released.
*/

/*!
    \qmlproperty RenderTargetOutput Qt3D.Render::Scene2D::output
    Holds the RenderTargetOutput, which specifies where the Scene2D is rendering to.
 */

/*!
    \qmlproperty enumeration Qt3D.Render::Scene2D::renderPolicy
    Holds the render policy of this Scene2D.
 */

QScene2DPrivate::QScene2DPrivate()
    : Qt3DCore::QNodePrivate()
    , m_renderManager(new Scene2DManager(this))
    , m_output(nullptr)
{
}

QScene2DPrivate::~QScene2DPrivate()
{
    m_renderManager->cleanup();
    delete m_renderManager;
}


/*!
    The constructor creates a new QScene2D instance with the specified \a parent.
 */
QScene2D::QScene2D(Qt3DCore::QNode *parent)
    : Qt3DCore::QNode(*new QScene2DPrivate, parent)
{
}

QQuickItem* QScene2D::item() const
{
    Q_D(const QScene2D);
    return d->m_renderManager->m_item;
}

void QScene2D::setItem(QQuickItem *item)
{
    Q_D(QScene2D);
    if (d->m_renderManager->m_initialized) {
        qWarning() << "Unable to set item after initialization.";
        return;
    }
    if (d->m_renderManager->m_item != item) {
        d->m_renderManager->setItem(item);
        emit itemChanged(item);
    }
}

/*!
    \property QScene2D::renderPolicy

    Holds the render policy of this Scene2D.
 */
QScene2D::RenderPolicy QScene2D::renderPolicy() const
{
    Q_D(const QScene2D);
    return d->m_renderManager->m_renderPolicy;
}

void QScene2D::setRenderPolicy(QScene2D::RenderPolicy renderPolicy)
{
    Q_D(const QScene2D);
    if (d->m_renderManager->m_renderPolicy != renderPolicy) {
        d->m_renderManager->m_renderPolicy = renderPolicy;
        emit renderPolicyChanged(renderPolicy);
    }
}

/*!
    \property QScene2D::output
    Holds the QRenderTargetOutput, which specifies where the QScene2D is
    rendering to.
 */
Qt3DRender::QRenderTargetOutput *QScene2D::output() const
{
    Q_D(const QScene2D);
    return d->m_output;
}

void QScene2D::setOutput(Qt3DRender::QRenderTargetOutput *output)
{
    Q_D(QScene2D);
    if (d->m_output != output) {
        if (d->m_output)
            d->unregisterDestructionHelper(d->m_output);
        d->m_output = output;
        if (output)
            d->registerDestructionHelper(output, &QScene2D::setOutput, d->m_output);
        emit outputChanged(output);
    }
}

Qt3DCore::QNodeCreatedChangeBasePtr QScene2D::createNodeCreationChange() const
{
    auto creationChange = Qt3DCore::QNodeCreatedChangePtr<QScene2DData>::create(this);
    auto &data = creationChange->data;
    Q_D(const QScene2D);
    data.renderPolicy = d->m_renderManager->m_renderPolicy;
    data.sharedObject = d->m_renderManager->m_sharedObject;
    data.output = d->m_output ? d->m_output->id() : Qt3DCore::QNodeId();
    for (Qt3DCore::QEntity *e : d->m_entities)
        data.entityIds.append(e->id());
    return creationChange;
}

bool QScene2D::event(QEvent *event)
{
    Q_D(QScene2D);
    d->m_renderManager->forwardEvent(event);
    return true;
}

bool QScene2D::isGrabMouseEnabled() const
{
    Q_D(const QScene2D);
    return d->m_renderManager->m_grabMouse;
}

QVector<Qt3DCore::QEntity*> QScene2D::entities()
{
    Q_D(const QScene2D);
    return d->m_entities;
}

void QScene2D::addEntity(Qt3DCore::QEntity *entity)
{
    Q_D(QScene2D);
    if (!d->m_entities.contains(entity)) {
        d->m_entities.append(entity);

        d->registerDestructionHelper(entity, &QScene2D::removeEntity, d->m_entities);

        if (d->m_changeArbiter != nullptr) {
            const auto change = Qt3DCore::QPropertyNodeAddedChangePtr::create(id(), entity);
            change->setPropertyName("entities");
            d->notifyObservers(change);
        }
    }
}

void QScene2D::removeEntity(Qt3DCore::QEntity *entity)
{
    Q_D(QScene2D);
    if (d->m_entities.contains(entity)) {
        d->m_entities.removeAll(entity);

        d->unregisterDestructionHelper(entity);

        if (d->m_changeArbiter != nullptr) {
            const auto change = Qt3DCore::QPropertyNodeRemovedChangePtr::create(id(), entity);
            change->setPropertyName("entities");
            d->notifyObservers(change);
        }
    }
}

void QScene2D::setGrabMouseEnabled(bool grab)
{
    Q_D(QScene2D);
    if (d->m_renderManager->m_grabMouse != grab) {
        d->m_renderManager->m_grabMouse = grab;
        emit grabMouseChanged(grab);
    }
}


} // namespace Quick
} // namespace Qt3DRender

QT_END_NAMESPACE
