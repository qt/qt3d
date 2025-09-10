// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DANIMATION_QANIMATIONCONTROLLER_P_H
#define QT3DANIMATION_QANIMATIONCONTROLLER_P_H

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

#include <private/qobject_p.h>
#include <Qt3DAnimation/QAnimationGroup>

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {

class QAnimationControllerPrivate : public QObjectPrivate
{
public:
    QAnimationControllerPrivate();

    QString m_name;
    int m_activeAnimationGroup;
    QList<QAnimationGroup *> m_animationGroups;
    float m_position;
    float m_scaledPosition;
    float m_positionScale;
    float m_positionOffset;
    Qt3DCore::QEntity *m_entity;
    bool m_recursive;

    void updatePosition(float position);
    void extractAnimations();
    void clearAnimations();
    QAnimationGroup *findGroup(const QString &name);
    float scaledPosition(float position) const;

    Q_DECLARE_PUBLIC(QAnimationController)
};

} // Qt3DAnimation

QT_END_NAMESPACE

#endif // QT3DANIMATION_QANIMATIONCONTROLLER_P_H
