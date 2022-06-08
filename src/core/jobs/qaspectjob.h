// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DCORE_QASPECTJOB_H
#define QT3DCORE_QASPECTJOB_H

#include <Qt3DCore/qt3dcore_global.h>
#include <QtCore/QSharedPointer>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

class QAspectJobPrivate;
class QAspectEngine;

class Q_3DCORESHARED_EXPORT QAspectJob
{
public:
    QAspectJob();
    virtual ~QAspectJob();

    void addDependency(QWeakPointer<QAspectJob> dependency);
    void removeDependency(QWeakPointer<QAspectJob> dependency);

    const std::vector<QWeakPointer<QAspectJob>> &dependencies() const;

    virtual void run() = 0;
    virtual void postFrame(QAspectEngine *aspectEngine);
    virtual bool isRequired();

protected:
    explicit QAspectJob(QAspectJobPrivate &dd);
    QAspectJobPrivate *d_ptr;

private:
    Q_DISABLE_COPY(QAspectJob)
    Q_DECLARE_PRIVATE(QAspectJob)
};

typedef QSharedPointer<QAspectJob> QAspectJobPtr;

} // namespace Qt3DCore

QT_END_NAMESPACE

#endif // QT3DCORE_QASPECTJOB_H
