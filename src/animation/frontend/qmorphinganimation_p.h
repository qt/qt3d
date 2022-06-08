// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DANIMATION_QMORPHINGANIMATION_P_H
#define QT3DANIMATION_QMORPHINGANIMATION_P_H

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

#include <QtCore/qeasingcurve.h>
#include <Qt3DAnimation/qmorphinganimation.h>

#include <private/qobject_p.h>
#include <private/qabstractanimation_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {

class QMorphingAnimationPrivate : public QAbstractAnimationPrivate
{
public:
    QMorphingAnimationPrivate();
    ~QMorphingAnimationPrivate();

    void updateAnimation(float position);
    void setTargetInterpolated(int morphTarget);

    float m_minposition;
    float m_maxposition;
    QVector<float> m_targetPositions;
    QVector<QVector<float>*> m_weights;
    QVector<float> m_morphKey;
    QStringList m_attributeNames;
    QVector<Qt3DAnimation::QMorphTarget *> m_morphTargets;
    QMorphTarget *m_flattened;
    QMorphingAnimation::Method m_method;
    QEasingCurve m_easing;
    float m_interpolator;
    Qt3DRender::QGeometryRenderer *m_target;
    QString m_targetName;

    QMorphTarget *m_currentTarget;

    QMetaObject::Connection m_positionConnection;

    Q_DECLARE_PUBLIC(QMorphingAnimation)
};

} // Qt3DAnimation

QT_END_NAMESPACE

#endif // QT3DANIMATION_QMORPHINGANIMATION_P_H
