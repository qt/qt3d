// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DANIMATION_QADDITIVECLIPBLEND_H
#define QT3DANIMATION_QADDITIVECLIPBLEND_H

#include <Qt3DAnimation/qabstractclipblendnode.h>

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {

class QAdditiveClipBlendPrivate;

class Q_3DANIMATIONSHARED_EXPORT QAdditiveClipBlend : public QAbstractClipBlendNode
{
    Q_OBJECT
    Q_PROPERTY(Qt3DAnimation::QAbstractClipBlendNode *baseClip READ baseClip WRITE setBaseClip NOTIFY baseClipChanged)
    Q_PROPERTY(Qt3DAnimation::QAbstractClipBlendNode *additiveClip READ additiveClip WRITE setAdditiveClip NOTIFY additiveClipChanged)
    Q_PROPERTY(float additiveFactor READ additiveFactor WRITE setAdditiveFactor NOTIFY additiveFactorChanged)
public:
    explicit QAdditiveClipBlend(Qt3DCore::QNode *parent = nullptr);
    ~QAdditiveClipBlend();

    float additiveFactor() const;
    Qt3DAnimation::QAbstractClipBlendNode *baseClip() const;
    Qt3DAnimation::QAbstractClipBlendNode *additiveClip() const;

public Q_SLOTS:
    void setAdditiveFactor(float additiveFactor);
    void setBaseClip(Qt3DAnimation::QAbstractClipBlendNode *baseClip);
    void setAdditiveClip(Qt3DAnimation::QAbstractClipBlendNode *additiveClip);

Q_SIGNALS:
    void additiveFactorChanged(float additiveFactor);
    void baseClipChanged(Qt3DAnimation::QAbstractClipBlendNode *baseClip);
    void additiveClipChanged(Qt3DAnimation::QAbstractClipBlendNode *additiveClip);

protected:
    explicit QAdditiveClipBlend(QAdditiveClipBlendPrivate &dd, Qt3DCore::QNode *parent = nullptr);

private:
    Q_DECLARE_PRIVATE(QAdditiveClipBlend)
};

} // Qt3DAnimation

QT_END_NAMESPACE

#endif // QT3DANIMATION_QADDITIVECLIPBLEND_H
