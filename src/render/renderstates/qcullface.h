/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Copyright (C) 2022 The Qt Company Ltd and/or its subsidiary(-ies).
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


#ifndef QT3DRENDER_QCULLFACE_H
#define QT3DRENDER_QCULLFACE_H

#include <Qt3DRender/qrenderstate.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QCullFacePrivate;

class Q_3DRENDERSHARED_EXPORT QCullFace : public QRenderState
{
    Q_OBJECT
    Q_PROPERTY(CullingMode mode READ mode WRITE setMode NOTIFY modeChanged)

public:

    enum CullingMode
    {
        NoCulling = 0x0,
        Front = 0x0404,
        Back = 0x0405,
        FrontAndBack = 0x0408
    };
    Q_ENUM(CullingMode) // LCOV_EXCL_LINE

    explicit QCullFace(Qt3DCore::QNode *parent = nullptr);
    ~QCullFace();

    CullingMode mode() const;

public Q_SLOTS:
    void setMode(CullingMode mode);

Q_SIGNALS:
    void modeChanged(CullingMode mode);

private:
    Q_DECLARE_PRIVATE(QCullFace)
};

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QCULLFACE_H
