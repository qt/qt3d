// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DANIMATION_QLERPCLIPBLEND_H
#define QT3DANIMATION_QLERPCLIPBLEND_H

#include <Qt3DAnimation/qabstractclipblendnode.h>

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {

class QLerpClipBlendPrivate;

class Q_3DANIMATIONSHARED_EXPORT QLerpClipBlend : public QAbstractClipBlendNode
{
    Q_OBJECT
    Q_PROPERTY(Qt3DAnimation::QAbstractClipBlendNode *startClip READ startClip WRITE setStartClip NOTIFY startClipChanged)
    Q_PROPERTY(Qt3DAnimation::QAbstractClipBlendNode *endClip READ endClip WRITE setEndClip NOTIFY endClipChanged)
    Q_PROPERTY(float blendFactor READ blendFactor WRITE setBlendFactor NOTIFY blendFactorChanged)

public:
    explicit QLerpClipBlend(Qt3DCore::QNode *parent = nullptr);
    ~QLerpClipBlend();

    float blendFactor() const;
    Qt3DAnimation::QAbstractClipBlendNode *startClip() const;
    Qt3DAnimation::QAbstractClipBlendNode *endClip() const;

public Q_SLOTS:
    void setBlendFactor(float blendFactor);
    void setStartClip(Qt3DAnimation::QAbstractClipBlendNode * startClip);
    void setEndClip(Qt3DAnimation::QAbstractClipBlendNode * endClip);

Q_SIGNALS:
    void blendFactorChanged(float blendFactor);
    void startClipChanged(Qt3DAnimation::QAbstractClipBlendNode * startClip);
    void endClipChanged(Qt3DAnimation::QAbstractClipBlendNode * endClip);

protected:
    explicit QLerpClipBlend(QLerpClipBlendPrivate &dd, Qt3DCore::QNode *parent = nullptr);

private:
    Q_DECLARE_PRIVATE(QLerpClipBlend)
};

} // Qt3DAnimation

QT_END_NAMESPACE


#endif // QT3DANIMATION_QLERPCLIPBLEND_H
