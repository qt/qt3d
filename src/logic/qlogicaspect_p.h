// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DLOGIC_QLOGICASPECT_P_H
#define QT3DLOGIC_QLOGICASPECT_P_H

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

#include <QtCore/qsharedpointer.h>

#include <Qt3DCore/private/qabstractaspect_p.h>
#include <Qt3DLogic/private/callbackjob_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DLogic {

namespace Logic {
class Executor;
class Manager;
}

class QLogicAspectPrivate : public Qt3DCore::QAbstractAspectPrivate
{
    QLogicAspectPrivate();

    Q_DECLARE_PUBLIC(QLogicAspect)

    void onEngineAboutToShutdown() override;
    void registerBackendTypes();

    qint64 m_time;
    QScopedPointer<Logic::Manager> m_manager;
    QScopedPointer<Logic::Executor> m_executor;
    QSharedPointer<Logic::CallbackJob> m_callbackJob;
};

} // namespace Qt3DLogic

QT_END_NAMESPACE

#endif // QT3DLOGIC_QLOGICASPECT_P_H

