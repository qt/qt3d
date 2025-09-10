// Copyright (C) 2020 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
#ifndef QABSTRACTASPECTJOBMANAGER_P_H
#define QABSTRACTASPECTJOBMANAGER_P_H

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

#include <Qt3DCore/qaspectjob.h>
#include <QtCore/QObject>
#include <private/qglobal_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

class Q_3DCORESHARED_EXPORT QAbstractAspectJobManager : public QObject
{
    Q_OBJECT
public:
    explicit QAbstractAspectJobManager(QObject *p = nullptr);

    virtual void initialize() {}
    virtual void enqueueJobs(const std::vector<QAspectJobPtr> &jobQueue) = 0;
    virtual int waitForAllJobs() = 0;

    // Callback signature for running SynchronizedJobs
    typedef void (*JobFunction)(void *);
    virtual void waitForPerThreadFunction(JobFunction func, void *arg) = 0;
};

} // namespace Qt3DCore

QT_END_NAMESPACE

#endif // QABSTRACTASPECTJOBMANAGER_P_H
