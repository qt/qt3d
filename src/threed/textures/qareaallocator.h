/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtQuick3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QAREAALLOCATOR_P_H
#define QAREAALLOCATOR_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "qt3dglobal.h"
#include <QtCore/qsize.h>
#include <QtCore/qrect.h>
#include <QtCore/qlist.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3D)

class Q_QT3D_EXPORT QAreaAllocator
{
public:
    QAreaAllocator(const QSize &size);
    virtual ~QAreaAllocator();

    QSize size() const { return m_size; }

    QSize minimumAllocation() const { return m_minAlloc; }
    void setMinimumAllocation(const QSize &size) { m_minAlloc = size; }

    QSize margin() const { return m_margin; }
    void setMargin(const QSize &margin) { m_margin = margin; }

    virtual void expand(const QSize &size);
    void expandBy(const QSize &size);

    virtual QRect allocate(const QSize &size) = 0;
    virtual QList<QRect> allocate(const QList<QSize> &sizes);
    virtual void release(const QRect &rect);
    virtual void release(const QList<QRect> &rects);

    virtual int overhead() const;

protected:
    QSize m_size;
    QSize m_minAlloc;
    QSize m_margin;

    QSize roundAllocation(const QSize &size) const;
};

class Q_QT3D_EXPORT QSimpleAreaAllocator : public QAreaAllocator
{
public:
    QSimpleAreaAllocator(const QSize &size);
    virtual ~QSimpleAreaAllocator();

    QRect allocate(const QSize &size);

private:
    int m_row;
    int m_column;
    int m_rowHeight;
};

class Q_QT3D_EXPORT QGeneralAreaAllocator : public QAreaAllocator
{
public:
    QGeneralAreaAllocator(const QSize &size);
    virtual ~QGeneralAreaAllocator();

    void expand(const QSize &size);
    QRect allocate(const QSize &size);
    void release(const QRect &rect);
    int overhead() const;

private:
    enum Split { SplitOnX, SplitOnY };

    struct Node
    {
        QRect rect;
        QSize largestFree;
        Node *parent;
        Node *left;
        Node *right;
    };

    Node *m_root;
    int m_nodeCount;

    static void freeNode(Node *node);
    QPoint allocateFromNode(const QSize &size, Node *node);
    Node *splitNode(Node *node, Split split);
    static void updateLargestFree(Node *node);
};

class Q_QT3D_EXPORT QUniformAreaAllocator : public QAreaAllocator
{
public:
    QUniformAreaAllocator(const QSize &size, const QSize &uniformSize);
    virtual ~QUniformAreaAllocator();

    QSize uniformSize() const { return m_uniformSize; }

    void expand(const QSize &size);
    QRect allocate(const QSize &size);
    void release(const QRect &rect);
    int overhead() const;

private:
    QSize m_uniformSize;
    QSize m_gridSize;
    int *m_grid;
    int m_firstFree;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif
