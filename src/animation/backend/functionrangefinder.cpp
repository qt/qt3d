// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "functionrangefinder_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {
namespace Animation {

/*!
    \internal
    \class FunctionRangeFinder finds the lower bound index of a range that encloses a function value

    Given a vector of function values (typically abscissa values of some other function), this
    class can find the lower bound index of a range that encloses the requested value. This is
    very useful for finding the two points that sandwich a value to which you later wish to
    interpolate for example.
 */

/*
    \internal

    int findLowerBound (float x)

    Finds the lower bound index of a range that encloses the requested value \a x.

    We use a technique which tries to be better than a simple bisection. Often when
    performing interpolations, subsequent points are correlated with earlier calls.
    This is especially true with time based lookups. If two calls are determined to
    be correlated, then the next subsequent call will use the hunt function to search
    close to the last returned value first. The hunt algorithms searches outward in
    increasing step sizes until a sandwiching range is found. Traditional bisection
    is then used to refine this result.

    If the previous results are uncorrelated, a simple bisection is used.
 */

FunctionRangeFinder::FunctionRangeFinder(QList<float> *x)
    : m_x(x)
    , m_previousLowerBound(0)
    , m_correlated(0)
    , m_rangeSize(2)
    , m_correlationThreshold(1)
    , m_ascending(true)
{
    updateAutomaticCorrelationThreshold();
    if (!m_x->isEmpty())
        m_ascending = (m_x->last() >= m_x->first());
}

/*!
    \internal
    Locates the lower bound of a range that encloses \a x by a bisection method.
*/
int FunctionRangeFinder::locate(float x) const
{
    if (m_x->size() < 2 || m_rangeSize < 2 || m_rangeSize > m_x->size())
        return -1;

    qsizetype jLower = 0;
    qsizetype jUpper = m_x->size() - 1;
    while (jUpper - jLower > 1) {
        qsizetype jMid = (jUpper + jLower) >> 1;
        if ((x >= m_x->at(jMid)) == m_ascending)
            jLower = jMid;
        else
            jUpper = jMid;
    }

    m_correlated = std::abs(jLower - m_previousLowerBound) <= m_correlationThreshold;
    m_previousLowerBound = jLower;

    return qMax(0, qMin(m_x->size() - m_rangeSize, jLower - ((m_rangeSize - 2) >> 1)));
}

/*!
    \internal
    Hunts outward from the previous result in increasing step sizes then refines via bisection.
 */
int FunctionRangeFinder::hunt(float x) const
{
    if (m_x->size() < 2 || m_rangeSize < 2 || m_rangeSize > m_x->size())
        return -1;

    qsizetype jLower = m_previousLowerBound;
    qsizetype jMid;
    qsizetype jUpper;
    if (jLower < 0 || jLower > (m_x->size() - 1)) {
        jLower = 0;
        jUpper = m_x->size() - 1;
    } else {
        qsizetype increment = 1;
        if ((x >= m_x->at(jLower)) == m_ascending) {
            for (;;) {
                jUpper = jLower + increment;
                if (jUpper >= m_x->size() - 1) {
                    jUpper = m_x->size() - 1;
                    break;
                } else if ((x < m_x->at(jUpper)) == m_ascending) {
                    break;
                } else {
                    jLower = jUpper;
                    increment += increment;
                }
            }
        } else {
            jUpper = jLower;
            for (;;) {
                jLower = jLower - increment;
                if (jLower <= 0) {
                    jLower = 0;
                    break;
                } else if ((x >= m_x->at(jLower)) == m_ascending) {
                    break;
                } else {
                    jUpper = jLower;
                    increment += increment;
                }
            }
        }
    }

    while (jUpper - jLower > 1) {
        jMid = (jUpper + jLower) >> 1;
        if ((x >= m_x->at(jMid)) == m_ascending)
            jLower = jMid;
        else
            jUpper = jMid;
    }

    m_correlated = std::abs(jLower - m_previousLowerBound) <= m_correlationThreshold;
    m_previousLowerBound = jLower;

    return qMax(0, qMin(m_x->size() - m_rangeSize, jLower - ((m_rangeSize - 2) >> 1)));
}

} // namespace Animation
} // namespace Qt3DAnimation

QT_END_NAMESPACE
