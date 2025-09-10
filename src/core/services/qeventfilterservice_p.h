// Copyright (C) 2015 Paul Lemire (paul.lemire350@gmail.com)
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DCORE_QEVENTFILTERSERVICE_H
#define QT3DCORE_QEVENTFILTERSERVICE_H

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
#include <Qt3DCore/private/qservicelocator_p.h>

#include <vector>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

class QEventFilterServicePrivate;

class Q_3DCORESHARED_EXPORT QEventFilterService : public QAbstractServiceProvider
{
    Q_OBJECT
public:
    QEventFilterService();
    ~QEventFilterService();

    void initialize(QObject *eventSource);
    void shutdown(QObject *eventSource);
    void registerEventFilter(QObject *eventFilter, int priority);
    void unregisterEventFilter(QObject *eventFilter);

private:
    Q_DECLARE_PRIVATE(QEventFilterService)
};

} // Qt3DCore

QT_END_NAMESPACE

#endif // QT3DCORE_QEVENTFILTERSERVICE_H
