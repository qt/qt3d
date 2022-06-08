// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DCORE_QIDTYPEVISITOR_H
#define QT3DCORE_QIDTYPEVISITOR_H

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

#include <Qt3DCore/private/qentity_p.h>
#include <Qt3DCore/private/qnodevisitor_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

class QDestructionIdAndTypeCollector
{
public:
    QDestructionIdAndTypeCollector(QNode *rootNode);

    QList<QNodeIdTypePair> subtreeIdsAndTypes() const { return m_subtreeIdsAndTypes; }

private:
    void collectIdAndType(QNode *node)
    {
        QNodeIdTypePair idAndType(node->id(), QNodePrivate::get(node)->m_typeInfo);
        m_subtreeIdsAndTypes.push_back(idAndType);

        // Mark this node as having been handled for destruction so we don't
        // repeat it unnecessarily in an O(n^2) manner
        QNodePrivate::get(node)->m_hasBackendNode = false;
    }

    QList<QNodeIdTypePair> m_subtreeIdsAndTypes;
};

} // namespace Qt3DCore

QT_END_NAMESPACE

#endif // QT3DCORE_QIDTYPEVISITOR_H
