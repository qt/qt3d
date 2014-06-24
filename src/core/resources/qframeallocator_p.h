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

#ifndef QT3D_QFRAMEALLOCATOR_P_H
#define QT3D_QFRAMEALLOCATOR_P_H

#include <Qt3DCore/qt3dcore_global.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

class QFrameAllocator;

struct Q_AUTOTEST_EXPORT QFrameChunk
{
    void init(uint blockSize, uchar blocks);
    void *allocate(uint blockSize);

    void deallocate(void *p, uint blockSize);
    bool contains(void *p, uint blockSize);
    void clear(uint blockSize, uchar blocks);
    void release();

    inline bool isEmpty() const { return m_blocksAvailable == m_maxBlocksAvailable; }

    uchar *m_data;
    uchar  m_firstAvailableBlock;
    uchar  m_blocksAvailable;
    uchar  m_maxBlocksAvailable;
};

class Q_AUTOTEST_EXPORT QFixedFrameAllocator
{
public:
    QFixedFrameAllocator();
    ~QFixedFrameAllocator();

    void init(uint blockSize, uchar pageSize = 128);
    void *allocate();
    void  deallocate(void *ptr);
    void trim();
    void release();
    void clear();

    inline int chunkCount() const { return m_chunks.size(); }
    inline uchar pageSize() const { return m_nbrBlock; }
    inline uint blockSize() const { return m_blockSize; }

private:
    uint m_blockSize;
    uchar m_nbrBlock;
    QVector<QFrameChunk> m_chunks;
    QFrameChunk *m_lastAllocatedChunck;
    QFrameChunk *m_lastFreedChunck;
};

class QFrameAllocatorPrivate
{
public:
    QFrameAllocatorPrivate(QFrameAllocator *qq);

    Q_DECLARE_PUBLIC(QFrameAllocator)
    QFrameAllocator *q_ptr;

    uint m_maxObjectSize;
    uint m_alignment;
    int m_allocatorPoolSize;
    QVector<QFixedFrameAllocator> m_allocatorPool;
};

} // Qt3D

QT_END_NAMESPACE

#endif // QT3D_QFRAMEALLOCATOR_P_H
