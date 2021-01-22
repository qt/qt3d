/****************************************************************************
**
** Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
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

#ifndef QT3DINPUT_QACTION_P_H
#define QT3DINPUT_QACTION_P_H

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

#include <Qt3DCore/private/qnode_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {

/*!
    \class Qt3DInput::QActionPrivate
    \internal
*/
class QActionPrivate : public Qt3DCore::QNodePrivate
{
public:
    QActionPrivate()
        : Qt3DCore::QNodePrivate()
        , m_active(false)
    {}

    Q_DECLARE_PUBLIC(QAction)

    QVector<QAbstractActionInput *> m_inputs;
    bool m_active;

    void setActive(bool active)
    {
        if (active != m_active) {
            m_active = active;
            Q_EMIT q_func()->activeChanged(active);
        }
    }
};

struct QActionData
{
    QVector<Qt3DCore::QNodeId> inputIds;
};

} // namespace Qt3DInput

QT_END_NAMESPACE

#endif // QT3DINPUT_QACTION_P_H

