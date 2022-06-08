// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DINPUT_QANALOGAXISINPUT_P_H
#define QT3DINPUT_QANALOGAXISINPUT_P_H

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

class QAnalogAxisInputPrivate : public QAbstractAxisInputPrivate
{
public:
    QAnalogAxisInputPrivate()
        : QAbstractAxisInputPrivate()
        , m_axis(-1)
    {}

    int m_axis;
};

struct QAnalogAxisInputData : public QAbstractAxisInputData
{
    int axis;
};

} // Qt3DInput

QT_END_NAMESPACE

#endif // QT3DINPUT_QBUTTONAXISINPUT_P_H

