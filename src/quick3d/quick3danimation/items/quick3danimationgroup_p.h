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

#ifndef QT3DANIMATION_QUICK_QUICK3DANIMATIONGROUP_P_H
#define QT3DANIMATION_QUICK_QUICK3DANIMATIONGROUP_P_H

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
#include <Qt3DAnimation/qabstractanimation.h>
#include <Qt3DAnimation/qanimationgroup.h>

#include <private/qt3dquickanimation_global_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {
namespace Quick {

class Q_3DQUICKANIMATIONSHARED_PRIVATE_EXPORT QQuick3DAnimationGroup : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<Qt3DAnimation::QAbstractAnimation> animations READ animations)

public:

    explicit QQuick3DAnimationGroup(QObject *parent = nullptr);

    inline Qt3DAnimation::QAnimationGroup *parentAnimationGroup() const
    {
        return qobject_cast<Qt3DAnimation::QAnimationGroup *>(parent());
    }

    QQmlListProperty<Qt3DAnimation::QAbstractAnimation> animations();

private:

    static void appendAnimation(QQmlListProperty<Qt3DAnimation::QAbstractAnimation> *list, Qt3DAnimation::QAbstractAnimation *animation);
    static Qt3DAnimation::QAbstractAnimation *animationAt(QQmlListProperty<Qt3DAnimation::QAbstractAnimation> *list, int index);
    static int animationCount(QQmlListProperty<Qt3DAnimation::QAbstractAnimation> *list);
    static void clearAnimation(QQmlListProperty<Qt3DAnimation::QAbstractAnimation> *list);
};

} // namespace Quick
} // namespace Qt3DAnimation

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_QUICK_QUICK3DEFFECT_P_H
