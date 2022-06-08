// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DINPUT_QAXIS_P_H
#define QT3DINPUT_QAXIS_P_H

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

class QAbstractAxisInput;

class QAxisPrivate : public Qt3DCore::QNodePrivate
{
public:
    QAxisPrivate()
        : Qt3DCore::QNodePrivate()
        , m_value(0.0f)
    {}

    Q_DECLARE_PUBLIC(QAxis)

    QList<QAbstractAxisInput *> m_inputs;
    float m_value;

    void setValue(float value)
    {
        if (value != m_value) {
            m_value = value;
            Q_EMIT q_func()->valueChanged(m_value);
        }
    }
};

struct QAxisData
{
    QList<Qt3DCore::QNodeId> inputIds;
};

} // Qt3DInput

QT_END_NAMESPACE

#endif // QT3DINPUT_QAXIS_P_H

