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
****************************************************************************/

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
        m_total -= m_samples.at(m_currentSample);
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
