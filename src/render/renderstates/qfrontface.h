// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// Copyright (C) 2016 The Qt Company Ltd and/or its subsidiary(-ies).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_QFRONTFACE_H
#define QT3DRENDER_QFRONTFACE_H

#include <Qt3DRender/qrenderstate.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QFrontFacePrivate;

class Q_3DRENDERSHARED_EXPORT QFrontFace : public QRenderState
{
    Q_OBJECT
    Q_PROPERTY(WindingDirection direction READ direction WRITE setDirection NOTIFY directionChanged)
public:

    enum WindingDirection
    {
        ClockWise = 0x0900,
        CounterClockWise = 0x0901
    };
    Q_ENUM(WindingDirection) // LCOV_EXCL_LINE

    explicit QFrontFace(Qt3DCore::QNode *parent = nullptr);
    ~QFrontFace();

    WindingDirection direction() const;

public Q_SLOTS:
    void setDirection(WindingDirection direction);

Q_SIGNALS:
    void directionChanged(WindingDirection direction);

private:
    Q_DECLARE_PRIVATE(QFrontFace)
};

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QFRONTFACE_H
