// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "updateaxisactionjob_p.h"
#include <Qt3DCore/private/qaspectmanager_p.h>
#include <Qt3DInput/qaction.h>
#include <Qt3DInput/qaxis.h>
#include <Qt3DInput/private/qaction_p.h>
#include <Qt3DInput/private/qaxis_p.h>
#include <Qt3DInput/private/inputhandler_p.h>
#include <Qt3DInput/private/inputmanagers_p.h>
#include <Qt3DInput/private/job_common_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {

namespace Input {

class UpdateAxisActionJobPrivate : public Qt3DCore::QAspectJobPrivate
{
public:
    UpdateAxisActionJobPrivate() { }
    ~UpdateAxisActionJobPrivate() override { }

    void postFrame(Qt3DCore::QAspectManager *manager) override;

    QList<QPair<Qt3DCore::QNodeId, bool>> m_triggeredActions;
    QList<QPair<Qt3DCore::QNodeId, float>> m_triggeredAxis;
};

UpdateAxisActionJob::UpdateAxisActionJob(qint64 currentTime, InputHandler *handler, HLogicalDevice handle)
    : Qt3DCore::QAspectJob(*new UpdateAxisActionJobPrivate())
    , m_currentTime(currentTime)
    , m_handler(handler)
    , m_handle(handle)
{
    SET_JOB_RUN_STAT_TYPE(this, JobTypes::UpdateAxisAction, 0)
}

void UpdateAxisActionJob::run()
{
    // Note: we assume axis/action are not really shared:
    // there's no benefit in sharing those when it comes to computing
    LogicalDevice *device = m_handler->logicalDeviceManager()->data(m_handle);

    if (!device->isEnabled())
        return;

    updateAction(device);
    updateAxis(device);
}

void UpdateAxisActionJob::updateAction(LogicalDevice *device)
{
    Q_D(UpdateAxisActionJob);
    const auto actionIds = device->actions();
    d->m_triggeredActions.reserve(actionIds.size());

    for (const Qt3DCore::QNodeId &actionId : actionIds) {
        bool actionTriggered = false;
        Action *action = m_handler->actionManager()->lookupResource(actionId);

        const auto actionInputIds = action->inputs();
        for (const Qt3DCore::QNodeId &actionInputId : actionInputIds)
            actionTriggered |= processActionInput(actionInputId);

        if (action->isEnabled() && (action->actionTriggered() != actionTriggered)) {
            action->setActionTriggered(actionTriggered);
            d->m_triggeredActions.push_back({actionId, actionTriggered});
        }
    }
}

bool UpdateAxisActionJob::processActionInput(const Qt3DCore::QNodeId actionInputId)
{
    AbstractActionInput *actionInput = m_handler->lookupActionInput(actionInputId);
    Q_ASSERT(actionInput);
    return actionInput->process(m_handler, m_currentTime);
}

void UpdateAxisActionJob::updateAxis(LogicalDevice *device)
{
    Q_D(UpdateAxisActionJob);
    const auto axisIds = device->axes();
    d->m_triggeredAxis.reserve(axisIds.size());

    for (const Qt3DCore::QNodeId &axisId : axisIds) {
        Axis *axis = m_handler->axisManager()->lookupResource(axisId);
        float axisValue = 0.0f;

        const auto axisInputIds = axis->inputs();
        for (const Qt3DCore::QNodeId &axisInputId : axisInputIds)
            axisValue += processAxisInput(axisInputId);

        // Clamp the axisValue -1/1
        axisValue = qMin(1.0f, qMax(axisValue, -1.0f));

        if (axis->isEnabled() && !qFuzzyCompare(axisValue, axis->axisValue())) {
            axis->setAxisValue(axisValue);
            d->m_triggeredAxis.push_back({axisId, axisValue});
        }
    }
}

float UpdateAxisActionJob::processAxisInput(const Qt3DCore::QNodeId axisInputId)
{
    AnalogAxisInput *analogInput = m_handler->analogAxisInputManager()->lookupResource(axisInputId);
    if (analogInput)
        return analogInput->process(m_handler, m_currentTime);

    ButtonAxisInput *buttonInput = m_handler->buttonAxisInputManager()->lookupResource(axisInputId);
    if (buttonInput)
        return buttonInput->process(m_handler, m_currentTime);

    Q_UNREACHABLE_RETURN(0.0f);
}

void UpdateAxisActionJobPrivate::postFrame(Qt3DCore::QAspectManager *manager)
{
    for (const auto &data: std::as_const(m_triggeredActions)) {
        Qt3DInput::QAction *action = qobject_cast<Qt3DInput::QAction *>(manager->lookupNode(data.first));
        if (!action)
            continue;

        Qt3DInput::QActionPrivate *daction = static_cast<Qt3DInput::QActionPrivate *>(Qt3DCore::QNodePrivate::get(action));
        daction->setActive(data.second);
    }

    for (const auto &data: std::as_const(m_triggeredAxis)) {
        Qt3DInput::QAxis *axis = qobject_cast<Qt3DInput::QAxis *>(manager->lookupNode(data.first));
        if (!axis)
            continue;

        Qt3DInput::QAxisPrivate *daxis = static_cast<Qt3DInput::QAxisPrivate *>(Qt3DCore::QNodePrivate::get(axis));
        daxis->setValue(data.second);
    }

    m_triggeredActions.clear();
    m_triggeredAxis.clear();
}

} // Input

} // Qt3DInput

QT_END_NAMESPACE
