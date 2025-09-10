// Copyright (C) 2020 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qabstractfrontendnodemanager_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

QAbstractFrontEndNodeManager::QAbstractFrontEndNodeManager() = default;

QAbstractFrontEndNodeManager::~QAbstractFrontEndNodeManager() = default;

/*
\fn QNode *Qt3DCore::QAbstractFrontEndNodeManager::lookupNode(QNodeId id) const

Returns the node instance matching the id, or nullptr if not found.

*/

/*
\fn QList<QNode *> Qt3DCore::QAbstractFrontEndNodeManager::lookupNodes(const QList<QNodeId> &ids) const

Returns the vector of node instance matching the ids.

*/


} // Qt3DCore

QT_END_NAMESPACE
