// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DANIMATION_QABSTRACTCLIPBLENDNODE_H
#define QT3DANIMATION_QABSTRACTCLIPBLENDNODE_H

#include <Qt3DAnimation/qt3danimation_global.h>
#include <Qt3DAnimation/qabstractanimationclip.h>
#include <Qt3DCore/qnode.h>

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {

class QAbstractClipBlendNodePrivate;

class Q_3DANIMATIONSHARED_EXPORT QAbstractClipBlendNode : public Qt3DCore::QNode
{
    Q_OBJECT
public:
    ~QAbstractClipBlendNode();

protected:
    explicit QAbstractClipBlendNode(Qt3DCore::QNode *parent = nullptr);
    explicit QAbstractClipBlendNode(QAbstractClipBlendNodePrivate &dd, Qt3DCore::QNode *parent = nullptr);

private:
    Q_DECLARE_PRIVATE(QAbstractClipBlendNode)
};


} // Qt3DAnimation

QT_END_NAMESPACE


#endif // QT3DANIMATION_QABSTRACTCLIPBLENDNODE_H
