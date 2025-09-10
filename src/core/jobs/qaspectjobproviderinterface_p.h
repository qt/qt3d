// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DCORE_QASPECTJOBPROVIDERINTERFACE_P_H
#define QT3DCORE_QASPECTJOBPROVIDERINTERFACE_P_H

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

#include <Qt3DCore/private/qt3dcore_global_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

class QScheduler;

class QAspectJobProviderInterface
{
public:
    virtual ~QAspectJobProviderInterface() {}

private:
    virtual std::vector<QAspectJobPtr> jobsToExecute(qint64 time) = 0;
    virtual void jobsDone() = 0;
    virtual void frameDone() = 0;

    friend class QScheduler;
};

}

QT_END_NAMESPACE

#endif // QT3DCORE_QASPECTJOBPROVIDERINTERFACE_P_H
