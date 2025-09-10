// Copyright (C) 2020 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
#ifndef QT3DCORE_QSCHEDULER_P_H
#define QT3DCORE_QSCHEDULER_P_H

#include <Qt3DCore/qt3dcore_global.h>
#include <QtCore/QObject>
#include <private/qglobal_p.h>

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

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

class QAspectManager;

class Q_AUTOTEST_EXPORT QScheduler : public QObject
{
    Q_OBJECT
public:
    explicit QScheduler(QObject *parent = nullptr);
    ~QScheduler();

    void setAspectManager(QAspectManager *aspectManager);
    QAspectManager *aspectManager() const;

    virtual int scheduleAndWaitForFrameAspectJobs(qint64 time, bool dumpJobs);

private:
    QAspectManager *m_aspectManager;
};

} // namespace Qt3DCore

QT_END_NAMESPACE

#endif // QT3DCORE_QSCHEDULER_P_H
