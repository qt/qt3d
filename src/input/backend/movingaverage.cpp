/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
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
