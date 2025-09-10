// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

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

    QList<QAbstractActionInput *> m_inputs;
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
    QList<Qt3DCore::QNodeId> inputIds;
};

} // namespace Qt3DInput

QT_END_NAMESPACE

#endif // QT3DINPUT_QACTION_P_H

