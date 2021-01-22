/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
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

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

class Q_3DCORESHARED_EXPORT QAbstractAspectJobManager : public QObject
{
    Q_OBJECT
public:
    explicit QAbstractAspectJobManager(QObject *p = nullptr);

    virtual void initialize() {}
    virtual void enqueueJobs(const QVector<QAspectJobPtr> &jobQueue) = 0;
    virtual int waitForAllJobs() = 0;

    // Callback signature for running SynchronizedJobs
    typedef void (*JobFunction)(void *);
    virtual void waitForPerThreadFunction(JobFunction func, void *arg) = 0;
};

} // namespace Qt3DCore

QT_END_NAMESPACE

#endif // QABSTRACTASPECTJOBMANAGER_P_H
