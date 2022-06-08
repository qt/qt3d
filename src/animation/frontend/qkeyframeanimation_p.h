// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DANIMATION_QKEYFRAMEANIMATION_P_H
#define QT3DANIMATION_QKEYFRAMEANIMATION_P_H

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

#include <private/qobject_p.h>
#include <Qt3DAnimation/qkeyframeanimation.h>
#include <private/qabstractanimation_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {

class QKeyframeAnimationPrivate : public QAbstractAnimationPrivate
{
public:
    QKeyframeAnimationPrivate();

    void calculateFrame(float position);

    QList<float> m_framePositions;
    QList<Qt3DCore::QTransform *> m_keyframes;
    Qt3DCore::QTransform *m_target;
    QEasingCurve m_easing;
    QString m_animationName;
    QString m_targetName;
    float m_minposition;
    float m_maxposition;
    QKeyframeAnimation::RepeatMode m_startMode;
    QKeyframeAnimation::RepeatMode m_endMode;
    QVector3D m_baseScale;
    QVector3D m_baseTranslation;
    QQuaternion m_baseRotation;
    QMetaObject::Connection m_positionConnection;

    Q_DECLARE_PUBLIC(QKeyframeAnimation)
};

} // Qt3DAnimation

QT_END_NAMESPACE

#endif // QT3DANIMATION_QKEYFRAMEANIMATION_P_H
