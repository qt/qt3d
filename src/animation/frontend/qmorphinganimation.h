/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
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
****************************************************************************/

#ifndef QT3DANIMATION_QMORPHINGANIMATION_H
#define QT3DANIMATION_QMORPHINGANIMATION_H

#include <Qt3DRender/qgeometryrenderer.h>

#include <Qt3DAnimation/qabstractanimation.h>
#include <Qt3DAnimation/qmorphtarget.h>
#include <Qt3DAnimation/qt3danimation_global.h>

#include <QtCore/qeasingcurve.h>

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {

class QMorphingAnimationPrivate;

class Q_3DANIMATIONSHARED_EXPORT QMorphingAnimation : public QAbstractAnimation
{
    Q_OBJECT
    Q_PROPERTY(QVector<float> targetPositions READ targetPositions WRITE setTargetPositions NOTIFY targetPositionsChanged)
    Q_PROPERTY(float interpolator READ interpolator NOTIFY interpolatorChanged)
    Q_PROPERTY(Qt3DRender::QGeometryRenderer *target READ target WRITE setTarget NOTIFY targetChanged)
    Q_PROPERTY(QString targetName READ targetName WRITE setTargetName NOTIFY targetNameChanged)
    Q_PROPERTY(Method method READ method WRITE setMethod NOTIFY methodChanged)
    Q_PROPERTY(QEasingCurve easing READ easing WRITE setEasing NOTIFY easingChanged)

public:
    enum Method
    {
        Normalized,
        Relative
    };
    Q_ENUM(Method)

    explicit QMorphingAnimation(QObject *parent = nullptr);

    QVector<float> targetPositions() const;
    float interpolator() const;
    Qt3DRender::QGeometryRenderer *target() const;
    QString targetName() const;
    QMorphingAnimation::Method method() const;
    QEasingCurve easing() const;

    void setMorphTargets(const QVector<Qt3DAnimation::QMorphTarget *> &targets);
    void addMorphTarget(Qt3DAnimation::QMorphTarget *target);
    void removeMorphTarget(Qt3DAnimation::QMorphTarget *target);

    void setWeights(int positionIndex, const QVector<float> &weights);
    QVector<float> getWeights(int positionIndex);

    QVector<Qt3DAnimation::QMorphTarget *> morphTargetList();

public Q_SLOTS:
    void setTargetPositions(const QVector<float> &targetPositions);
    void setTarget(Qt3DRender::QGeometryRenderer *target);
    void setTargetName(const QString name);
    void setMethod(QMorphingAnimation::Method method);
    void setEasing(const QEasingCurve &easing);

Q_SIGNALS:
    void targetPositionsChanged(const QVector<float> &targetPositions);
    void interpolatorChanged(float interpolator);
    void targetChanged(Qt3DRender::QGeometryRenderer *target);
    void targetNameChanged(const QString &name);
    void methodChanged(QMorphingAnimation::Method method);
    void easingChanged(const QEasingCurve &easing);

private:

    void updateAnimation(float position);

    Q_DECLARE_PRIVATE(QMorphingAnimation)
};

} // Qt3DAnimation

QT_END_NAMESPACE

#endif // QT3DANIMATION_QMORPHINGANIMATION_H
