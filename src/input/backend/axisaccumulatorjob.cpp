// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "axisaccumulatorjob_p.h"
#include <Qt3DCore/private/qaspectmanager_p.h>
#include <Qt3DInput/qaxisaccumulator.h>
#include <Qt3DInput/private/qaxisaccumulator_p.h>
#include <Qt3DInput/private/axisaccumulator_p.h>
#include <Qt3DInput/private/job_common_p.h>
#include <Qt3DInput/private/inputmanagers_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {
namespace Input {

class AxisAccumulatorJobPrivate : public Qt3DCore::QAspectJobPrivate
{
public:
    AxisAccumulatorJobPrivate() { }
    ~AxisAccumulatorJobPrivate() override { }

    void postFrame(Qt3DCore::QAspectManager *manager) override;

    QList<AxisAccumulator *> updates;
};

AxisAccumulatorJob::AxisAccumulatorJob(AxisAccumulatorManager *axisAccumulatormanager,
                                       AxisManager *axisManager)
    : Qt3DCore::QAspectJob(*new AxisAccumulatorJobPrivate)
    , m_axisAccumulatorManager(axisAccumulatormanager)
    , m_axisManager(axisManager)
    , m_dt(0.0f)
{
    SET_JOB_RUN_STAT_TYPE(this, JobTypes::AxisAccumulatorIntegration, 0)
}

void AxisAccumulatorJob::run()
{
    Q_D(AxisAccumulatorJob);
    // Iterate over the accumulators and ask each to step the integrations
    const auto activeHandles = m_axisAccumulatorManager->activeHandles();
    d->updates.reserve(activeHandles.size());

    for (const auto &accumulatorHandle : activeHandles) {
        AxisAccumulator *accumulator = m_axisAccumulatorManager->data(accumulatorHandle);
        if (accumulator->isEnabled()) {
            accumulator->stepIntegration(m_axisManager, m_dt);
            d->updates.push_back(accumulator);
        }
    }
}

void AxisAccumulatorJobPrivate::postFrame(Qt3DCore::QAspectManager *manager)
{
    for (auto backend: std::as_const(updates)) {
        QAxisAccumulator *node = qobject_cast<QAxisAccumulator *>(manager->lookupNode(backend->peerId()));
        if (!node)
            continue;

        QAxisAccumulatorPrivate *dnode = static_cast<QAxisAccumulatorPrivate *>(QAxisAccumulatorPrivate::get(node));
        dnode->setValue(backend->value());
        dnode->setVelocity(backend->velocity());
    }
}

} // namespace Input
} // namespace Qt3DInput

QT_END_NAMESPACE
