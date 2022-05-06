/****************************************************************************
**
** Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
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
**
**
**
******************************************************************************/

#ifndef QT3DANIMATION_QBLENDEDCLIPANIMATOR_H
#define QT3DANIMATION_QBLENDEDCLIPANIMATOR_H

#include <Qt3DAnimation/qt3danimation_global.h>
#include <Qt3DAnimation/qabstractclipanimator.h>
#include <Qt3DAnimation/qabstractclipblendnode.h>

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {

class QBlendedClipAnimatorPrivate;

class Q_3DANIMATIONSHARED_EXPORT QBlendedClipAnimator : public Qt3DAnimation::QAbstractClipAnimator
{
    Q_OBJECT
    Q_PROPERTY(Qt3DAnimation::QAbstractClipBlendNode *blendTree READ blendTree WRITE setBlendTree NOTIFY blendTreeChanged)

public:
    explicit QBlendedClipAnimator(Qt3DCore::QNode *parent = nullptr);
    ~QBlendedClipAnimator();

    QAbstractClipBlendNode *blendTree() const;

public Q_SLOTS:
    void setBlendTree(QAbstractClipBlendNode * blendTree);

Q_SIGNALS:
    void blendTreeChanged(QAbstractClipBlendNode * blendTree);

protected:
    explicit QBlendedClipAnimator(QBlendedClipAnimatorPrivate &dd, Qt3DCore::QNode *parent = nullptr);

private:
    Q_DECLARE_PRIVATE(QBlendedClipAnimator)
};

} // namespace Qt3DAnimation

QT_END_NAMESPACE

#endif // QT3DANIMATION_QBLENDEDCLIPANIMATOR_H
