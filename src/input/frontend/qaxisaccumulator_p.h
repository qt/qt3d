// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DINPUT_QAXISACCUMULATOR_P_H
#define QT3DINPUT_QAXISACCUMULATOR_P_H

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

#include <Qt3DInput/qaxisaccumulator.h>

#include <Qt3DCore/private/qcomponent_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {

class QAxisAccumulatorPrivate : public Qt3DCore::QComponentPrivate
{
public:
    QAxisAccumulatorPrivate();

    Q_DECLARE_PUBLIC(QAxisAccumulator)

    void setValue(float value);
    void setVelocity(float velocity);

    QAxis *m_sourceAxis;
    QAxisAccumulator::SourceAxisType m_sourceAxisType;
    float m_scale;
    float m_value;
    float m_velocity;
};

struct QAxisAccumulatorData
{
    Qt3DCore::QNodeId sourceAxisId;
    QAxisAccumulator::SourceAxisType sourceAxisType;
    float scale;
};

} // Qt3DInput

QT_END_NAMESPACE

#endif // QT3DINPUT_QAXISACCUMULATOR_P_H
