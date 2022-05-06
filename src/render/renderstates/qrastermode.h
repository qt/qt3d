/****************************************************************************
**
** Copyright (C) 2018 Klaralvdalens Datakonsult AB (KDAB).
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

#ifndef QT3DRENDER_QRASTERMODE_H
#define QT3DRENDER_QRASTERMODE_H

#include <Qt3DRender/qrenderstate.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QRasterModePrivate;

class Q_3DRENDERSHARED_EXPORT QRasterMode : public QRenderState
{
    Q_OBJECT
    Q_PROPERTY(RasterMode rasterMode READ rasterMode WRITE setRasterMode NOTIFY rasterModeChanged)
    Q_PROPERTY(FaceMode faceMode READ faceMode WRITE setFaceMode NOTIFY faceModeChanged)
public:

    enum RasterMode {
        Points = 0x1B00,
        Lines = 0x1B01,
        Fill = 0x1B02,
    };
    Q_ENUM(RasterMode) // LCOV_EXCL_LINE

    enum FaceMode
    {
        Front = 0x0404,
        Back = 0x0405,
        FrontAndBack = 0x0408
    };
    Q_ENUM(FaceMode) // LCOV_EXCL_LINE

    explicit QRasterMode(Qt3DCore::QNode *parent = nullptr);
    ~QRasterMode();

    RasterMode rasterMode() const;
    FaceMode faceMode() const;

public Q_SLOTS:
    void setRasterMode(RasterMode rasterMode);
    void setFaceMode(FaceMode faceMode);

Q_SIGNALS:
    void rasterModeChanged(RasterMode rasterMode);
    void faceModeChanged(FaceMode faceMode);

private:
    Q_DECLARE_PRIVATE(QRasterMode)
};

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QRASTERMODE_H
