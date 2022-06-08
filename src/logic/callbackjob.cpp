// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "callbackjob_p.h"

#include <Qt3DLogic/private/job_common_p.h>
#include <Qt3DLogic/private/manager_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DLogic {
namespace Logic {

CallbackJob::CallbackJob()
    : QAspectJob()
    , m_logicManager(nullptr)
{
    SET_JOB_RUN_STAT_TYPE(this, JobTypes::Callback, 0)
}

void CallbackJob::setManager(Manager *manager)
{
    m_logicManager = manager;
}

void CallbackJob::run()
{
}

bool CallbackJob::isRequired()
{
    return false;
}

void CallbackJob::postFrame(Qt3DCore::QAspectEngine *aspectEngine)
{
    Q_UNUSED(aspectEngine);
    Q_ASSERT(m_logicManager);
    m_logicManager->triggerLogicFrameUpdates();
}

} // namespace Logic
} // namespace Qt3DLogic

QT_END_NAMESPACE
