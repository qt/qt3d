/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "nodevisitor.h"

#include "node.h"
#include "entity.h"

namespace Qt3D {

NodeVisitor::NodeVisitor()
    : m_traverseDisabled(false)
{
}

void NodeVisitor::traverse(Node *rootNode)
{
    m_path = NodeList() << rootNode;

    m_matrixStack.clear();
    Entity* rootEntity = rootNode->asEntity();

    m_matrixStack.append(rootEntity ? rootEntity->matrix() : QMatrix4x4());

    if (rootEntity)
        visitEntity(rootEntity);
    else
        visitNode(rootNode);
}

Node* NodeVisitor::rootNode() const
{
    return m_path.front();
}

Node* NodeVisitor::currentNode() const
{
    return m_path.back();
}

QMatrix4x4 NodeVisitor::currentMatrix() const
{
    return m_matrixStack.back();
}

NodeList NodeVisitor::path() const
{
    return m_path;
}

void NodeVisitor::setTraverseDisabled(bool on)
{
    m_traverseDisabled = on;
}

void NodeVisitor::visitNode(Node *nd)
{
    Q_UNUSED(nd);
    traverseChildren();
}

void NodeVisitor::visitEntity(Entity *nd)
{
    visitNode(nd);
}

void NodeVisitor::traverseChildren()
{
    foreach (Node* n, currentNode()->children()) {
        outerVisitNode(n);
    } // of children iteration
}

void NodeVisitor::outerVisitNode(Node *n)
{
    m_path.append(n);
    Entity* e = n->asEntity();
    if (e) {
        QMatrix4x4 m = m_matrixStack.back() * e->matrix();
        m_matrixStack.push_back(m);

        visitEntity(e);

        m_path.pop_back();
        m_matrixStack.pop_back();
    } else {
        visitNode(n);
    }
}

} // of namespace
