/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Copyright (C) 2015 The Qt Company Ltd and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QT3DRENDER_QBLENDSTATE_H
#define QT3DRENDER_QBLENDSTATE_H

#include <Qt3DRender/qrenderstate.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QBlendStatePrivate;

class QT3DRENDERSHARED_EXPORT QBlendState : public QRenderState
{
    Q_OBJECT
    Q_PROPERTY(Blending srcRGB READ srcRGB WRITE setSrcRGB NOTIFY srcRGBChanged)
    Q_PROPERTY(Blending srcAlpha READ srcAlpha WRITE setSrcAlpha NOTIFY srcAlphaChanged)
    Q_PROPERTY(Blending dstRGB READ dstRGB WRITE setDstRGB NOTIFY dstRGBChanged)
    Q_PROPERTY(Blending dstAlpha READ dstAlpha WRITE setDstAlpha NOTIFY dstAlphaChanged)

public:

    enum Blending
    {
        Zero = 0,
        One = 1,
        SrcColor = 0x0300,
        SrcAlpha = 0x0302,
        Src1Alpha,
        Src1Color,
        DstColor = 0x0306,
        DstAlpha = 0x0304,
        SrcAlphaSaturate = 0x0308,
        ConstantColor = 0x8001,
        ConstantAlpha = 0x8003,
        OneMinusSrcColor = 0x0301,
        OneMinusSrcAlpha = 0x0303,
        OneMinusDstAlpha = 0x0305,
        OneMinusDstColor = 0x0307,
        OneMinusConstantColor = 0x8002,
        OneMinusConstantAlpha = 0x8004,
        OneMinusSrc1Alpha,
        OneMinusSrc1Color0
    };
    Q_ENUM(Blending)

    explicit QBlendState(Qt3DCore::QNode *parent = Q_NULLPTR);
    ~QBlendState();

    Blending srcRGB() const;
    Blending dstRGB() const;
    Blending srcAlpha() const;
    Blending dstAlpha() const;

public Q_SLOTS:
    void setSrcRGB(Blending srcRGB);
    void setDstRGB(Blending dstRGB);
    void setSrcAlpha(Blending srcAlpha);
    void setDstAlpha(Blending dstAlpha);

Q_SIGNALS:
    void srcRGBChanged(Blending srcRGB);
    void srcAlphaChanged(Blending srcAlpha);
    void dstRGBChanged(Blending dstRGB);
    void dstAlphaChanged(Blending dstAlpha);

protected:
    void copy(const Qt3DCore::QNode *ref) Q_DECL_OVERRIDE;
    QBlendState(QRenderState::Type type, Qt3DCore::QNode *parent = Q_NULLPTR);

private:
    Q_DECLARE_PRIVATE(QBlendState)
    QT3D_CLONEABLE(QBlendState)
};

class QT3DRENDERSHARED_EXPORT QBlendStateSeparate : public QBlendState
{
    Q_OBJECT
public:
    explicit QBlendStateSeparate(Qt3DCore::QNode *parent = Q_NULLPTR);
};

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QBLENDSTATE_H
