/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
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
