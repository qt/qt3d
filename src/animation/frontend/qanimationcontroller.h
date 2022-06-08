// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DANIMATION_QANIMATIONCONTROLLER_H
#define QT3DANIMATION_QANIMATIONCONTROLLER_H

#include <Qt3DAnimation/qkeyframeanimation.h>
#include <Qt3DAnimation/qanimationgroup.h>
#include <Qt3DCore/qentity.h>

#include <Qt3DAnimation/qt3danimation_global.h>

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {

class QAnimationControllerPrivate;

class Q_3DANIMATIONSHARED_EXPORT QAnimationController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int activeAnimationGroup READ activeAnimationGroup WRITE setActiveAnimationGroup NOTIFY activeAnimationGroupChanged)
    Q_PROPERTY(float position READ position WRITE setPosition NOTIFY positionChanged)
    Q_PROPERTY(float positionScale READ positionScale WRITE setPositionScale NOTIFY positionScaleChanged)
    Q_PROPERTY(float positionOffset READ positionOffset WRITE setPositionOffset NOTIFY positionOffsetChanged)
    Q_PROPERTY(Qt3DCore::QEntity *entity READ entity WRITE setEntity NOTIFY entityChanged)
    Q_PROPERTY(bool recursive READ recursive WRITE setRecursive NOTIFY recursiveChanged)

public:
    QAnimationController(QObject *parent = nullptr);

    QList<Qt3DAnimation::QAnimationGroup *> animationGroupList();

    int activeAnimationGroup() const;
    float position() const;
    float positionScale() const;
    float positionOffset() const;
    Qt3DCore::QEntity *entity() const;
    bool recursive() const;

    void setAnimationGroups(const QList<Qt3DAnimation::QAnimationGroup *> &animationGroups);
    void addAnimationGroup(Qt3DAnimation::QAnimationGroup *animationGroups);
    void removeAnimationGroup(Qt3DAnimation::QAnimationGroup *animationGroups);

    Q_INVOKABLE int getAnimationIndex(const QString &name) const;
    Q_INVOKABLE Qt3DAnimation::QAnimationGroup *getGroup(int index) const;

public Q_SLOTS:
    void setActiveAnimationGroup(int index);
    void setPosition(float position);
    void setPositionScale(float scale);
    void setPositionOffset(float offset);
    void setEntity(Qt3DCore::QEntity *entity);
    void setRecursive(bool recursive);

Q_SIGNALS:
    void activeAnimationGroupChanged(int index);
    void positionChanged(float position);
    void positionScaleChanged(float scale);
    void positionOffsetChanged(float offset);
    void entityChanged(Qt3DCore::QEntity *entity);
    void recursiveChanged(bool recursive);

private:
    Q_DECLARE_PRIVATE(QAnimationController)
};

} // Qt3DAnimation

QT_END_NAMESPACE

#endif // QT3DANIMATION_QANIMATIONCONTROLLER_H
