/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Copyright (C) 2021 The Qt Company Ltd and/or its subsidiary(-ies).
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
****************************************************************************/

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
    Qt3DCore::QNodeCreatedChangeBasePtr createNodeCreationChange() const override;
};

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QFRONTFACE_H
