/****************************************************************************
**
** Copyright (C) 2020 Klaralvdalens Datakonsult AB (KDAB).
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
******************************************************************************/

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
