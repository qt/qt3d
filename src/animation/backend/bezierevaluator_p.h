// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef BEZIEREVALUATOR_P_H
#define BEZIEREVALUATOR_P_H

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

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {
namespace Animation {

struct Keyframe;

class Q_AUTOTEST_EXPORT BezierEvaluator
{
public:
    explicit BezierEvaluator(float time0, const Keyframe &keyframe0,
                             float time1, const Keyframe &keyframe1)
        : m_time0(time0)
        , m_time1(time1)
        , m_keyframe0(keyframe0)
        , m_keyframe1(keyframe1)
    {
    }

    float valueForTime(float time) const;
    float parameterForTime(float time) const;

    static int findCubicRoots(const float coefficients[], float roots[3]);

private:
    float m_time0;
    float m_time1;
    const Keyframe &m_keyframe0;
    const Keyframe &m_keyframe1;
};

} // namespace Animation
} // namespace Qt3DAnimation

QT_END_NAMESPACE

#endif // BEZIEREVALUATOR_P_H
