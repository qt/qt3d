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

#include "qinputaspect.h"
#include "qinputaspect_p.h"
#include "cameracontroller_p.h"
#include "inputhandler_p.h"
#include "keyboardcontroller_p.h"
#include "keyboardinput_p.h"
#include "mousecontroller_p.h"
#include "mouseinput_p.h"
#include <Qt3DInput/qkeyboardcontroller.h>
#include <Qt3DInput/qkeyboardinput.h>
#include <Qt3DInput/qmousecontroller.h>
#include <Qt3DInput/qmouseinput.h>
#include <Qt3DInput/qinputdeviceintegration.h>
#include <Qt3DInput/private/qinputdeviceintegrationfactory_p.h>
#include <Qt3DCore/private/qservicelocator_p.h>
#include <Qt3DCore/private/qeventfilterservice_p.h>
#include <QDir>
#include <QLibrary>
#include <QLibraryInfo>
#include <QPluginLoader>

#include <Qt3DInput/qaxis.h>
#include <Qt3DInput/qaxisactionhandler.h>
#include <Qt3DInput/qaction.h>
#include <Qt3DInput/qaxisinput.h>
#include <Qt3DInput/qaxissetting.h>
#include <Qt3DInput/qactioninput.h>
#include <Qt3DInput/qlogicaldevice.h>
#include <Qt3DInput/qabstractphysicaldevice.h>
#include <Qt3DInput/private/axis_p.h>
#include <Qt3DInput/private/axisactionhandler_p.h>
#include <Qt3DInput/private/action_p.h>
#include <Qt3DInput/private/axisinput_p.h>
#include <Qt3DInput/private/axissetting_p.h>
#include <Qt3DInput/private/actioninput_p.h>
#include <Qt3DInput/private/logicaldevice_p.h>
#include <Qt3DInput/private/inputbackendnodefunctor_p.h>
#include <Qt3DInput/private/inputmanagers_p.h>
#include <Qt3DInput/private/updateaxisactionjob_p.h>
#include <Qt3DInput/private/updatehandlerjob_p.h>
#include <Qt3DInput/private/keyboardmousedeviceintegration_p.h>

QT_BEGIN_NAMESPACE

using namespace Qt3DCore;

namespace Qt3DInput {

QInputAspectPrivate::QInputAspectPrivate()
    : QAbstractAspectPrivate()
    , m_inputHandler(new Input::InputHandler())
    , m_cameraController(new Input::CameraController())
    , m_keyboardMouseIntegration(new Input::KeyboardMouseDeviceIntegration(m_inputHandler.data()))
{
}

/*!
    \class Qt3DInput::QInputAspect
    \inmodule Qt3DInput
    \since 5.5
*/

QInputAspect::QInputAspect(QObject *parent)
    : QAbstractAspect(*new QInputAspectPrivate, parent)
{
    registerBackendType<QKeyboardController>(QBackendNodeFunctorPtr(new Input::KeyboardControllerFunctor(this, d_func()->m_inputHandler.data())));
    registerBackendType<QKeyboardInput>(QBackendNodeFunctorPtr(new Input::KeyboardInputFunctor(d_func()->m_inputHandler.data())));
    registerBackendType<QMouseController>(QBackendNodeFunctorPtr(new Input::MouseControllerFunctor(this, d_func()->m_inputHandler.data())));
    registerBackendType<QMouseInput>(QBackendNodeFunctorPtr(new Input::MouseInputFunctor(d_func()->m_inputHandler.data())));
    registerBackendType<QAxis>(QBackendNodeFunctorPtr(new Input::InputNodeFunctor<Input::Axis, Input::AxisManager>(d_func()->m_inputHandler->axisManager())));
    registerBackendType<QAxisInput>(QBackendNodeFunctorPtr(new Input::InputNodeFunctor<Input::AxisInput, Input::AxisInputManager>(d_func()->m_inputHandler->axisInputManager())));
    registerBackendType<QAxisSetting>(QBackendNodeFunctorPtr(new Input::InputNodeFunctor<Input::AxisSetting, Input::AxisSettingManager>(d_func()->m_inputHandler->axisSettingManager())));
    registerBackendType<Qt3DInput::QAction>(QBackendNodeFunctorPtr(new Input::InputNodeFunctor<Input::Action, Input::ActionManager>(d_func()->m_inputHandler->actionManager())));
    registerBackendType<QActionInput>(QBackendNodeFunctorPtr(new Input::InputNodeFunctor<Input::ActionInput, Input::ActionInputManager>(d_func()->m_inputHandler->actionInputManager())));
    registerBackendType<Qt3DInput::QAxisActionHandler>(QBackendNodeFunctorPtr(new Input::AxisActionHandlerNodeFunctor(d_func()->m_inputHandler->axisActionHandlerManager())));
    registerBackendType<QLogicalDevice>(QBackendNodeFunctorPtr(new Input::LogicalDeviceNodeFunctor(d_func()->m_inputHandler->logicalDeviceManager())));

    // Plugins are QInputDeviceIntegration instances
    loadInputDevicePlugins();

    // KeyboardController and MouseController also provide their own QInputDeviceIntegration
    Q_D(QInputAspect);
    d->m_inputHandler->addInputDeviceIntegration(d->m_keyboardMouseIntegration.data());
}

void QInputAspect::loadInputDevicePlugins()
{
    Q_D(QInputAspect);
    QStringList keys = QInputDeviceIntegrationFactory::keys();
    Q_FOREACH (QString key, keys) {
        Qt3DInput::QInputDeviceIntegration *integration = QInputDeviceIntegrationFactory::create(key, QStringList());
        if (integration != Q_NULLPTR) {
            d->m_inputHandler->addInputDeviceIntegration(integration);
            // Initialize will allow the InputDeviceIntegration to
            // register their frontend / backend types,
            // create their managers
            // launch a thread to listen to the actual physical device....
            integration->initialize(this);
        }
    }
}

Qt3DCore::QCamera *QInputAspect::camera() const
{
    Q_D(const QInputAspect);
    return d->m_cameraController->camera();
}

// Note: caller is responsible for ownership
QAbstractPhysicalDevice *QInputAspect::createPhysicalDevice(const QString &name)
{
    Q_D(QInputAspect);
    QAbstractPhysicalDevice *device = Q_NULLPTR;
    Q_FOREACH (Qt3DInput::QInputDeviceIntegration *integration, d->m_inputHandler->inputDeviceIntegrations()) {
        if ((device = integration->createPhysicalDevice(name)) != Q_NULLPTR)
            break;
    }
    return device;
}

void QInputAspect::setCamera(Qt3DCore::QCamera *camera)
{
    Q_D(QInputAspect);
    d->m_cameraController->setCamera(camera);
}

QVector<QAspectJobPtr> QInputAspect::jobsToExecute(qint64 time)
{
    Q_UNUSED(time);
    Q_D(QInputAspect);
    QVector<QAspectJobPtr> jobs;

    jobs.append(d->m_inputHandler->keyboardJobs());
    jobs.append(d->m_inputHandler->mouseJobs());

    Q_FOREACH (QInputDeviceIntegration *integration, d->m_inputHandler->inputDeviceIntegrations())
        jobs += integration->jobsToExecute(time);

    // Jobs that update Axis/Action (store combined axis/action value)
    QHash<Input::HLogicalDevice, QAspectJobPtr> logicalDeviceJobs;

    Q_FOREACH (Input::HLogicalDevice devHandle, d->m_inputHandler->logicalDeviceManager()->activeDevices()) {
        QAspectJobPtr updateAxisActionJob(new Input::UpdateAxisActionJob(d->m_inputHandler.data(), devHandle));
        logicalDeviceJobs.insert(devHandle, updateAxisActionJob);

        Q_FOREACH (const QAspectJobPtr job, jobs)
            updateAxisActionJob->addDependency(job);
    }

    // Jobs that update the axisactionhandlers
    Q_FOREACH (Input::HAxisActionHandler handlerHandle, d->m_inputHandler->axisActionHandlerManager()->activeAxisActionHandlers()) {
        Input::AxisActionHandler *axisActionHandler = d->m_inputHandler->axisActionHandlerManager()->data(handlerHandle);
        Input::HLogicalDevice logicalDeviceHandle = d->m_inputHandler->logicalDeviceManager()->lookupHandle(axisActionHandler->logicalDevice());
        QAspectJobPtr updateHandlerJob(new Input::UpdateHandlerJob(axisActionHandler, logicalDeviceHandle, d->m_inputHandler.data()));

        // Create AxisActionHandler Job
        jobs += updateHandlerJob;

        QAspectJobPtr logicalDeviceJob = logicalDeviceJobs.value(logicalDeviceHandle);
        if (logicalDeviceJob) {
            updateHandlerJob->addDependency(logicalDeviceJob);
            jobs += logicalDeviceJob;
        }
    }

    return jobs;
}

void QInputAspect::onInitialize(const QVariantMap &)
{
    Q_D(QInputAspect);
    Qt3DCore::QEventFilterService *eventService = d->services()->eventFilterService();
    eventService->registerEventFilter(d->m_cameraController.data(), 128);
    d->m_inputHandler->registerEventFilters(eventService);
}

void QInputAspect::onCleanup()
{
    Q_D(QInputAspect);
    // At this point it is too late to call removeEventFilter as the eventSource (Window)
    // may already be destroyed
    d->m_inputHandler.reset(Q_NULLPTR);
}

} // namespace Qt3DInput

QT_END_NAMESPACE

QT3D_REGISTER_NAMESPACED_ASPECT("input", QT_PREPEND_NAMESPACE(Qt3DInput), QInputAspect)
