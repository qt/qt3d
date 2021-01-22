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
    Q_PROPERTY(QVector<float> framePositions READ framePositions WRITE setFramePositions NOTIFY framePositionsChanged)
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

    QVector<float> framePositions() const;
    QVector<Qt3DCore::QTransform *> keyframeList() const;
    Qt3DCore::QTransform *target() const;
    QEasingCurve easing() const;
    QString targetName() const;
    RepeatMode startMode() const;
    RepeatMode endMode() const;

    void setKeyframes(const QVector<Qt3DCore::QTransform *> &keyframes);
    void addKeyframe(Qt3DCore::QTransform *keyframe);
    void removeKeyframe(Qt3DCore::QTransform *keyframe);

public Q_SLOTS:
    void setFramePositions(const QVector<float> &positions);
    void setTarget(Qt3DCore::QTransform *target);
    void setEasing(const QEasingCurve &easing);
    void setTargetName(const QString &name);
    void setStartMode(RepeatMode mode);
    void setEndMode(RepeatMode mode);

Q_SIGNALS:
    void framePositionsChanged(const QVector<float> &positions);
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
