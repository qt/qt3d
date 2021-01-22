/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
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
****************************************************************************/

#ifndef QT3DRENDER_QPICKEVENT_P_H
#define QT3DRENDER_QPICKEVENT_P_H

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

#include <Qt3DCore/qnodeid.h>

#include <private/qobject_p.h>
#include <private/qt3drender_global_p.h>


QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QPickEvent;
class QViewport;

class Q_3DRENDERSHARED_PRIVATE_EXPORT QPickEventPrivate : public QObjectPrivate
{
public:
    QPickEventPrivate()
        : QObjectPrivate()
        , m_accepted(true)
        , m_distance(-1.f)
        , m_button(QPickEvent::NoButton)
        , m_buttons(QPickEvent::NoButton)
        , m_modifiers(QPickEvent::NoModifier)
        , m_entityPtr(nullptr)
        , m_viewport(nullptr)
    {
    }

    bool m_accepted;
    QPointF m_position;
    QVector3D m_worldIntersection;
    QVector3D m_localIntersection;
    float m_distance;
    QPickEvent::Buttons m_button;
    int m_buttons;
    int m_modifiers;
    Qt3DCore::QNodeId m_entity;
    Qt3DCore::QEntity *m_entityPtr;
    QViewport *m_viewport;

    static QPickEventPrivate *get(QPickEvent *object);
};

} // Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QPICKEVENT_P_H
