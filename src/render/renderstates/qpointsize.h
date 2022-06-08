// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
#ifndef QT3DRENDER_QPOINTSIZE_H
#define QT3DRENDER_QPOINTSIZE_H

#include <Qt3DRender/qrenderstate.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QPointSizePrivate;

class Q_3DRENDERSHARED_EXPORT QPointSize : public QRenderState
{
    Q_OBJECT
    Q_PROPERTY(SizeMode sizeMode READ sizeMode WRITE setSizeMode NOTIFY sizeModeChanged)
    Q_PROPERTY(float value READ value WRITE setValue NOTIFY valueChanged)

public:
    enum SizeMode {
        Fixed = 0,
        Programmable = 1
    };
    Q_ENUM(SizeMode) // LCOV_EXCL_LINE

    explicit QPointSize(Qt3DCore::QNode *parent = nullptr);
    ~QPointSize();

    SizeMode sizeMode() const;
    float value() const;

public Q_SLOTS:
    void setSizeMode(SizeMode sizeMode);
    void setValue(float value);

Q_SIGNALS:
    void sizeModeChanged(SizeMode sizeMode);
    void valueChanged(float value);

private:
    Q_DECLARE_PRIVATE(QPointSize)
};

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QPOINTSIZE_H
