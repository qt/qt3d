// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qnodevisitor_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

QNodeVisitor::QNodeVisitor()
{
}

QNodeVisitor::~QNodeVisitor()
{
}

QNode* QNodeVisitor::rootNode() const
{
    return m_path.front();
}

QNode* QNodeVisitor::currentNode() const
{
    return m_path.back();
}

void QNodeVisitor::setPath(QNodeVector path)
{
    m_path = path;
}

QNodeVector QNodeVisitor::path() const
{
    return m_path;
}

void QNodeVisitor::append(QNode *n)
{
    m_path.append(n);
}

void QNodeVisitor::pop_back()
{
    m_path.pop_back();
}

} // namespace Qt3DCore

QT_END_NAMESPACE
