// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qdestructionidandtypecollector_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

QDestructionIdAndTypeCollector::QDestructionIdAndTypeCollector(QNode *rootNode)
{
    QNodeVisitor visitor;
    visitor.traverse(rootNode, this, &QDestructionIdAndTypeCollector::collectIdAndType);
}

QT_END_NAMESPACE

} // namespace Qt3DCore
