/****************************************************************************
**
** Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
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

#include "eventsourcesetterhelper_p.h"

#include <Qt3DInput/private/inputhandler_p.h>
#include <Qt3DCore/private/qeventfilterservice_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {

namespace Input {

EventSourceSetterHelper::EventSourceSetterHelper(InputHandler *inputHandler)
    : QObject()
    , m_service(nullptr)
    , m_inputHandler(inputHandler)
    , m_lastEventSource(nullptr)
{
}

// Main thread
void EventSourceSetterHelper::setEventFilterService(Qt3DCore::QEventFilterService *service)
{
    m_service = service;
}

// Main thread
void EventSourceSetterHelper::setEventSource(QObject *eventSource)
{
    if (eventSource && m_lastEventSource != eventSource) {
        if (m_service) {
            m_service->initialize(eventSource);
            m_inputHandler->registerEventFilters(m_service);
            m_lastEventSource = eventSource;
        }
    }
}

void EventSourceSetterHelper::unsetEventSource(QObject *eventSource)
{
    if (m_service) {
        m_inputHandler->unregisterEventFilters(m_service);
        if (eventSource)
            m_service->shutdown(eventSource);
    }
}

} // Input

} // Qt3DInput

QT_END_NAMESPACE
