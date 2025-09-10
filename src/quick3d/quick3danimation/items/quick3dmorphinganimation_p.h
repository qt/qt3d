// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DANIMATION_QUICK_QUICK3DMORPHINGANIMATION_P_H
#define QT3DANIMATION_QUICK_QUICK3DMORPHINGANIMATION_P_H

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

#include <QtQml/qqmllist.h>

#include <Qt3DAnimation/qmorphinganimation.h>

#include <private/qt3dquickanimation_global_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {
namespace Quick {

class Q_3DQUICKANIMATIONSHARED_PRIVATE_EXPORT QQuick3DMorphingAnimation : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<Qt3DAnimation::QMorphTarget> morphTargets READ morphTargets)

public:
    QQuick3DMorphingAnimation(QObject *parent = nullptr);

    inline QMorphingAnimation *parentMorphingAnimation() const
    {
        return qobject_cast<QMorphingAnimation *>(parent());
    }

    QQmlListProperty<Qt3DAnimation::QMorphTarget> morphTargets();
};

} // namespace Quick
} // namespace Qt3DAnimation

QT_END_NAMESPACE

#endif // QT3DANIMATION_QUICK_QUICK3DMORPHINGANIMATION_P_H
