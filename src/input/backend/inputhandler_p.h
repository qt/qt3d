/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
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

#ifndef QT3DINPUT_INPUT_INPUTHANDLER_P_H
#define QT3DINPUT_INPUT_INPUTHANDLER_P_H

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

#include <Qt3DInput/qt3dinput_global.h>
#include <Qt3DInput/private/handle_types_p.h>
#include <Qt3DCore/qaspectjob.h>
#include <QKeyEvent>
#include <QMutex>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {
class QEventFilterService;
}

namespace Qt3DInput {

class QInputDeviceIntegration;

namespace Input {

class KeyboardInputManager;
class KeyboardControllerManager;
class KeyboardEventFilter;
class MouseControllerManager;
class MouseInputManager;
class MouseEventFilter;
class AxisManager;
class AxisActionHandlerManager;
class ActionManager;
class AxisInputManager;
class AxisSettingManager;
class ActionInputManager;
class LogicalDeviceManager;

class InputHandler
{
public:
    InputHandler();

    void registerEventFilters(Qt3DCore::QEventFilterService *service);

    inline KeyboardControllerManager *keyboardControllerManager() const { return m_keyboardControllerManager; }
    inline KeyboardInputManager *keyboardInputManager() const  { return m_keyboardInputManager; }
    inline MouseControllerManager *mouseControllerManager() const { return m_mouseControllerManager; }
    inline MouseInputManager *mouseInputManager() const { return m_mouseInputManager; }
    inline AxisManager *axisManager() const { return m_axisManager; }
    inline ActionManager *actionManager() const { return m_actionManager; }
    inline AxisInputManager *axisInputManager() const { return m_axisInputManager; }
    inline AxisActionHandlerManager *axisActionHandlerManager() const { return m_axisActionHandlerManager; }
    inline AxisSettingManager *axisSettingManager() const { return m_axisSettingManager; }
    inline ActionInputManager *actionInputManager() const { return m_actionInputManager; }
    inline LogicalDeviceManager *logicalDeviceManager() const { return m_logicalDeviceManager; }

    void appendKeyEvent(const QT_PREPEND_NAMESPACE(QKeyEvent) &event);
    QList<QT_PREPEND_NAMESPACE(QKeyEvent)> pendingKeyEvents();
    void clearPendingKeyEvents();

    void appendMouseEvent(const QMouseEvent &event);
    QList<QMouseEvent> pendingMouseEvents();
    void clearPendingMouseEvents();

    void appendKeyboardController(HKeyboardController controller);
    void removeKeyboardController(HKeyboardController controller);

    void appendMouseController(HMouseController controller);
    void removeMouseController(HMouseController controller);

    QVector<Qt3DCore::QAspectJobPtr> keyboardJobs();
    QVector<Qt3DCore::QAspectJobPtr> mouseJobs();

    QVector<Qt3DInput::QInputDeviceIntegration *> inputDeviceIntegrations() const;
    void addInputDeviceIntegration(QInputDeviceIntegration *inputIntegration);

private:
    KeyboardControllerManager *m_keyboardControllerManager;
    KeyboardInputManager *m_keyboardInputManager;
    MouseControllerManager *m_mouseControllerManager;
    MouseInputManager *m_mouseInputManager;

    QVector<HKeyboardController> m_activeKeyboardControllers;
    QVector<HMouseController> m_activeMouseControllers;
    KeyboardEventFilter *m_keyboardEventFilter;
    QList<QT_PREPEND_NAMESPACE(QKeyEvent)> m_pendingEvents;
    MouseEventFilter *m_mouseEventFilter;
    QList<QMouseEvent> m_pendingMouseEvents;
    mutable QMutex m_mutex;

    AxisManager *m_axisManager;
    ActionManager *m_actionManager;
    AxisInputManager *m_axisInputManager;
    AxisActionHandlerManager *m_axisActionHandlerManager;
    AxisSettingManager *m_axisSettingManager;
    ActionInputManager *m_actionInputManager;
    LogicalDeviceManager *m_logicalDeviceManager;
    QVector<Qt3DInput::QInputDeviceIntegration *> m_inputDeviceIntegrations;
};

} // namespace Input
} // namespace Qt3DInput

QT_END_NAMESPACE

#endif // QT3DINPUT_INPUT_INPUTHANDLER_P_H
