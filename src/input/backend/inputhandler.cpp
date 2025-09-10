// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "inputhandler_p.h"

#include <Qt3DCore/private/qscene_p.h>
#include <Qt3DInput/qmousehandler.h>
#include <Qt3DInput/qkeyboardhandler.h>
#include <Qt3DInput/private/assignkeyboardfocusjob_p.h>
#include <Qt3DInput/private/inputmanagers_p.h>
#include <Qt3DInput/private/inputsettings_p.h>
#include <Qt3DInput/private/qinputdeviceintegration_p.h>
#include <Qt3DInput/private/qmousehandler_p.h>
#include <Qt3DInput/private/qkeyboardhandler_p.h>
#include <Qt3DCore/private/qeventfilterservice_p.h>


QT_BEGIN_NAMESPACE

namespace Qt3DInput {
namespace Input {

using namespace Qt3DCore;

class InternalEventFilter : public QObject
{
public:
    explicit InternalEventFilter(QObject *parent = nullptr) : QObject(parent), m_inputHandler(nullptr) { }
    ~InternalEventFilter() override;

    void setInputHandler(Qt3DInput::Input::InputHandler *handler) { m_inputHandler = handler; }
    inline Qt3DInput::Input::InputHandler *inputHandler() const { return m_inputHandler; }

protected:
    bool eventFilter(QObject *obj, QEvent *e) override {
        switch (e->type()) {
        case QEvent::MouseMove:
        case QEvent::MouseButtonPress:
        case QEvent::MouseButtonRelease:
        case QEvent::MouseButtonDblClick:
            return processMouseEvent(obj, static_cast<QT_PREPEND_NAMESPACE(QMouseEvent) *>(e));
        case QEvent::HoverMove: {
            const QHoverEvent *he = static_cast<QHoverEvent *>(e);
            auto mouseEvent = QT_PREPEND_NAMESPACE(QMouseEvent)(QEvent::MouseMove,
                                                                he->position(),
                                                                he->globalPosition(),
                                                                Qt::NoButton, Qt::NoButton,
                                                                he->modifiers());
            return processMouseEvent(obj, static_cast<QT_PREPEND_NAMESPACE(QMouseEvent) *>(&mouseEvent));
        }
#if QT_CONFIG(wheelevent)
        case QEvent::Wheel:
            return processWheelEvent(obj, static_cast<QT_PREPEND_NAMESPACE(QWheelEvent) *>(e));
#endif
        case QEvent::KeyPress:
        case QEvent::KeyRelease:
            return processKeyEvent(obj, static_cast<QT_PREPEND_NAMESPACE(QKeyEvent) *>(e));
        default:
            break;
        }
        return false;
    }

    bool processMouseEvent(QObject *obj, QT_PREPEND_NAMESPACE(QMouseEvent) *event);
#if QT_CONFIG(wheelevent)
    bool processWheelEvent(QObject *obj, QT_PREPEND_NAMESPACE(QWheelEvent) *event);
#endif
    bool processKeyEvent(QObject *obj, QT_PREPEND_NAMESPACE(QKeyEvent) *event);

private:
    Qt3DInput::Input::InputHandler *m_inputHandler;
};


InputHandler::InputHandler()
    : m_keyboardDeviceManager(new KeyboardDeviceManager())
    , m_keyboardInputManager(new KeyboardInputManager())
    , m_mouseDeviceManager(new MouseDeviceManager())
    , m_mouseInputManager(new MouseInputManager())
    , m_eventFilter(new InternalEventFilter())
    , m_axisManager(new AxisManager())
    , m_axisAccumulatorManager(new AxisAccumulatorManager())
    , m_actionManager(new ActionManager())
    , m_axisSettingManager(new AxisSettingManager())
    , m_actionInputManager(new ActionInputManager())
    , m_analogAxisInputManager(new AnalogAxisInputManager())
    , m_buttonAxisInputManager(new ButtonAxisInputManager())
    , m_inputChordManager(new InputChordManager())
    , m_inputSequenceManager(new InputSequenceManager())
    , m_logicalDeviceManager(new LogicalDeviceManager())
    , m_genericPhysicalDeviceBackendNodeManager(new GenericDeviceBackendNodeManager)
    , m_physicalDeviceProxyManager(new PhysicalDeviceProxyManager())
    , m_settings(nullptr)
    , m_service(nullptr)
    , m_lastEventSource(nullptr)
{
    m_eventFilter->setInputHandler(this);
}

InputHandler::~InputHandler()
{
    delete m_keyboardDeviceManager;
    delete m_keyboardInputManager;
    delete m_mouseDeviceManager;
    delete m_mouseInputManager;
    delete m_eventFilter;
    delete m_axisManager;
    delete m_axisAccumulatorManager;
    delete m_actionManager;
    delete m_axisSettingManager;
    delete m_analogAxisInputManager;
    delete m_buttonAxisInputManager;
    delete m_actionInputManager;
    delete m_inputChordManager;
    delete m_inputSequenceManager;
    delete m_logicalDeviceManager;
    delete m_genericPhysicalDeviceBackendNodeManager;
    delete m_physicalDeviceProxyManager;
}

// Called in MainThread (by the EventSourceHelperSetter)
void InputHandler::registerEventFilters()
{
    if (m_service)
        m_service->registerEventFilter(m_eventFilter, 512);
}

void InputHandler::unregisterEventFilters()
{
    if (m_service)
        m_service->unregisterEventFilter(m_eventFilter);
}

void InputHandler::setInputSettings(InputSettings *settings)
{
    if (m_settings && settings == nullptr) {
        unregisterEventFilters();
        m_lastEventSource = nullptr;
        if (m_settings->eventSource() && m_service)
            m_service->shutdown(m_settings->eventSource());
    }
    if (m_service) {
        unregisterEventFilters();
        if (m_settings && m_settings->eventSource())
            m_service->shutdown(m_settings->eventSource());
    }
    m_settings = settings;
}

void InputHandler::updateEventSource()
{
    // Called every frame from input aspect
    // Should probably just listen to changes in source property on settings object
    if (!m_settings || !m_service)
        return;

    // Will be updated only if eventSource is different than
    // what was set last
    QObject *eventSource = m_settings->eventSource();
    if (eventSource && m_lastEventSource != eventSource) {
        m_service->initialize(eventSource);
        registerEventFilters();
        m_lastEventSource = eventSource;
    }
}

void InputHandler::appendKeyboardDevice(HKeyboardDevice device)
{
    m_activeKeyboardDevices.append(device);
}

void InputHandler::removeKeyboardDevice(HKeyboardDevice device)
{
    m_activeKeyboardDevices.removeAll(device);
}

void InputHandler::appendMouseDevice(HMouseDevice device)
{
    m_activeMouseDevices.append(device);
}

void InputHandler::removeMouseDevice(HMouseDevice device)
{
    m_activeMouseDevices.removeAll(device);
}

void Qt3DInput::Input::InputHandler::appendGenericDevice(HGenericDeviceBackendNode device)
{
    m_activeGenericPhysicalDevices.append(device);
}

void Qt3DInput::Input::InputHandler::removeGenericDevice(HGenericDeviceBackendNode device)
{
    m_activeGenericPhysicalDevices.removeAll(device);
}

// called every frame to reset the
void Qt3DInput::Input::InputHandler::resetMouseAxisState()
{
    for (const HMouseDevice &cHandle : std::as_const(m_activeMouseDevices)) {
        MouseDevice *controller = m_mouseDeviceManager->data(cHandle);
        controller->resetMouseAxisState();
    }
}

QList<QInputDeviceIntegration *> InputHandler::inputDeviceIntegrations() const
{
    return m_inputDeviceIntegrations;
}

void InputHandler::addInputDeviceIntegration(QInputDeviceIntegration *inputIntegration)
{
    m_inputDeviceIntegrations.push_back(inputIntegration);
}

QAbstractPhysicalDevice *Qt3DInput::Input::InputHandler::createPhysicalDevice(const QString &name)
{
    QAbstractPhysicalDevice *device = nullptr;
    for (Qt3DInput::QInputDeviceIntegration *integration : std::as_const(m_inputDeviceIntegrations)) {
        if ((device = integration->createPhysicalDevice(name)) != nullptr)
            break;
    }
    return device;
}

void Qt3DInput::Input::InputHandler::setEventFilterService(QEventFilterService *service)
{
    m_service = service;
}

AbstractActionInput *InputHandler::lookupActionInput(Qt3DCore::QNodeId id) const
{
    AbstractActionInput *input = nullptr;
    if ((input = actionInputManager()->lookupResource(id)) != nullptr)
        return input;
    if ((input = inputSequenceManager()->lookupResource(id)) != nullptr)
        return input;
    return inputChordManager()->lookupResource(id); // nullptr if not found
}

InternalEventFilter::~InternalEventFilter() = default;

bool InternalEventFilter::processMouseEvent(QObject *obj, QT_PREPEND_NAMESPACE(QMouseEvent) *event)
{
    Q_UNUSED(obj);
    Q_ASSERT(m_inputHandler);
    if (!m_inputHandler->m_scene)
        return false;

    for (const HMouseDevice &cHandle : std::as_const(m_inputHandler->m_activeMouseDevices)) {
        MouseDevice *controller = m_inputHandler->m_mouseDeviceManager->data(cHandle);

        controller->updateMouseEvent(event);

        // Send the events to the mouse handlers that have for sourceDevice controller
        const std::vector<HMouseHandler> &activeMouseHandlers = m_inputHandler->m_mouseInputManager->activeHandles();
        for (const HMouseHandler &mouseHandlerHandle : activeMouseHandlers) {
            MouseHandler *mouseHandler = m_inputHandler->m_mouseInputManager->data(mouseHandlerHandle);
            Q_ASSERT(mouseHandler);

            if (mouseHandler->mouseDevice() == controller->peerId()) {
                QMouseHandler *node = qobject_cast<QMouseHandler *>(m_inputHandler->m_scene->lookupNode(mouseHandler->peerId()));
                if (node) {
                    QMouseHandlerPrivate *dnode = static_cast<QMouseHandlerPrivate *>(QMouseHandlerPrivate::get(node));
                    dnode->mouseEvent(QMouseEventPtr::create(*event));  // Do we really need Qt3D specific events?
                }
            }
        }
    }

    return false;
}

#if QT_CONFIG(wheelevent)
bool InternalEventFilter::processWheelEvent(QObject *obj, QT_PREPEND_NAMESPACE(QWheelEvent) *event)
{
    Q_UNUSED(obj);
    Q_ASSERT(m_inputHandler);
    if (!m_inputHandler->m_scene)
        return false;

    for (const HMouseDevice &cHandle : std::as_const(m_inputHandler->m_activeMouseDevices)) {
        MouseDevice *controller = m_inputHandler->m_mouseDeviceManager->data(cHandle);

        controller->updateWheelEvent(event);

        // Send the events to the mouse handlers that have for sourceDevice controller
        const std::vector<HMouseHandler> &activeMouseHandlers = m_inputHandler->m_mouseInputManager->activeHandles();
        for (const HMouseHandler &mouseHandlerHandle : activeMouseHandlers) {
            MouseHandler *mouseHandler = m_inputHandler->m_mouseInputManager->data(mouseHandlerHandle);
            Q_ASSERT(mouseHandler);

            if (mouseHandler->mouseDevice() == controller->peerId()) {
                QMouseHandler *node = qobject_cast<QMouseHandler *>(m_inputHandler->m_scene->lookupNode(mouseHandler->peerId()));
                QWheelEvent we(*event);
                node->wheel(&we);  // Do we really need Qt3D specific events?
            }
        }
    }

    return false;
}
#endif

bool InternalEventFilter::processKeyEvent(QObject *obj, QT_PREPEND_NAMESPACE(QKeyEvent) *event)
{
    Q_UNUSED(obj);
    Q_ASSERT(m_inputHandler);
    if (!m_inputHandler->m_scene)
        return false;

    for (const HKeyboardDevice &cHandle : std::as_const(m_inputHandler->m_activeKeyboardDevices)) {
        KeyboardDevice *keyboardDevice = m_inputHandler->m_keyboardDeviceManager->data(cHandle);
        if (keyboardDevice) {
            keyboardDevice->updateKeyEvent(event);

            // update the focus
            if (keyboardDevice->lastKeyboardInputRequester() != keyboardDevice->currentFocusItem()) {
                const auto handles = m_inputHandler->keyboardInputManager()->activeHandles();
                for (const HKeyboardHandler &handle : handles) {
                    KeyboardHandler *input = m_inputHandler->keyboardInputManager()->data(handle);
                    Q_ASSERT(input);
                    if (input->keyboardDevice() == keyboardDevice->peerId()) {
                        bool hasFocus = input->peerId() == keyboardDevice->lastKeyboardInputRequester();
                        input->setFocus(hasFocus);
                        QKeyboardHandler *node = qobject_cast<QKeyboardHandler *>(m_inputHandler->m_scene->lookupNode(input->peerId()));
                        if (node) {
                            const bool b = node->blockNotifications(true);
                            node->setFocus(hasFocus);
                            node->blockNotifications(b);
                        }
                        if (hasFocus)
                            keyboardDevice->setCurrentFocusItem(input->peerId());
                    }
                }
            }

            // deliver the event
            QKeyboardHandler *node = qobject_cast<QKeyboardHandler *>(m_inputHandler->m_scene->lookupNode(keyboardDevice->currentFocusItem()));
            if (node) {
                QKeyboardHandlerPrivate *dnode = static_cast<QKeyboardHandlerPrivate *>(QKeyboardHandlerPrivate::get(node));

                QKeyEvent ke(*event);
                dnode->keyEvent(&ke);  // Do we really need Qt3D specific events?
            }
        }
    }

    return false;
}

} // namespace Input
} // namespace Qt3DInput

QT_END_NAMESPACE
