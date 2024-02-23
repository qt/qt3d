// Copyright (C) 2020 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include <Qt3DCore/private/qchangearbiter_p.h>
#include <Qt3DCore/private/qnode_p.h>

QT_BEGIN_NAMESPACE

class TestArbiter : public Qt3DCore::QChangeArbiter
{
public:

    void setArbiterOnNode(Qt3DCore::QNode *node)
    {
        Qt3DCore::QNodePrivate::get(node)->setArbiter(this);
        const auto childNodes = node->childNodes();
        for (Qt3DCore::QNode *n : childNodes)
            setArbiterOnNode(n);
    }

    QList<Qt3DCore::QNode *> dirtyNodes() const { return m_dirtyFrontEndNodes; }
    QList<Qt3DCore::ComponentRelationshipChange> dirtyComponents() const { return m_dirtyEntityComponentNodeChanges; }

    void clear()
    {
        m_dirtyFrontEndNodes.clear();
        m_dirtyEntityComponentNodeChanges.clear();
    }
};

QT_END_NAMESPACE
