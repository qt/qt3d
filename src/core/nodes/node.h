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

#ifndef QT3D_NODE_H
#define QT3D_NODE_H

#include <QObject>
#include "qt3dcore_global.h"

#include <QQmlListProperty>

namespace Qt3D {

class Node;
class Component;
class Entity;

typedef QList<Node *> NodeList;

class QT3DCORESHARED_EXPORT Node : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<QObject> data READ data)
    Q_PROPERTY(QQmlListProperty<Qt3D::Node> childNodes READ childNodes)
    Q_CLASSINFO("DefaultProperty", "data")

public:
    explicit Node(Node *parent = 0);
    ~Node();

    QQmlListProperty<QObject> data();
    QQmlListProperty<Qt3D::Node> childNodes();

    void dump();

    NodeList children() const;
    void addChild( Node* childNode );
    void removeChild( Node* childNode );

    void removeAllChildren();

    virtual Entity* asEntity();

    Node* parentNode() const;

protected:
    bool event(QEvent *e);

private:
    static void appendData(QQmlListProperty<QObject> *list, QObject *obj);
    static QObject *dataAt(QQmlListProperty<QObject> *list, int index);
    static int dataCount(QQmlListProperty<QObject> *list);
    static void clearData(QQmlListProperty<QObject> *list);

    static void appendChild(QQmlListProperty<Qt3D::Node> *list, Qt3D::Node *obj);
    static Node *childAt(QQmlListProperty<Qt3D::Node> *list, int index);
    static int childCount(QQmlListProperty<Qt3D::Node> *list);
    static void clearChildren(QQmlListProperty<Qt3D::Node> *list);

    NodeList m_children;
};

}

#endif
