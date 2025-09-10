// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "movingaverage_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DInput {
namespace Input {

MovingAverage::MovingAverage(unsigned int samples)
    : m_maxSampleCount(samples)
    , m_sampleCount(0)
    , m_currentSample(0)
    , m_total(0.0f)
    , m_samples(samples)
{
}

void MovingAverage::addSample(float sample)
{
    if (m_sampleCount == m_maxSampleCount)
        m_total -= m_samples[m_currentSample];
    else
        ++m_sampleCount;

    m_samples[m_currentSample] = sample;
    m_total += sample;
    ++m_currentSample;
    if (m_currentSample >= m_maxSampleCount)
        m_currentSample = 0;
}

float MovingAverage::average() const
{
    return m_sampleCount ? m_total / static_cast<float>(m_sampleCount) : 0.0f;
}

} // Input
} // Qt3DInput

QT_END_NAMESPACE
