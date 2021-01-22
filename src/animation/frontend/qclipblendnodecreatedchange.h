/****************************************************************************
**
** Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
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
****************************************************************************/

#ifndef QT3DANIMATION_QCLIPBLENDNODECREATEDCHANGE_H
#define QT3DANIMATION_QCLIPBLENDNODECREATEDCHANGE_H

#include <Qt3DCore/qnodecreatedchange.h>
#include <Qt3DAnimation/qabstractclipblendnode.h>

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {

class QAbstractClipBlendNode;
class QClipBlendNodeCreatedChangeBasePrivate;

class Q_3DANIMATIONSHARED_EXPORT QClipBlendNodeCreatedChangeBase : public Qt3DCore::QNodeCreatedChangeBase
{
public:
    explicit QClipBlendNodeCreatedChangeBase(const QAbstractClipBlendNode *node);
    ~QClipBlendNodeCreatedChangeBase();

private:
    Q_DECLARE_PRIVATE(QClipBlendNodeCreatedChangeBase)
};

typedef QSharedPointer<QClipBlendNodeCreatedChangeBase> QClipBlendNodeCreatedChangeBasePtr;

template<typename T>
class QClipBlendNodeCreatedChange : public QClipBlendNodeCreatedChangeBase
{
public:
    explicit QClipBlendNodeCreatedChange(const QAbstractClipBlendNode *node)
        : QClipBlendNodeCreatedChangeBase(node)
        , data()
    {
    }

    T data;
};

template<typename T>
using QClipBlendNodeCreatedChangePtr = QSharedPointer<QClipBlendNodeCreatedChange<T>>;


} // Qt3DAnimation

QT_END_NAMESPACE

#endif // QT3DANIMATION_QCLIPBLENDNODECREATEDCHANGE_H
