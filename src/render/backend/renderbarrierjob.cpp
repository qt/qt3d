/****************************************************************************
**
** Copyright (C) 2018 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "renderbarrierjob_p.h"

#include <Qt3DRender/private/renderlogging_p.h>

#include <QtCore/QThread>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {

QDebug operator<<(QDebug debug, JobTypes::JobType type)
{
    switch (type) {
    case JobTypes::ReadRenderQueueSizeBarrier:
        debug << "ReadRenderQueueSize";
        break;
    case JobTypes::BeginDrawingBarrier:
        debug << "BeginDrawing";
        break;
    case JobTypes::UpdateGLResourcesBarrier:
        debug << "UpdateGLResources";
        break;
    case JobTypes::PrepareCommandSubmissionBarrier:
        debug << "PrepareCommandSubmission";
        break;
    case JobTypes::EndDrawingBarrier:
        debug << "EndDrawing";
        break;
    default:
        break;
    }
    return debug;
}

RenderBarrierJob::RenderBarrierJob(JobTypes::JobType type)
    : QAspectJob()
    , m_type(type)
    , m_begin(0)
    , m_end(0)
{
    SET_JOB_RUN_STAT_TYPE(this, type, 0);
}

void RenderBarrierJob::waitForDependencies()
{
    qCDebug(Jobs) << Q_FUNC_INFO << m_type << "waiting for job on" << QThread::currentThread();
    m_begin.acquire();
    qCDebug(Jobs) << Q_FUNC_INFO << m_type << "done waiting for job on" << QThread::currentThread();
    Q_ASSERT(m_begin.available() == 0);
}

void RenderBarrierJob::allowToProceed()
{
    qCDebug(Jobs) << Q_FUNC_INFO << m_type << "releasing job on" << QThread::currentThread();
    m_end.release();
}

void RenderBarrierJob::run()
{
    qCDebug(Jobs) << Q_FUNC_INFO << m_type << "job releasing render thread on" << QThread::currentThread();
    m_begin.release();
    qCDebug(Jobs) << Q_FUNC_INFO << m_type << "job waiting for render thread on" << QThread::currentThread();
    m_end.acquire();
    qCDebug(Jobs) << Q_FUNC_INFO << m_type << "job done on" << QThread::currentThread();
    Q_ASSERT(m_end.available() == 0);
}

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
