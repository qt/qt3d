// Copyright (C) 2015 Paul Lemire
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DCORE_QABSTRACTFRAMEADVANCESERVICE_P_H
#define QT3DCORE_QABSTRACTFRAMEADVANCESERVICE_P_H

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

#include <Qt3DCore/qt3dcore_global.h>
#include <QtCore/QString>

#include <Qt3DCore/private/qservicelocator_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

class QAbstractFrameAdvanceServicePrivate;

class Q_3DCORESHARED_EXPORT QAbstractFrameAdvanceService : public QAbstractServiceProvider
{
    Q_OBJECT
public:
    virtual qint64 waitForNextFrame() = 0;
    virtual void start() = 0;
    virtual void stop() = 0;

protected:
    QAbstractFrameAdvanceService(const QString &description = QString());
    QAbstractFrameAdvanceService(QAbstractFrameAdvanceServicePrivate &dd);
};

} // Qt3D

QT_END_NAMESPACE

#endif // QT3DCORE_QABSTRACTFRAMEADVANCESERVICE_P_H

