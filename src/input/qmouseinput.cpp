/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
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

#include "qmouseinput.h"
#include "qmouseinput_p.h"
#include "qmousecontroller.h"
#include "q3dmouseevent.h"
#include <Qt3DCore/qbackendscenepropertychange.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

QMouseInputPrivate::QMouseInputPrivate()
    : QComponentPrivate()
    , m_controller(Q_NULLPTR)
    , m_containsMouse(false)
{
    m_shareable = false;
}

/*!
 * \class Qt3D::QMouseInput
 *
 * \brief Provides a mean of being notified about mouse events when attached to
 * a Qt3D::QMouseController instance.
 *
 * \since 5.5
 *
 * \note Qt3D::QMouseInput components shouldn't be shared, not respecting that
 * condition will most likely result in undefined behaviors.
 *
 * \sa Qt3D::QMouseController
 */

/*!
 * Constructs a new Qt3D::QMouseInput instance with parent \a parent.
 */
QMouseInput::QMouseInput(QNode *parent)
    : QComponent(*new QMouseInputPrivate, parent)
{
}

/*!
    \internal
*/
QMouseInput::QMouseInput(QMouseInputPrivate &dd, QNode *parent)
    : QComponent(dd, parent)
{
}

/*!
   Destroys this QMouseInput object
*/
QMouseInput::~QMouseInput()
{
    QNode::cleanup();
}

/*!
 * Sets the mouse controller of the Qt3D::QMouseInput instance to \a controller.
 */
void QMouseInput::setController(QMouseController *controller)
{
    Q_D(QMouseInput);
    if (d->m_controller != controller) {
        d->m_controller = controller;
        emit controllerChanged();
    }
}

/*!
 * Returns the current mouse controller of the QMouseInput instance.
 */
QMouseController *QMouseInput::controller() const
{
    Q_D(const QMouseInput);
    return d->m_controller;
}

/*!
 * Returns true if the QMouseInput currently contains the mouse.
 *
 * \note: In this context, contains mean that the ray originating from the
 * mouse is intersecting with the Qt3D::QEntity that aggregates the current
 * Qt3D::QMouseInput instance component.
 */
bool QMouseInput::containsMouse() const
{
    Q_D(const QMouseInput);
    return d->m_containsMouse;
}

void QMouseInput::copy(const QNode *ref)
{
    Q_D(QMouseInput);
    const QMouseInput *refInput = static_cast<const QMouseInput *>(ref);
    d->m_containsMouse = refInput->containsMouse();

    // TODO: We may want to store the controller id and only send a clone when we are the parent
    // of the controller.
    // Perhaps it's time to investigate sending a "kernel" or "seed" over to the backend rather
    // than a complete clone.
    if (refInput && refInput->controller()->parent() == ref)
        d->m_controller = static_cast<QMouseController *>(QNode::clone(refInput->controller()));
}

void QMouseInput::sceneChangeEvent(const QSceneChangePtr &change)
{
    QBackendScenePropertyChangePtr e = qSharedPointerCast<QBackendScenePropertyChange>(change);
    if (e->type() == NodeUpdated) {
        if (e->propertyName() == QByteArrayLiteral("mouse")) {
            Q3DMouseEventPtr ev = e->value().value<Q3DMouseEventPtr>();
            mouseEvent(ev.data());
        } else if (e->propertyName() == QByteArrayLiteral("wheel")) {
            Q3DWheelEventPtr ev = e->value().value<Q3DWheelEventPtr>();
            emit wheel(ev.data());
        }
    }
}

void QMouseInput::mouseEvent(Q3DMouseEvent *event)
{
    switch (event->type()) {
    case QEvent::MouseButtonPress:
        if (event->wasHeld())
            emit pressAndHold(event);
        else
            emit pressed(event);
        break;
    case QEvent::MouseButtonRelease:
        emit released(event);
        break;
    case Qt::TapGesture:
        emit clicked(event);
        break;
    case QEvent::MouseButtonDblClick:
        emit doubleClicked(event);
        break;
    case QEvent::MouseMove:
        emit positionChanged(event);
        break;
    default:
        break;
    }
}

void QMouseInput::setContainsMouse(bool contains)
{
    Q_D(QMouseInput);
    if (contains != d->m_containsMouse) {
        d->m_containsMouse = contains;
        emit containsMouseChanged();
    }
}

} // Qt3D

QT_END_NAMESPACE
