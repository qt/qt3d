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

#include "qeventforward.h"
#include "qeventforward_p.h"
#include "posteventstofrontend_p.h"

#include <Qt3DCore/qpropertyupdatedchange.h>
#include <QtCore/qcoreapplication.h>

#include <QtGui/qevent.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

/*!
    \class Qt3DRender::QEventForward
    \inmodule Qt3DRender

    \brief The QEventForward class instantiates a node that can
    be used to forward events from the scene.

    An QEventForward class can be used to forward pick events and keyboard events
    from entities having a QObjectPicker. When the QObjectPicker handles pick event,
    it hands it to the QEventForward, which converts it to QMouseEvent. The event
    mouse coordinates are calculated by interpolating the given coordinate attribute
    on the position the pick ray intersected the triangle, and then multiplying the
    interpolated coordinate with the given transformation matrix. The event is then
    sent and to the target object.

    \since 5.9
*/

/*!
    \qmltype EventForward
    \instantiates Qt3DRender::QEventForward
    \inherits Node
    \inqmlmodule Qt3D.Render
    \brief The EventForward type instantiates a node that can
    be used to forward events from the scene.

    An EventForward type can be used to forward pick events and keyboard events
    from entities having a ObjectPicker. When the ObjectPicker handles pick event,
    it hands it to the EventForward, which converts it to MouseEvent.  The event
    mouse coordinates are calculated by interpolating the given coordinate attribute
    on the position the pick ray intersected the triangle, and then multiplying the
    interpolated coordinate with the given transformation matrix. The event is then
    sent and to the target object.

 */

/*!
    \qmlproperty Object Qt3D.Render::EventForward::target
    Holds the object the events are sent to.
*/
/*!
    \qmlproperty bool Qt3D.Render::EventForward::forwardMouseEvents
    Holds whether mouse event forwarding is enabled.
*/
/*!
    \qmlproperty bool Qt3D.Render::EventForward::forwardKeyboardEvents
    Holds whether keyboard event forwarding is enabled.
*/
/*!
    \qmlproperty Matrix4x4 Qt3D.Render::EventForward::coordinateTransform
    Holds the coordinate transformation.
*/
/*!
    \qmlproperty string Qt3D.Render::EventForward::coordinateAttribute
    Holds the name of the coordinate attribute the mouse coordinates are calculated from.
*/
/*!
    \qmlproperty bool Qt3D.Render::EventForward::focus
    Holds whether the QObjectPicker has focus.
*/

/*!
    \property QEventForward::target
    Holds the object the events are sent to.
*/
/*!
    \property QEventForward::forwardMouseEvents
    Holds whether mouse event forwarding is enabled.
*/
/*!
    \property QEventForward::forwardKeyboardEvents
    Holds whether keyboard event forwarding is enabled.
*/
/*!
    \property QEventForward::coordinateTransform
    Holds the coordinate transformation.
*/
/*!
    \property QEventForward::coordinateAttribute
    Holds the name of the coordinate attribute the mouse coordinates are calculated from.
*/
/*!
    \property QEventForward::focus
    Holds whether the ObjectPicker has focus.
 */

/*!

 */
QEventForward::QEventForward(Qt3DCore::QNode *parent)
    : Qt3DCore::QNode(*new QEventForwardPrivate(), parent)
{
}

/*! \internal */
QEventForward::~QEventForward()
{
}

QObject *QEventForward::target() const
{
    Q_D(const QEventForward);
    return d->m_target;
}

void QEventForward::targetDestroyed(QObject *target)
{
    Q_D(QEventForward);
    Q_ASSERT(target == d->m_target);
    setTarget(nullptr);
}

void QEventForward::setTarget(QObject *target)
{
    Q_D(QEventForward);
    if (target != d->m_target) {
        if (d->m_target != nullptr)
            QObject::disconnect(d->m_destructionConnection);
        d->m_target = target;
        if (d->m_target) {
            d->m_destructionConnection = QObject::connect(target, &QObject::destroyed,
                                                          this, &QEventForward::targetDestroyed);
        }
        emit targetChanged(target);
    }
}

QMatrix4x4 QEventForward::coordinateTransform() const
{
    Q_D(const QEventForward);
    return d->m_transform;
}

void QEventForward::setCoordinateTransform(const QMatrix4x4 &coordinateTransform)
{
    Q_D(QEventForward);
    if (coordinateTransform != d->m_transform) {
        d->m_transform = coordinateTransform;
        emit coordinateTransformChanged(coordinateTransform);
    }
}

QString QEventForward::coordinateAttribute() const
{
    Q_D(const QEventForward);
    return d->m_attribute;
}

void QEventForward::setCoordinateAttribute(const QString &coordinateAttribute)
{
    Q_D(QEventForward);
    if (coordinateAttribute != d->m_attribute) {
        d->m_attribute = coordinateAttribute;
        emit coordinateAttributeChanged(coordinateAttribute);
    }
}

bool QEventForward::forwardMouseEvents() const
{
    Q_D(const QEventForward);
    return d->m_forwardMouseEvents;
}

void QEventForward::setForwardMouseEvents(bool forward)
{
    Q_D(QEventForward);
    if (forward != d->m_forwardMouseEvents) {
        d->m_forwardMouseEvents = forward;
        emit forwardMouseEventsChanged(forward);
    }
}

bool QEventForward::forwardKeyboardEvents() const
{
    Q_D(const QEventForward);
    return d->m_forwardKeyboardEvents;
}

void QEventForward::setForwardKeyboardEvents(bool forward)
{
    Q_D(QEventForward);
    if (forward != d->m_forwardKeyboardEvents) {
        d->m_forwardKeyboardEvents = forward;
        emit forwardKeyboardEventsChanged(forward);
    }
}

bool QEventForward::focus() const
{
    Q_D(const QEventForward);
    return d->m_focus;
}

void QEventForward::setFocus(bool focus)
{
    Q_D(QEventForward);
    if (d->m_focus != focus) {
        d->m_focus = focus;
        emit focusChanged(focus);
    }
}

/*!
    \internal
 */
Qt3DCore::QNodeCreatedChangeBasePtr QEventForward::createNodeCreationChange() const
{
    auto creationChange = Qt3DCore::QNodeCreatedChangePtr<QEventForwardData>::create(this);
    auto &data = creationChange->data;
    Q_D(const QEventForward);
    data.target = d->m_target;
    data.coordinateTransform = d->m_transform;
    data.coordinateAttribute = d->m_attribute;
    data.forwardMouseEvents = d->m_forwardMouseEvents;
    data.forwardKeyboardEvents = d->m_forwardKeyboardEvents;
    data.focus = d->m_focus;
    return creationChange;
}

void QEventForward::sceneChangeEvent(const Qt3DCore::QSceneChangePtr &change)
{
    Q_D(QEventForward);
    Qt3DCore::QPropertyUpdatedChangePtr e
            = qSharedPointerCast<Qt3DCore::QPropertyUpdatedChange>(change);
    if (e->type() == Qt3DCore::PropertyUpdated && d->m_target != nullptr) {
        if (e->propertyName() == QByteArrayLiteral("events")) {
            PostEventsToFrontendPtr postedEvents = e->value().value<PostEventsToFrontendPtr>();
            for (QEvent *event : postedEvents->events())
                QCoreApplication::sendEvent(d->m_target, event);
        }
    }
}

} // Qt3DRender

QT_END_NAMESPACE
