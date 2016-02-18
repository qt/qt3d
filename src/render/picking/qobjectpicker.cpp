/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
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

#include "qobjectpicker.h"
#include <Qt3DCore/qentity.h>
#include <Qt3DCore/private/qcomponent_p.h>
#include <Qt3DCore/qbackendscenepropertychange.h>
#include <Qt3DRender/qpickevent.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

/*!
    \class Qt3DRender::QObjectPicker
    \inmodule Qt3DRender

    \brief The QObjectPicker class instantiates a component that can
    be used to interact with a QEntity by a process known as picking.

    The signals pressed(), released(), clicked(), moved(), entered(), and exited() are
    emitted when the bounding volume defined by the pickAttribute property intersects
    with a ray.

    \note Instances of this component shouldn't be shared, not respecting that
    condition will most likely result in undefined behavior.

    \since 5.6
*/

/*!
    \qmlsignal Qt3D.Render::ObjectPicker::pressed()
*/

/*!
    \qmlsignal Qt3D.Render::ObjectPicker::released()
*/

/*!
    \qmlsignal Qt3D.Render::ObjectPicker::clicked()
*/

/*!
    \qmlsignal Qt3D.Render::ObjectPicker::moved()
*/

/*!
    \qmlsignal Qt3D.Render::ObjectPicker::entered()
*/

/*!
    \qmlsignal Qt3D.Render::ObjectPicker::exited()
*/

class QObjectPickerPrivate : public Qt3DCore::QComponentPrivate
{
public:
    QObjectPickerPrivate()
        : QComponentPrivate()
        , m_hoverEnabled(false)
        , m_mouseTrackingEnabled(false)
        , m_pressed(false)
        , m_containsMouse(false)
        , m_acceptedLastPressedEvent(true)
    {
        m_shareable = false;
    }

    Q_DECLARE_PUBLIC(QObjectPicker)
    bool m_hoverEnabled;
    bool m_mouseTrackingEnabled;
    bool m_pressed;
    bool m_containsMouse;
    bool m_acceptedLastPressedEvent;

    enum EventType {
        Pressed,
        Released,
        Clicked,
        Moved
    };

    void propagateEvent(QPickEvent *event, EventType type);

    void pressedEvent(QPickEvent *event);
    void clickedEvent(QPickEvent *event);
    void movedEvent(QPickEvent *event);
    void releasedEvent(QPickEvent *event);
};

QObjectPicker::QObjectPicker(Qt3DCore::QNode *parent)
    : Qt3DCore::QComponent(*new QObjectPickerPrivate(), parent)
{
}

QObjectPicker::~QObjectPicker()
{
    QComponent::cleanup();
}

void QObjectPicker::setHoverEnabled(bool hoverEnabled)
{
    Q_D(QObjectPicker);
    if (hoverEnabled != d->m_hoverEnabled) {
        d->m_hoverEnabled = hoverEnabled;
        emit hoverEnabledChanged(hoverEnabled);
    }
}

/*!
    \qmlproperty bool Qt3D.Render::ObjectPicker::hoverEnabled
*/
bool QObjectPicker::hoverEnabled() const
{
    Q_D(const QObjectPicker);
    return d->m_hoverEnabled;
}

void QObjectPicker::setMouseTrackingEnabled(bool mouseTrackingEnabled)
{
    Q_D(QObjectPicker);
    if (mouseTrackingEnabled != d->m_mouseTrackingEnabled) {
        d->m_mouseTrackingEnabled = mouseTrackingEnabled;
        emit mouseTrackingEnabledChanged(mouseTrackingEnabled);
    }
}

/*!
    \qmlproperty bool Qt3D.Render::ObjectPicker::mouseTrackingEnabled
*/
bool QObjectPicker::mouseTrackingEnabled() const
{
    Q_D(const QObjectPicker);
    return d->m_mouseTrackingEnabled;
}

/*!
    \qmlproperty bool Qt3D.Render::ObjectPicker::containsMouse
*/
bool QObjectPicker::containsMouse() const
{
    Q_D(const QObjectPicker);
    return d->m_containsMouse;
}

/*!
    \qmlproperty bool Qt3D.Render::ObjectPicker::pressed
*/
bool QObjectPicker::isPressed() const
{
    Q_D(const QObjectPicker);
    return d->m_pressed;
}

void QObjectPicker::copy(const QNode *ref)
{
    QComponent::copy(ref);
    const QObjectPicker *picker = static_cast<const QObjectPicker *>(ref);
    d_func()->m_hoverEnabled = picker->d_func()->m_hoverEnabled;
    d_func()->m_mouseTrackingEnabled = picker->d_func()->m_mouseTrackingEnabled;
}

void QObjectPicker::sceneChangeEvent(const Qt3DCore::QSceneChangePtr &change)
{
    Q_D(QObjectPicker);
    Qt3DCore::QBackendScenePropertyChangePtr e = qSharedPointerCast<Qt3DCore::QBackendScenePropertyChange>(change);
    if (e->type() == Qt3DCore::NodeUpdated) {
        // TO DO: Complete this part
        // to emit the correct signals
        const QByteArray propertyName = e->propertyName();
        if (propertyName == QByteArrayLiteral("pressed")) {
            QPickEventPtr ev = e->value().value<QPickEventPtr>();
            d->pressedEvent(ev.data());
        } else if (propertyName == QByteArrayLiteral("released")) {
            QPickEventPtr ev = e->value().value<QPickEventPtr>();
            d->releasedEvent(ev.data());
        } else if (propertyName == QByteArrayLiteral("clicked")) {
            QPickEventPtr ev = e->value().value<QPickEventPtr>();
            d->clickedEvent(ev.data());
        } else if (propertyName == QByteArrayLiteral("moved")) {
            QPickEventPtr ev = e->value().value<QPickEventPtr>();
            d->movedEvent(ev.data());
        } else if (propertyName == QByteArrayLiteral("entered")) {
            emit entered();
            setContainsMouse(true);
        } else if (propertyName == QByteArrayLiteral("exited")) {
            setContainsMouse(false);
            emit exited();
        }
    }
}

void QObjectPicker::setPressed(bool pressed)
{
    Q_D(QObjectPicker);
    if (d->m_pressed != pressed) {
        const bool blocked = blockNotifications(true);
        d->m_pressed = pressed;
        emit pressedChanged(pressed);
        blockNotifications(blocked);
    }
}

void QObjectPicker::setContainsMouse(bool containsMouse)
{
    Q_D(QObjectPicker);
    if (d->m_containsMouse != containsMouse) {
        const bool blocked = blockNotifications(true);
        d->m_containsMouse = containsMouse;
        emit containsMouseChanged(containsMouse);
        blockNotifications(blocked);
    }
}

/*!
    \internal
 */
void QObjectPickerPrivate::propagateEvent(QPickEvent *event, EventType type)
{
    if (!m_entities.isEmpty()) {
        Qt3DCore::QEntity *entity = m_entities.first();
        Qt3DCore::QEntity *parentEntity = Q_NULLPTR;
        Qt3DRender::QObjectPicker *objectPicker = Q_NULLPTR;
        while (entity != Q_NULLPTR && entity->parent() != Q_NULLPTR && !event->isAccepted()) {
            parentEntity = entity->parentEntity();
            Q_FOREACH (Qt3DCore::QComponent *c, parentEntity->components()) {
                if ((objectPicker = qobject_cast<Qt3DRender::QObjectPicker *>(c)) != Q_NULLPTR) {
                    QObjectPickerPrivate *objectPickerPrivate = static_cast<QObjectPickerPrivate *>(QObjectPickerPrivate::get(objectPicker));
                    switch (type) {
                    case EventType::Pressed:
                        objectPickerPrivate->pressedEvent(event);
                        break;
                    case EventType::Released:
                        objectPickerPrivate->releasedEvent(event);
                        break;
                    case EventType::Clicked:
                        objectPickerPrivate->clickedEvent(event);
                        break;
                    case EventType::Moved:
                        objectPickerPrivate->movedEvent(event);
                        break;
                    }
                    break;
                }
            }
            entity = parentEntity;
        }
    }
}

/*!
    \internal
 */
void QObjectPickerPrivate::pressedEvent(QPickEvent *event)
{
    Q_Q(QObjectPicker);
    emit q->pressed(event);

    m_acceptedLastPressedEvent = event->isAccepted();
    if (!m_acceptedLastPressedEvent) {
        // Travel parents to transmit the event
        propagateEvent(event, EventType::Pressed);
    } else {
        q->setPressed(true);
    }
}

/*!
    \internal
 */
void QObjectPickerPrivate::clickedEvent(QPickEvent *event)
{
    Q_Q(QObjectPicker);
    emit q->clicked(event);
    if (!event->isAccepted())
        propagateEvent(event, EventType::Clicked);
}

/*!
    \internal
 */
void QObjectPickerPrivate::movedEvent(QPickEvent *event)
{
    Q_Q(QObjectPicker);
    emit q->moved(event);
    if (!event->isAccepted())
        propagateEvent(event, EventType::Moved);
}

/*!
    \internal
 */
void QObjectPickerPrivate::releasedEvent(QPickEvent *event)
{
    Q_Q(QObjectPicker);
    if (m_acceptedLastPressedEvent) {
        emit q->released(event);
        q->setPressed(false);
    } else {
        event->setAccepted(false);
        propagateEvent(event, EventType::Released);
    }
}

} // Qt3DRender

QT_END_NAMESPACE
