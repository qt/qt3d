// Copyright (C) 2016 Paul Lemire
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_GENERICLAMBDAJOB_H
#define QT3DRENDER_RENDER_GENERICLAMBDAJOB_H

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
#include <Qt3DRender/private/job_common_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

template<typename T>
class GenericLambdaJob : public Qt3DCore::QAspectJob
{
public:
    explicit GenericLambdaJob(T callable, JobTypes::JobType type = JobTypes::GenericLambda, const char *name = "GenericLambda", int instance = 0)
        : Qt3DCore::QAspectJob()
        , m_callable(callable)
    {
        SET_JOB_RUN_STAT_TYPE_AND_NAME(this, type, name, instance)
    }

    // QAspectJob interface
    void run() final
    {
        m_callable();
    }

private:
    T m_callable;
};

template<typename T>
using GenericLambdaJobPtr = QSharedPointer<GenericLambdaJob<T>>;

template<typename T, typename U>
class GenericLambdaJobAndPostFramePrivate : public Qt3DCore::QAspectJobPrivate
{
public:
    explicit GenericLambdaJobAndPostFramePrivate(U postFrameCallable)
        : m_postFrameCallable(postFrameCallable)
    {}

    ~GenericLambdaJobAndPostFramePrivate() override {}

    void postFrame(Qt3DCore::QAspectManager *manager) override
    {
        m_postFrameCallable(manager);
    }

private:
    U m_postFrameCallable;
};

template<typename T, typename U>
class GenericLambdaJobAndPostFrame : public Qt3DCore::QAspectJob
{
public:
    explicit GenericLambdaJobAndPostFrame(T runCallable, U postFrameCallable, JobTypes::JobType type = JobTypes::GenericLambda, const char *name = "GenericLambda")
        : Qt3DCore::QAspectJob(*new GenericLambdaJobAndPostFramePrivate<T, U>(postFrameCallable))
        , m_runCallable(runCallable)
    {
        SET_JOB_RUN_STAT_TYPE_AND_NAME(this, type, name, 0)
    }

    // QAspectJob interface
    void run() final
    {
        m_runCallable();
    }

private:
    T m_runCallable;
};

template<typename T, typename U>
using GenericLambdaJobAndPostFramePtr = QSharedPointer<GenericLambdaJobAndPostFrame<T, U>>;

} // Render

} // Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_GENERICLAMBDAJOB_H
