// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DANIMATION_QKEYFRAMEANIMATION_H
#define QT3DANIMATION_QKEYFRAMEANIMATION_H

#include <Qt3DCore/qtransform.h>

#include <Qt3DAnimation/qabstractanimation.h>
#include <Qt3DAnimation/qt3danimation_global.h>

#include <QtCore/qeasingcurve.h>

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {

class QKeyframeAnimationPrivate;

class Q_3DANIMATIONSHARED_EXPORT QKeyframeAnimation : public QAbstractAnimation
{
    Q_OBJECT
    Q_PROPERTY(QList<float> framePositions READ framePositions WRITE setFramePositions NOTIFY framePositionsChanged)
    Q_PROPERTY(Qt3DCore::QTransform *target READ target WRITE setTarget NOTIFY targetChanged)
    Q_PROPERTY(QEasingCurve easing READ easing WRITE setEasing NOTIFY easingChanged)
    Q_PROPERTY(QString targetName READ targetName WRITE setTargetName NOTIFY targetNameChanged)
    Q_PROPERTY(RepeatMode startMode READ startMode WRITE setStartMode NOTIFY startModeChanged)
    Q_PROPERTY(RepeatMode endMode READ endMode WRITE setEndMode NOTIFY endModeChanged)

public:
    explicit QKeyframeAnimation(QObject *parent = nullptr);

    enum RepeatMode
    {
        None,
        Constant,
        Repeat,
    };
    Q_ENUM(RepeatMode)

    QList<float> framePositions() const;
    QList<Qt3DCore::QTransform *> keyframeList() const;
    Qt3DCore::QTransform *target() const;
    QEasingCurve easing() const;
    QString targetName() const;
    RepeatMode startMode() const;
    RepeatMode endMode() const;

    void setKeyframes(const QList<Qt3DCore::QTransform *> &keyframes);
    void addKeyframe(Qt3DCore::QTransform *keyframe);
    void removeKeyframe(Qt3DCore::QTransform *keyframe);

public Q_SLOTS:
    void setFramePositions(const QList<float> &positions);
    void setTarget(Qt3DCore::QTransform *target);
    void setEasing(const QEasingCurve &easing);
    void setTargetName(const QString &name);
    void setStartMode(RepeatMode mode);
    void setEndMode(RepeatMode mode);

Q_SIGNALS:
    void framePositionsChanged(const QList<float> &positions);
    void targetChanged(Qt3DCore::QTransform *target);
    void easingChanged(const QEasingCurve &easing);
    void targetNameChanged(const QString &name);
    void startModeChanged(QKeyframeAnimation::RepeatMode startMode);
    void endModeChanged(QKeyframeAnimation::RepeatMode endMode);

private:
    void updateAnimation(float position);

    Q_DECLARE_PRIVATE(QKeyframeAnimation)
};

} // Qt3DAnimation

QT_END_NAMESPACE

#endif // QT3DANIMATION_QKEYFRAMEANIMATION_H
