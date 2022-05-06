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

#ifndef QT3DANIMATION_QABSTRACTANIMATION_H
#define QT3DANIMATION_QABSTRACTANIMATION_H

#include <QtCore/qlist.h>
#include <QtCore/qobject.h>

#include <Qt3DAnimation/qt3danimation_global.h>

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {

class QAbstractAnimationPrivate;

class Q_3DANIMATIONSHARED_EXPORT QAbstractAnimation : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString animationName READ animationName WRITE setAnimationName NOTIFY animationNameChanged)
    Q_PROPERTY(AnimationType animationType READ animationType CONSTANT)
    Q_PROPERTY(float position READ position WRITE setPosition NOTIFY positionChanged)
    Q_PROPERTY(float duration READ duration NOTIFY durationChanged)

public:
    enum AnimationType {
        KeyframeAnimation = 1,
        MorphingAnimation = 2,
        VertexBlendAnimation = 3,
    };
    Q_ENUM(AnimationType)

    QString animationName() const;
    QAbstractAnimation::AnimationType animationType() const;
    float position() const;
    float duration() const;

public Q_SLOTS:
    void setAnimationName(const QString &name);
    void setPosition(float position);

protected:
    explicit QAbstractAnimation(QAbstractAnimationPrivate &dd, QObject *parent = nullptr);

    void setDuration(float duration);

Q_SIGNALS:
    void animationNameChanged(const QString &name);
    void positionChanged(float position);
    void durationChanged(float duration);

private:
    Q_DECLARE_PRIVATE(QAbstractAnimation)
};

} // Qt3DAnimation

QT_END_NAMESPACE

#endif // QT3DANIMATION_QABSTRACTANIMATION_H
