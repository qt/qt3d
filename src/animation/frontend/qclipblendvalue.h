/****************************************************************************
**
** Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
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

#ifndef QT3DANIMATION_QCLIPBLENDVALUE_H
#define QT3DANIMATION_QCLIPBLENDVALUE_H

#include <Qt3DAnimation/qt3danimation_global.h>
#include <Qt3DAnimation/qabstractclipblendnode.h>

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {

class QClipBlendValuePrivate;

class Q_3DANIMATIONSHARED_EXPORT QClipBlendValue : public Qt3DAnimation::QAbstractClipBlendNode
{
    Q_OBJECT
    Q_PROPERTY(Qt3DAnimation::QAbstractAnimationClip *clip READ clip WRITE setClip NOTIFY clipChanged)

public:
    explicit QClipBlendValue(Qt3DCore::QNode *parent = nullptr);
    explicit QClipBlendValue(Qt3DAnimation::QAbstractAnimationClip *clip,
                             Qt3DCore::QNode *parent = nullptr);
    ~QClipBlendValue();

    Qt3DAnimation::QAbstractAnimationClip *clip() const;

public Q_SLOTS:
    void setClip(Qt3DAnimation::QAbstractAnimationClip *clip);

Q_SIGNALS:
    void clipChanged(Qt3DAnimation::QAbstractAnimationClip *clip);

protected:
    explicit QClipBlendValue(QClipBlendValuePrivate &dd, Qt3DCore::QNode *parent = nullptr);

private:
    Q_DECLARE_PRIVATE(QClipBlendValue)
};

} // namespace Qt3DAnimation

QT_END_NAMESPACE

#endif // QT3DANIMATION_QCLIPBLENDVALUE_H
