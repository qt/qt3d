// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DANIMATION_QANIMATIONGROUP_H
#define QT3DANIMATION_QANIMATIONGROUP_H

#include <QtCore/qobject.h>

#include <Qt3DAnimation/qabstractanimation.h>

#include <Qt3DAnimation/qt3danimation_global.h>

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {

class QAnimationGroupPrivate;

class Q_3DANIMATIONSHARED_EXPORT QAnimationGroup : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(float position READ position WRITE setPosition NOTIFY positionChanged)
    Q_PROPERTY(float duration READ duration NOTIFY durationChanged)

public:
    explicit QAnimationGroup(QObject *parent = nullptr);

    QString name() const;
    QList<Qt3DAnimation::QAbstractAnimation *> animationList();
    float position() const;
    float duration() const;

    void setAnimations(const QList<Qt3DAnimation::QAbstractAnimation *> &animations);
    void addAnimation(Qt3DAnimation::QAbstractAnimation *animation);
    void removeAnimation(Qt3DAnimation::QAbstractAnimation *animation);

public Q_SLOTS:
    void setName(const QString &name);
    void setPosition(float position);

Q_SIGNALS:
    void nameChanged(const QString &name);
    void positionChanged(float position);
    void durationChanged(float duration);

private:

    Q_DECLARE_PRIVATE(QAnimationGroup)
};

} // Qt3DAnimation

QT_END_NAMESPACE

#endif // QT3DANIMATION_QANIMATIONGROUP_H
