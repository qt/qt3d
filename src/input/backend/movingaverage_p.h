// Copyright (C) 2020 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
#ifndef QT3DINPUT_MOVINGAVERAGE_P_H
#define QT3DINPUT_MOVINGAVERAGE_P_H

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

#include <QtCore/private/qglobal_p.h>

#include <vector>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {
namespace Input {

class MovingAverage
{
public:
    MovingAverage(unsigned int samples = 3);

    void addSample(float sample);

    float average() const;

private:
    unsigned int m_maxSampleCount;
    unsigned int m_sampleCount;
    unsigned int m_currentSample;
    float m_total;
    std::vector<float> m_samples;
};

} // Input
} // Qt3DInput

QT_END_NAMESPACE

#endif // QT3DINPUT_MOVINGAVERAGE_H
