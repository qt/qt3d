/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
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

#ifndef QGLPICKNODE_H
#define QGLPICKNODE_H

#include <QtCore/qobject.h>
#include <Qt3D/qt3dglobal.h>

QT_BEGIN_NAMESPACE

class QGLAbstractScene;
class QGLSceneNode;
class QEvent;

class Q_QT3D_EXPORT QGLPickNode : public QObject
{
    Q_OBJECT
public:
    explicit QGLPickNode(QGLAbstractScene *parent = 0);
    int id() const { return m_id; }
    void setId(int id_) { m_id = id_; }

    QGLSceneNode *target() const { return m_target; }
    void setTarget(QGLSceneNode *target_) { m_target = target_; }

Q_SIGNALS:
    void pressed();
    void released();
    void clicked();
    void doubleClicked();
    void hoverChanged();

public Q_SLOTS:

protected:
    bool event(QEvent *e);
    int m_id;
    QGLSceneNode *m_target;
};

QT_END_NAMESPACE

#endif // QGLPICKNODE_H
