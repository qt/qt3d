// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qbackendnodetester.h"
#include <Qt3DCore/qbackendnode.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

QBackendNodeTester::QBackendNodeTester(QObject *parent)
    : QObject(parent)
{
}

void QBackendNodeTester::setPeerId(QBackendNode *backend, QNodeId id)
{
    Q_ASSERT(backend);
    backend->setPeerId(id);
}

} // namespace Qt3DCore

QT_END_NAMESPACE

#include "moc_qbackendnodetester.cpp"
