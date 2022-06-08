// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DINPUT_QBUTTONAXISINPUT_P_H
#define QT3DINPUT_QBUTTONAXISINPUT_P_H

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

#include <Qt3DInput/private/qabstractaxisinput_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {

class QButtonAxisInputPrivate : public QAbstractAxisInputPrivate
{
public:
    QButtonAxisInputPrivate()
        : QAbstractAxisInputPrivate()
        , m_scale(1.0f)
        , m_acceleration(-1.0f)
        , m_deceleration(-1.0f)
    {}

    QList<int> m_buttons;
    float m_scale;
    float m_acceleration;
    float m_deceleration;
};

struct QButtonAxisInputData : public QAbstractAxisInputData
{
    QList<int> buttons;
    float scale;
    float acceleration;
    float deceleration;
};

} // Qt3DInput

QT_END_NAMESPACE

#endif // QT3DINPUT_QBUTTONAXISINPUT_P_H

