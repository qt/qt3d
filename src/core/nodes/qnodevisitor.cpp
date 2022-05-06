/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
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
