// Copyright (C) 2015 Paul Lemire paul.lemire350@gmail.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "objectpicker_p.h"
#include "qpickevent.h"
#include <Qt3DRender/qobjectpicker.h>
#include <Qt3DRender/private/qobjectpicker_p.h>
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

    if (firstTime) {
        markDirty(AbstractRenderer::AllDirty);
        notifyJob();
    }

    if (isEnabled() != node->isEnabled()) {
        markDirty(AbstractRenderer::AllDirty);
        // We let QBackendNode::syncFromFrontEnd change the enabled property
    }

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

    BackendNode::syncFromFrontEnd(frontEnd, firstTime);
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
