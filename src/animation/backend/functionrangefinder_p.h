// Copyright (C) 2020 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
#ifndef QT3DANIMATION_ANIMATION_FUNCTIONRANGEFINDER_P_H
#define QT3DANIMATION_ANIMATION_FUNCTIONRANGEFINDER_P_H

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

#include <QtCore/qlist.h>
#include <private/qglobal_p.h>

#include <cmath>
#include <cstdlib>

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {
namespace Animation {

class Q_AUTOTEST_EXPORT FunctionRangeFinder
{
public:
    FunctionRangeFinder(QList<float> *x);

    inline int findLowerBound(float x) const { return m_correlated ? hunt(x) : locate(x); }

    int rangeSize() const { return m_rangeSize; }
    void setRangeSize(int rangeSize) { m_rangeSize = rangeSize; }

    bool isAscending() const { return m_ascending; }
    void setAscending(bool ascending) { m_ascending = ascending; }

    int correlationThreshold() const { return m_correlationThreshold; }
    void updateAutomaticCorrelationThreshold()
    {
        m_correlationThreshold = std::max(1, int(std::pow(float(m_x->size()), 0.25)));
    }

private:
    int locate(float x) const;
    int hunt(float x) const;

    QList<float> *m_x;
    mutable qsizetype m_previousLowerBound;
    mutable bool m_correlated;
    int m_rangeSize;
    int m_correlationThreshold;
    bool m_ascending;
};

} // namespace Animation
} // namespace Qt3DAnimation

QT_END_NAMESPACE

#endif // QT3DANIMATION_ANIMATION_FUNCTIONRANGEFINDER_P_H
