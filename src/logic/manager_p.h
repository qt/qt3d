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

#ifndef QT3DLOGIC_LOGIC_MANAGER_H
#define QT3DLOGIC_LOGIC_MANAGER_H

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

#include <Qt3DLogic/qt3dlogic_global.h>
#include <Qt3DCore/qnodeid.h>
#include <QtCore/qmutex.h>
#include <QtCore/qscopedpointer.h>
#include <QtCore/qsemaphore.h>

#include <Qt3DLogic/private/handle_types_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DLogic {

class QLogicAspect;

namespace Logic {

class Executor;
class HandlerManager;

class Manager
{
public:
    Manager();
    ~Manager();

    void setLogicAspect(QLogicAspect *logicAspect) { m_logicAspect = logicAspect; }
    void setExecutor(Executor *executor);

    HandlerManager *logicHandlerManager() const { return m_logicHandlerManager.data(); }

    void appendHandler(Handler *handler);
    void removeHandler(Qt3DCore::QNodeId id);
    bool hasFrameActions() const;

    void triggerLogicFrameUpdates();

    void setDeltaTime(float dt) { m_dt = dt; }

private:
    QScopedPointer<HandlerManager> m_logicHandlerManager;
    QVector<HHandler> m_logicHandlers;
    QVector<Qt3DCore::QNodeId> m_logicComponentIds;
    QLogicAspect *m_logicAspect;
    Executor *m_executor;
    float m_dt;
};

} // namespace Logic
} // namespace Qt3DLogic

QT_END_NAMESPACE

#endif // QT3DLOGIC_LOGIC_MANAGER_H
