/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
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
