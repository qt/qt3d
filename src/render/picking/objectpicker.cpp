/****************************************************************************
**
** Copyright (C) 2015 Paul Lemire paul.lemire350@gmail.com
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
****************************************************************************/

#include "objectpicker_p.h"
#include "qpickevent.h"
#include <Qt3DRender/qobjectpicker.h>
#include <Qt3DRender/private/qobjectpicker_p.h>
#include <Qt3DRender/qattribute.h>
#include <Qt3DRender/private/pickboundingvolumejob_p.h>
#include <Qt3DRender/private/qrenderaspect_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

ObjectPicker::ObjectPicker()
    : BackendNode(QBackendNode::ReadWrite)
    , m_priority(0)
    , m_isPressed(false)
    , m_hoverEnabled(false)
    , m_dragEnabled(false)
{
}

ObjectPicker::~ObjectPicker()
{
    notifyJob();
}

void ObjectPicker::cleanup()
{
    BackendNode::setEnabled(false);
    m_isPressed = false;
    m_hoverEnabled = false;
    m_dragEnabled = false;
    m_priority = 0;
    notifyJob();
}

void ObjectPicker::syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime)
{
    const QObjectPicker *node = qobject_cast<const QObjectPicker *>(frontEnd);
    if (!node)
        return;

    BackendNode::syncFromFrontEnd(frontEnd, firstTime);

    if (node->isHoverEnabled() != m_hoverEnabled) {
        m_hoverEnabled = node->isHoverEnabled();
        markDirty(AbstractRenderer::AllDirty);
        notifyJob();
    }

    if (node->isDragEnabled() != m_dragEnabled) {
        m_dragEnabled = node->isDragEnabled();
        markDirty(AbstractRenderer::AllDirty);
        notifyJob();
    }

    if (node->priority() != m_priority) {
        m_priority = node->priority();
        markDirty(AbstractRenderer::AllDirty);
        notifyJob();
    }
}

void ObjectPicker::notifyJob()
{
    if (m_renderer && m_renderer->aspect())
        QRenderAspectPrivate::get(m_renderer->aspect())->m_pickBoundingVolumeJob->markPickersDirty();
}

bool ObjectPicker::isPressed() const
{
    return m_isPressed;
}

bool ObjectPicker::isHoverEnabled() const
{
    return m_hoverEnabled;
}

bool ObjectPicker::isDragEnabled() const
{
    return m_dragEnabled;
}

void ObjectPicker::setPressed(bool pressed)
{
    m_isPressed = pressed;
}

void ObjectPicker::setPriority(int priority)
{
    m_priority = priority;
}

int ObjectPicker::priority() const
{
    return m_priority;
}

} // Render

} // Qt3DRender

QT_END_NAMESPACE
