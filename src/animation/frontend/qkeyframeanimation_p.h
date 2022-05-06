/****************************************************************************
**
** Copyright (C) 2022 The Qt Company Ltd.
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
******************************************************************************/

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
