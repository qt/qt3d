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
******************************************************************************/

#ifndef QT3DCORE_QASPECTJOB_P_H
#define QT3DCORE_QASPECTJOB_P_H

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

#include <QtCore/QWeakPointer>

#include <Qt3DCore/private/qt3dcore_global_p.h>
#include <Qt3DCore/private/qsysteminformationservice_p_p.h>
#include <Qt3DCore/qt3dcore-config.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

class QAspectJob;
class QAspectManager;

class Q_3DCORE_PRIVATE_EXPORT QAspectJobPrivate
{
public:
    QAspectJobPrivate();
    virtual ~QAspectJobPrivate();

    static QAspectJobPrivate *get(QAspectJob *job);

    virtual bool isRequired() const;
    virtual void postFrame(QAspectManager *aspectManager);

    void clearDependencies() { m_dependencies.clear(); }

    std::vector<QWeakPointer<QAspectJob> > m_dependencies;
    JobId m_jobId;
    QString m_jobName;
};
} // Qt3D

#define SET_JOB_RUN_STAT_TYPE(job, type, instance) \
    { \
        auto djob = Qt3DCore::QAspectJobPrivate::get(job); \
        auto &jobId = djob->m_jobId; \
        jobId.typeAndInstance[0] = type; \
        jobId.typeAndInstance[1] = instance; \
        djob->m_jobName = QLatin1String(#type); \
    }

#define SET_JOB_RUN_STAT_TYPE_AND_NAME(job, type, name, instance) \
    { \
        auto djob = Qt3DCore::QAspectJobPrivate::get(job); \
        auto &jobId = djob->m_jobId; \
        jobId.typeAndInstance[0] = type; \
        jobId.typeAndInstance[1] = instance; \
        djob->m_jobName = QLatin1String(name); \
    }

QT_END_NAMESPACE

#endif // QT3DCORE_QASPECTJOB_P_H
