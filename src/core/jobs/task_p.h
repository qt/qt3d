// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DCORE_TASK_P_H
#define QT3DCORE_TASK_P_H

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

#include <QtCore/QRunnable>
#include <QtCore/QSharedPointer>
#include <QtCore/QThread>
#include <QtCore/QtGlobal>

#include <Qt3DCore/private/qaspectjobmanager_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

class JobRunner;
class DependencyHandler;
class QThreadPooler;
class QSystemInformationService;

class RunnableInterface : public QRunnable
{
public:
    enum class RunnableType {
        AspectTask,
        SyncTask
    };

    virtual ~RunnableInterface();

    virtual bool isRequired() const = 0;
    virtual void run() override = 0;

    virtual int id() = 0;
    virtual void setId(int id) = 0;

    virtual void setReserved(bool reserved) = 0;
    virtual bool reserved() = 0;

    virtual void setPooler(QThreadPooler *pooler) = 0;

    virtual RunnableType type() const = 0;
};

class AspectTaskRunnable : public RunnableInterface
{
public:
    AspectTaskRunnable(QSystemInformationService *service);
    ~AspectTaskRunnable();

    bool isRequired() const override;
    void run() override;

    void setPooler(QThreadPooler *pooler) override { m_pooler = pooler; }

    void setReserved(bool reserved) override { m_reserved = reserved; }
    bool reserved() override { return m_reserved; }

    int id() override { return m_id; }
    void setId(int id) override { m_id = id; }

    RunnableType type() const override { return RunnableType::AspectTask; }

public:
    QSharedPointer<QAspectJob> m_job;
    QList<AspectTaskRunnable *> m_dependers;
    int m_dependerCount = 0;

private:
    QSystemInformationService *m_service;
    QThreadPooler *m_pooler;
    int m_id; // For testing purposes for now
    bool m_reserved;
};

class SyncTaskRunnable : public RunnableInterface
{
public:
    explicit SyncTaskRunnable(QAbstractAspectJobManager::JobFunction func, void *arg,
                              QAtomicInt *atomicCount);
    ~SyncTaskRunnable();

    bool isRequired() const override;
    void run() override;

    void setPooler(QThreadPooler *pooler) override { m_pooler = pooler; }

    void setReserved(bool reserved) override { m_reserved = reserved; }
    bool reserved() override { return m_reserved; }

    int id() override { return m_id; }
    void setId(int id) override { m_id = id; }

    RunnableType type() const override { return RunnableType::SyncTask; }

private:
    QAbstractAspectJobManager::JobFunction m_func;
    void *m_arg;
    QAtomicInt *m_atomicCount;

    QThreadPooler *m_pooler;
    bool m_reserved;

    int m_id;
};

} // namespace Qt3DCore

QT_END_NAMESPACE

#endif // QT3DCORE_TASK_P_H

