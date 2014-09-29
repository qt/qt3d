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

#include "qnode.h"
#include "qentity.h"

QT_BEGIN_NAMESPACE

namespace Qt3D {

NodeVisitor::NodeVisitor()
    : m_traverseDisabled(false)
{
}

NodeVisitor::~NodeVisitor()
{
}

void NodeVisitor::traverse(QNode *rootNode)
{
    m_path = NodeList() << rootNode;

    m_matrixStack.clear();
    QEntity* rootEntity = qobject_cast<QEntity *>(rootNode);

    if (rootEntity)
        visitEntity(rootEntity);
    else
        visitNode(rootNode);
}

QNode* NodeVisitor::rootNode() const
{
    return m_path.front();
}

QNode* NodeVisitor::currentNode() const
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

void NodeVisitor::visitNode(QNode *nd)
{
    Q_UNUSED(nd);
    traverseChildren();
}

void NodeVisitor::visitEntity(QEntity *nd)
{
    visitNode(nd);
}

void NodeVisitor::traverseChildren()
{
    foreach (QObject *n, currentNode()->children()) {
        QNode *node = qobject_cast<QNode *>(n);
        if (node != Q_NULLPTR)
            outerVisitNode(node);
    } // of children iteration
}

void NodeVisitor::outerVisitNode(QNode *n)
{
    m_path.append(n);
    QEntity* e = qobject_cast<QEntity *>(n);
    if (e) {
        visitEntity(e);
        m_path.pop_back();
    }
    else {
        visitNode(n);
    }
}

} // namespace Qt3D

QT_END_NAMESPACE
