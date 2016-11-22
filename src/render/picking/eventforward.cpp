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

#include <Qt3DRender/qeventforward.h>
#include <Qt3DRender/qattribute.h>
#include <Qt3DCore/qpropertyupdatedchange.h>

#include <private/qeventforward_p.h>
#include <private/eventforward_p.h>
#include <private/posteventstofrontend_p.h>

#include <QtGui/qevent.h>
#include <QtCore/qcoreapplication.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

EventForward::EventForward()
    : BackendNode(QBackendNode::ReadWrite)
    , m_forwardMouseEvents(false)
    , m_forwardKeyboardEvents(false)
    , m_focus(false)
{

}

EventForward::~EventForward()
{

}

void EventForward::cleanup()
{
    setEnabled(false);
    m_coordinateAttribute = "";
    m_coordinateTransform.setToIdentity();
    m_forwardMouseEvents = false;
    m_forwardKeyboardEvents = false;
    m_focus = false;
}

QString EventForward::coordinateAttribute() const
{
    return m_coordinateAttribute;
}

QMatrix4x4 EventForward::coordinateTransform() const
{
    return m_coordinateTransform;
}

bool EventForward::forwardMouseEvents() const
{
    return m_forwardMouseEvents;
}

bool EventForward::forwardKeyboardEvents() const
{
    return m_forwardKeyboardEvents;
}

bool EventForward::focus() const
{
    return m_focus;
}

void EventForward::setCoordinateTransform(const QMatrix4x4 &transform)
{
    m_coordinateTransform = transform;
}

void EventForward::setCoordinateAttribute(const QString &attribute)
{
    m_coordinateAttribute = attribute;
}

void EventForward::setForwardMouseEvents(bool enabled)
{
    m_forwardMouseEvents = enabled;
}

void EventForward::setForwardKeyboardEvents(bool enabled)
{
    m_forwardKeyboardEvents = enabled;
}

void EventForward::setFocus(bool focus)
{
    m_focus = focus;
}

void EventForward::initializeFromPeer(const Qt3DCore::QNodeCreatedChangeBasePtr &change)
{
    const auto typedChange = qSharedPointerCast<Qt3DCore::QNodeCreatedChange<QEventForwardData>>(change);
    const auto &data = typedChange->data;
    setCoordinateAttribute(data.coordinateAttribute);
    setCoordinateTransform(data.coordinateTransform);
    setForwardMouseEvents(data.forwardMouseEvents);
    setForwardKeyboardEvents(data.forwardKeyboardEvents);
    setFocus(data.focus);
}

void EventForward::sceneChangeEvent(const Qt3DCore::QSceneChangePtr &e)
{
    if (e->type() == Qt3DCore::PropertyUpdated) {
        const Qt3DCore::QPropertyUpdatedChangePtr propertyChange
                = qSharedPointerCast<Qt3DCore::QPropertyUpdatedChange>(e);

        if (propertyChange->propertyName() == QByteArrayLiteral("coordinateTransform"))
            setCoordinateTransform(propertyChange->value().value<QMatrix4x4>());
        else if (propertyChange->propertyName() == QByteArrayLiteral("coordinateAttribute"))
            setCoordinateAttribute(propertyChange->value().toString());
        else if (propertyChange->propertyName() == QByteArrayLiteral("forwardMouseEvents"))
            setForwardMouseEvents(propertyChange->value().toBool());
        else if (propertyChange->propertyName() == QByteArrayLiteral("forwardKeyboardEvents"))
            setForwardKeyboardEvents(propertyChange->value().toBool());
        else if (propertyChange->propertyName() == QByteArrayLiteral("focus"))
            setFocus(propertyChange->value().toBool());
    }

    BackendNode::sceneChangeEvent(e);
}

void EventForward::forward(const QMouseEvent &event, const QVector4D &coordinate)
{
    const QVector4D transformedCoordinate = m_coordinateTransform * coordinate;
    const QPointF local = QPointF(transformedCoordinate.x(), transformedCoordinate.y());
    QMouseEvent *mouseEvent = new QMouseEvent(event.type(), local, local, local, event.button(),
                                      event.buttons(), event.modifiers(),
                                      Qt::MouseEventSynthesizedByApplication);

    PostEventsToFrontendPtr events = PostEventsToFrontendPtr::create(mouseEvent);

    auto e = Qt3DCore::QPropertyUpdatedChangePtr::create(peerId());
    e->setDeliveryFlags(Qt3DCore::QSceneChange::DeliverToAll);
    e->setPropertyName("events");
    e->setValue(QVariant::fromValue(events));
    notifyObservers(e);
}

void EventForward::forward(const QList<QKeyEvent> &keyEvents)
{
    QVector<QEvent *> eventsToSend;
    for (const QKeyEvent &e : keyEvents) {
        QKeyEvent *keyEvent = new QKeyEvent(e.type(), e.key(), e.modifiers(), e.nativeScanCode(),
                                      e.nativeVirtualKey(), e.nativeModifiers());
        eventsToSend.append(keyEvent);
    }
    PostEventsToFrontendPtr events = PostEventsToFrontendPtr::create(eventsToSend);
    auto e = Qt3DCore::QPropertyUpdatedChangePtr::create(peerId());
    e->setDeliveryFlags(Qt3DCore::QSceneChange::DeliverToAll);
    e->setPropertyName("events");
    e->setValue(QVariant::fromValue(events));
    notifyObservers(e);
}

} // Render
} // Qt3DRender

QT_END_NAMESPACE
