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

#include "qobjectpicker.h"
#include <Qt3DCore/private/qcomponent_p.h>
#include <Qt3DCore/qbackendscenepropertychange.h>
#include <Qt3DRender/qattribute.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

/*!
    \class Qt3DRender::QObjectPicker

    \brief The Qt3DRender::QObjectPicker class instantiates a component that can
    be used to interact with a Qt3DCore::QEntity by a process known as picking.

    The signals pressed(), released(), clicked(), entered(), exited() are
    emitted when the bounding volume defined by the pickAttribute property intersects
    with a ray.

    \note Instances of this component shouldn't be shared, not respecting that
    condition will most likely result in undefined behaviors.

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
    \qmlsignal Qt3D.Render::ObjectPicker::entered()
*/

/*!
    \qmlsignal Qt3D.Render::ObjectPicker::exited()
*/

/*!
    \internal
*/
class QObjectPickerPrivate : public Qt3DCore::QComponentPrivate
{
public:
    QObjectPickerPrivate()
        : QComponentPrivate()
        , m_pickAttribute(Q_NULLPTR)
        , m_hoverEnabled(false)
        , m_pressed(false)
        , m_containsMouse(false)
    {}

    Q_DECLARE_PUBLIC(QObjectPicker)
    Qt3DRender::QAttribute *m_pickAttribute;
    bool m_hoverEnabled;
    bool m_pressed;
    bool m_containsMouse;
};

QObjectPicker::QObjectPicker(Qt3DCore::QNode *parent)
    : Qt3DCore::QComponent(*new QObjectPickerPrivate(), parent)
{
}

QObjectPicker::~QObjectPicker()
{
    QComponent::cleanup();
}

void QObjectPicker::setPickAttribute(QAttribute *pickAttribute)
{
    Q_D(QObjectPicker);
    if (pickAttribute != d->m_pickAttribute) {

        if (pickAttribute && !pickAttribute->parent())
            pickAttribute->setParent(this);

        d->m_pickAttribute = pickAttribute;
        emit pickAttributeChanged();
    }
}

/*!
    \qmlproperty Qt3D.Render.Attribute Qt3D.Render::ObjectPicker::pickAttribute
*/
QAttribute *QObjectPicker::pickAttribute() const
{
    Q_D(const QObjectPicker);
    return d->m_pickAttribute;
}

void QObjectPicker::setHoverEnabled(bool hoverEnabled)
{
    Q_D(QObjectPicker);
    if (hoverEnabled != d->m_hoverEnabled) {
        d->m_hoverEnabled = hoverEnabled;
        emit hoverEnabledChanged();
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
    // Only clone the pickAttribute if we are it's parent, will be deleted by its parent otherwise
    if (picker->d_func()->m_pickAttribute != Q_NULLPTR && picker->d_func()->m_pickAttribute->parent() == ref)
        setPickAttribute(qobject_cast<QAttribute *>(QNode::clone(picker->d_func()->m_pickAttribute)));
}

void QObjectPicker::sceneChangeEvent(const Qt3DCore::QSceneChangePtr &change)
{
    Qt3DCore::QBackendScenePropertyChangePtr e = qSharedPointerCast<Qt3DCore::QBackendScenePropertyChange>(change);
    if (e->type() == Qt3DCore::NodeUpdated) {
        // TO DO: Complete this part
        // to emit the correct signals
    }
}

void QObjectPicker::setPressed(bool pressed)
{
    Q_D(QObjectPicker);
    if (d->m_pressed != pressed) {
        const bool blocked = blockNotifications(true);
        d->m_pressed = pressed;
        emit pressedChanged();
        blockNotifications(blocked);
    }
}

void QObjectPicker::setContainsMouse(bool containsMouse)
{
    Q_D(QObjectPicker);
    if (d->m_containsMouse != containsMouse) {
        const bool blocked = blockNotifications(true);
        d->m_containsMouse = containsMouse;
        emit containsMouseChanged();
        blockNotifications(blocked);
    }
}

} // Qt3DRender

QT_END_NAMESPACE
