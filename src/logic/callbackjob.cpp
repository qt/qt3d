/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
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
******************************************************************************/

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
