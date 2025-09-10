// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DANIMATION_QUICK_QUICK3DANIMATIONCONTROLLER_P_H
#define QT3DANIMATION_QUICK_QUICK3DANIMATIONCONTROLLER_P_H

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

#include <private/qt3dquickanimation_global_p.h>
#include <QtQml/QQmlListProperty>
#include <Qt3DAnimation/QAnimationController>

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {
namespace Quick {

class Q_3DQUICKANIMATIONSHARED_PRIVATE_EXPORT QQuick3DAnimationController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<Qt3DAnimation::QAnimationGroup> animationGroups READ animationGroups)

public:

    explicit QQuick3DAnimationController(QObject *parent = nullptr);

    inline Qt3DAnimation::QAnimationController *parentAnimationController() const
    {
        return qobject_cast<Qt3DAnimation::QAnimationController *>(parent());
    }

    QQmlListProperty<Qt3DAnimation::QAnimationGroup> animationGroups();
};

} // namespace Quick
} // namespace Qt3DAnimation

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_QUICK_QUICK3DEFFECT_P_H
