/****************************************************************************
**
** Copyright (C) 2015 Paul Lemire paul.lemire350@gmail.com
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

#include "objectpicker_p.h"
#include <Qt3DRender/qobjectpicker.h>
#include <Qt3DRender/qattribute.h>
#include <Qt3DCore/qscenepropertychange.h>
#include <Qt3DCore/qbackendscenepropertychange.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

ObjectPicker::ObjectPicker()
    : QBackendNode(QBackendNode::ReadWrite)
    , m_isDirty(false)
    , m_hoverEnabled(false)
{
}

ObjectPicker::~ObjectPicker()
{
}

void ObjectPicker::cleanup()
{
    m_isDirty = false;
    m_hoverEnabled = false;
}

void ObjectPicker::updateFromPeer(Qt3DCore::QNode *peer)
{
    QObjectPicker *picker = static_cast<QObjectPicker *>(peer);
    if (picker) {
        m_hoverEnabled = picker->hoverEnabled();
        m_isDirty = true;
    }
}

void ObjectPicker::sceneChangeEvent(const Qt3DCore::QSceneChangePtr &e)
{
    const Qt3DCore::QScenePropertyChangePtr propertyChange = qSharedPointerCast<Qt3DCore::QScenePropertyChange>(e);
    const QByteArray propertyName = propertyChange->propertyName();

    if (propertyChange->type() == Qt3DCore::NodeUpdated) {
        if (propertyName == QByteArrayLiteral("hoverEnabled")) {
            m_hoverEnabled = propertyChange->value().toBool();
            m_isDirty = true;
        }
    }
}

bool ObjectPicker::isDirty() const
{
    return m_isDirty;
}

void ObjectPicker::unsetDirty()
{
    m_isDirty = false;
}

void ObjectPicker::makeDirty()
{
    m_isDirty = true;
}

bool ObjectPicker::hoverEnabled() const
{
    return m_hoverEnabled;
}

void ObjectPicker::onClicked()
{
    Qt3DCore::QBackendScenePropertyChangePtr e(new Qt3DCore::QBackendScenePropertyChange(Qt3DCore::NodeUpdated, peerUuid()));
    e->setPropertyName("clicked");
    e->setTargetNode(peerUuid());
    notifyObservers(e);
}

void ObjectPicker::onPressed()
{
    Qt3DCore::QBackendScenePropertyChangePtr e(new Qt3DCore::QBackendScenePropertyChange(Qt3DCore::NodeUpdated, peerUuid()));
    e->setPropertyName("pressed");
    e->setTargetNode(peerUuid());
    notifyObservers(e);
}

void ObjectPicker::onReleased()
{
    Qt3DCore::QBackendScenePropertyChangePtr e(new Qt3DCore::QBackendScenePropertyChange(Qt3DCore::NodeUpdated, peerUuid()));
    e->setPropertyName("released");
    e->setTargetNode(peerUuid());
    notifyObservers(e);
}

void ObjectPicker::onEntered()
{
    Qt3DCore::QBackendScenePropertyChangePtr e(new Qt3DCore::QBackendScenePropertyChange(Qt3DCore::NodeUpdated, peerUuid()));
    e->setPropertyName("entered");
    e->setTargetNode(peerUuid());
    notifyObservers(e);
}

void ObjectPicker::onExited()
{
    Qt3DCore::QBackendScenePropertyChangePtr e(new Qt3DCore::QBackendScenePropertyChange(Qt3DCore::NodeUpdated, peerUuid()));
    e->setPropertyName("exited");
    e->setTargetNode(peerUuid());
    notifyObservers(e);
}

} // Render

} // Qt3DRender

QT_END_NAMESPACE
