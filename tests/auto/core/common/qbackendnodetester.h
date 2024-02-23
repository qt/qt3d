// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#ifndef QT3DCORE_QBACKENDNODETESTER_H
#define QT3DCORE_QBACKENDNODETESTER_H

#include <QObject>
#include <Qt3DCore/qnodeid.h>
#include <Qt3DCore/qnode.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

class QBackendNode;

class QBackendNodeTester : public QObject
{
    Q_OBJECT
public:
    explicit QBackendNodeTester(QObject *parent = 0);

    // Proxies to allow test classes to call private methods on QBackendNode
    void setPeerId(QBackendNode *backend, QNodeId id);

    template<class Backend>
    void simulateInitializationSync(QNode *frontend, Backend *backend)
    {
        Q_ASSERT(frontend);
        Q_ASSERT(backend);

        backend->setPeerId(frontend->id());
        backend->setEnabled(frontend->isEnabled());
        backend->syncFromFrontEnd(frontend, true);
    }
};

} // namespace Qt3DCore

QT_END_NAMESPACE

#endif // QT3DCORE_QBACKENDNODETESTER_H
