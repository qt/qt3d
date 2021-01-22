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

#ifndef EVENTSOURCESETTERHELPER_H
#define EVENTSOURCESETTERHELPER_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of other Qt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QtCore/QMutex>
#include <QtCore/QObject>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {
class QEventFilterService;
}

namespace Qt3DInput {

namespace Input {

class InputHandler;

class EventSourceSetterHelper : public QObject
{
    Q_OBJECT
public:
    explicit EventSourceSetterHelper(InputHandler *inputHandler);

    // Called from aspect thread
    void setEventFilterService(Qt3DCore::QEventFilterService *service);

    // Called from main thread
    void setEventSource(QObject *eventSource);
    void unsetEventSource(QObject *eventSource);

private:
    Qt3DCore::QEventFilterService *m_service;
    InputHandler *m_inputHandler;
    QObject *m_lastEventSource;
    QMutex m_mutex;
};

} // Input

} // Qt3DInput

QT_END_NAMESPACE

#endif // EVENTSOURCESETTERHELPER_H
