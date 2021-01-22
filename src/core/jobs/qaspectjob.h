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

#ifndef QT3DCORE_QASPECTJOB_H
#define QT3DCORE_QASPECTJOB_H

#include <Qt3DCore/qt3dcore_global.h>
#include <QtCore/QSharedPointer>
#include <QtCore/QVector>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

class QAspectJobPrivate;
class QAspectManager;

class Q_3DCORESHARED_EXPORT QAspectJob
{
public:
    QAspectJob();
    virtual ~QAspectJob();

    void addDependency(QWeakPointer<QAspectJob> dependency);
    void removeDependency(QWeakPointer<QAspectJob> dependency);

    QVector<QWeakPointer<QAspectJob> > dependencies() const;

    virtual void run() = 0;
    void postFrame(QAspectManager *aspectManager);

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
