// Copyright (C) 2019 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
#ifndef QT3DLOGIC_LOGIC_HANDLER_H
#define QT3DLOGIC_LOGIC_HANDLER_H

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

#include <Qt3DCore/qbackendnode.h>
#include <Qt3DCore/qnodeid.h>
#include <Qt3DCore/qaspectjob.h>
#include <private/qglobal_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {
class QNode;
}

namespace Qt3DLogic {
namespace Logic {

class Manager;

class Handler : public Qt3DCore::QBackendNode
{
public:
    Handler();

    void setManager(Manager *manager) { m_logicManager = manager; }
    Manager *logicManager() const { return m_logicManager; }

    void syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime) override;

private:
    Manager *m_logicManager;
};


class HandlerFunctor : public Qt3DCore::QBackendNodeMapper
{
public:
    explicit HandlerFunctor(Manager *handler);

    Qt3DCore::QBackendNode *create(Qt3DCore::QNodeId id) const override;
    Qt3DCore::QBackendNode *get(Qt3DCore::QNodeId id) const override;
    void destroy(Qt3DCore::QNodeId id) const override;

private:
    Manager *m_manager;
};

} // namespace Logic
} // namespace Qt3DLogic

QT_END_NAMESPACE

#endif // QT3DLOGIC_LOGIC_HANDLER_H
