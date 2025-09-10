// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DANIMATION_QVERTEXBLENDANIMATION_H
#define QT3DANIMATION_QVERTEXBLENDANIMATION_H

#include <Qt3DRender/qgeometryrenderer.h>
#include <Qt3DAnimation/qabstractanimation.h>
#include <Qt3DAnimation/qmorphtarget.h>

#include <Qt3DAnimation/qt3danimation_global.h>

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {

class QVertexBlendAnimationPrivate;

class Q_3DANIMATIONSHARED_EXPORT QVertexBlendAnimation : public QAbstractAnimation
{
    Q_OBJECT
    Q_PROPERTY(QList<float> targetPositions READ targetPositions WRITE setTargetPositions NOTIFY targetPositionsChanged)
    Q_PROPERTY(float interpolator READ interpolator NOTIFY interpolatorChanged)
    Q_PROPERTY(Qt3DRender::QGeometryRenderer *target READ target WRITE setTarget NOTIFY targetChanged)
    Q_PROPERTY(QString targetName READ targetName WRITE setTargetName NOTIFY targetNameChanged)

public:
    explicit QVertexBlendAnimation(QObject *parent = nullptr);

    QList<float> targetPositions() const;
    float interpolator() const;
    Qt3DRender::QGeometryRenderer *target() const;
    QString targetName() const;

    void setMorphTargets(const QList<Qt3DAnimation::QMorphTarget *> &targets);
    void addMorphTarget(Qt3DAnimation::QMorphTarget *target);
    void removeMorphTarget(Qt3DAnimation::QMorphTarget *target);

    QList<Qt3DAnimation::QMorphTarget *> morphTargetList();

public Q_SLOTS:
    void setTargetPositions(const QList<float> &targetPositions);
    void setTarget(Qt3DRender::QGeometryRenderer *target);
    void setTargetName(const QString name);

Q_SIGNALS:
    void targetPositionsChanged(const QList<float> &targetPositions);
    void interpolatorChanged(float interpolator);
    void targetChanged(Qt3DRender::QGeometryRenderer *target);
    void targetNameChanged(const QString &name);

private:

    void updateAnimation(float position);

    Q_DECLARE_PRIVATE(QVertexBlendAnimation)
};

} // Qt3DAnimation

QT_END_NAMESPACE

#endif // QT3DANIMATION_QVERTEXBLENDANIMATION_H
