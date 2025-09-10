// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DINPUT_INPUT_AXISACCUMULATORJOB_H
#define QT3DINPUT_INPUT_AXISACCUMULATORJOB_H

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

#include <Qt3DInput/private/handle_types_p.h>
#include <Qt3DCore/qaspectjob.h>
#include <Qt3DCore/qnodeid.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {
namespace Input {

class AxisAccumulatorManager;
class AxisManager;
class AxisAccumulatorJobPrivate;

class Q_AUTOTEST_EXPORT AxisAccumulatorJob : public Qt3DCore::QAspectJob
{
public:
    AxisAccumulatorJob(AxisAccumulatorManager *axisAccumulatormanager,
                       AxisManager *axisManager);

    void setDeltaTime(float dt) { m_dt = dt; }

    void run() override;

private:
    Q_DECLARE_PRIVATE(AxisAccumulatorJob)

    AxisAccumulatorManager *m_axisAccumulatorManager;
    AxisManager *m_axisManager;
    float m_dt;
};

typedef QSharedPointer<AxisAccumulatorJob> AxisAccumulatorJobPtr;

} // namespace Input
} // namespace Qt3DInput

QT_END_NAMESPACE

#endif // QT3DINPUT_INPUT_AXISACCUMULATORJOB_H
