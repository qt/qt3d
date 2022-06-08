// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DANIMATION_QVERTEXBLENDANIMATION_P_H
#define QT3DANIMATION_QVERTEXBLENDANIMATION_P_H

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


#include <Qt3DAnimation/qvertexblendanimation.h>

#include <private/qobject_p.h>
#include <private/qabstractanimation_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {

class QVertexBlendAnimationPrivate : public QAbstractAnimationPrivate
{
public:
    QVertexBlendAnimationPrivate();

    void getAttributesInPosition(float position, int *target0, int *target1, float *interpolator);
    void updateAnimation(float position);

    QList<float> m_targetPositions;
    QList<Qt3DAnimation::QMorphTarget *> m_morphTargets;
    float m_interpolator;
    Qt3DRender::QGeometryRenderer *m_target;
    QString m_targetName;
    QMorphTarget *m_currentBase;
    QMorphTarget *m_currentTarget;

    QMetaObject::Connection m_positionConnection;

    Q_DECLARE_PUBLIC(QVertexBlendAnimation)
};

} // Qt3DAnimation

QT_END_NAMESPACE

#endif // QT3DANIMATION_QVertexBlendANIMATION_P_H
