/****************************************************************************
**
** Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
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
******************************************************************************/

#ifndef QT3DANIMATION_ANIMATION_KEYFRAME_P_H
#define QT3DANIMATION_ANIMATION_KEYFRAME_P_H

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

#include <Qt3DAnimation/qkeyframe.h>
#include <QtGui/qvector2d.h>

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {
namespace Animation {

struct Keyframe
{
    inline bool operator==(const Keyframe &rhs) const
    {
        return value == rhs.value
            && leftControlPoint == rhs.leftControlPoint
            && rightControlPoint == rhs.rightControlPoint
            && interpolation == rhs.interpolation;
    }

    float value;                 // Value (time is stored separately in FCurve)
    QVector2D leftControlPoint;  // Bezier control point (time, value)
    QVector2D rightControlPoint; // Bezier control point (time, value)
    QKeyFrame::InterpolationType interpolation; // Method to use for evaluation between this Keyframe and the next
};

} // namespace Animation
} // namespace Qt3DAnimation

QT_END_NAMESPACE

#endif // QT3DANIMATION_ANIMATION_KEYFRAME_P_H
