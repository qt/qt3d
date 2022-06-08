// Copyright (C) 2015 Paul Lemire
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DCORE_QTICKCLOCKSERVICE_P_H
#define QT3DCORE_QTICKCLOCKSERVICE_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <Qt3DCore/private/qabstractframeadvanceservice_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

class QTickClockServicePrivate;

class QTickClockService : public QAbstractFrameAdvanceService
{
    Q_OBJECT
public:
    QTickClockService();
    ~QTickClockService();

    qint64 waitForNextFrame() final;
    void start() final;
    void stop() final;

    Q_DECLARE_PRIVATE(QTickClockService)
};

} // Qt3D

QT_END_NAMESPACE

#endif // QT3DCORE_QTICKCLOCKSERVICE_P_H
