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
    QVector<QAnimationGroup *> m_animationGroups;
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
