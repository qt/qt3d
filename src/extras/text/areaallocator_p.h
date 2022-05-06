/****************************************************************************
**
** Copyright (C) 2022 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtQuick module of the Qt Toolkit.
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

#ifndef QT3DEXTRAS_AREAALLOCATOR_P_H
#define QT3DEXTRAS_AREAALLOCATOR_P_H

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

//
// This file is copied from qtdeclarative/src/quick/scenegraph/util/qsgareaallocator_p.h
//

#include <QtCore/qsize.h>

QT_BEGIN_NAMESPACE

class QRect;
class QPoint;

namespace Qt3DExtras {

struct AreaAllocatorNode;

class AreaAllocator
{
public:
    AreaAllocator(const QSize &size);
    ~AreaAllocator();

    QRect allocate(const QSize &size);
    bool deallocate(const QRect &rect);
    bool isEmpty() const { return m_root == 0; }
    QSize size() const { return m_size; }
private:
    bool allocateInNode(const QSize &size, QPoint &result, const QRect &currentRect, AreaAllocatorNode *node);
    bool deallocateInNode(const QPoint &pos, AreaAllocatorNode *node);
    void mergeNodeWithNeighbors(AreaAllocatorNode *node);

    AreaAllocatorNode *m_root;
    QSize m_size;
};

} // namespace Qt3DExtras

QT_END_NAMESPACE

#endif // QT3DEXTRAS_AREAALLOCATOR_P_H
