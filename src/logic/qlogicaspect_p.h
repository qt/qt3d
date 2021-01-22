/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
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
    bool m_initialized;
    QScopedPointer<Logic::Manager> m_manager;
    QScopedPointer<Logic::Executor> m_executor;
    QSharedPointer<Logic::CallbackJob> m_callbackJob;
};

} // namespace Qt3DLogic

QT_END_NAMESPACE

#endif // QT3DLOGIC_QLOGICASPECT_P_H

