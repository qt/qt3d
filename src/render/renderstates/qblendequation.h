// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// Copyright (C) 2016 The Qt Company Ltd and/or its subsidiary(-ies).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_QBLENDEQUATION_H
#define QT3DRENDER_QBLENDEQUATION_H

#include <Qt3DRender/qrenderstate.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QBlendEquationPrivate;

class Q_3DRENDERSHARED_EXPORT QBlendEquation : public QRenderState
{
    Q_OBJECT
    Q_PROPERTY(BlendFunction blendFunction READ blendFunction WRITE setBlendFunction NOTIFY blendFunctionChanged)
public:

    enum BlendFunction
    {
        Add = 0x8006,
        Subtract = 0x800A,
        ReverseSubtract = 0x800B,
        Min = 0x8007,
        Max = 0x8008
    };
    Q_ENUM(BlendFunction) // LCOV_EXCL_LINE

    explicit QBlendEquation(Qt3DCore::QNode *parent = nullptr);
    ~QBlendEquation();

    BlendFunction blendFunction() const;

public Q_SLOTS:
    void setBlendFunction(BlendFunction blendFunction);

Q_SIGNALS:
    void blendFunctionChanged(BlendFunction blendFunction);

private:
    Q_DECLARE_PRIVATE(QBlendEquation)
};

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QBLENDEQUATION_H
