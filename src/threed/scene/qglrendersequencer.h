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


#ifndef QGLRENDERSEQUENCER_H
#define QGLRENDERSEQUENCER_H

#include <QtCore/qlinkedlist.h>
#include <QtCore/qset.h>

#include <Qt3D/qglrenderorder.h>

QT_BEGIN_NAMESPACE

class QGLSceneNode;
class QGLPainter;
class QGLRenderOrderComparator;
class QGLRenderSequencerPrivate;

class Q_QT3D_EXPORT QGLRenderSequencer
{
public:
    explicit QGLRenderSequencer(QGLPainter *painter);
    void setPainter(QGLPainter *painter);
    bool renderInSequence(QGLSceneNode *node);
    bool nextInSequence();
    void beginState(QGLSceneNode *node);
    void endState(QGLSceneNode *node);
    void reset();
    QGLSceneNode *top() const;
    void setTop(QGLSceneNode *top);
    QGLRenderOrderComparator *comparator() const;
    void setComparator(QGLRenderOrderComparator *comparator);
    void applyState();
private:
    void insertNew(const QGLRenderOrder &order);

    QGLRenderSequencerPrivate *d;
};

QT_END_NAMESPACE

#endif // QGLRENDERSEQUENCER_H
