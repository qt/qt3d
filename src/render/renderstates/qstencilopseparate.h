/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
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

#ifndef QT3DRENDER_QSTENCILOPSEPARATE_H
#define QT3DRENDER_QSTENCILOPSEPARATE_H

#include <QObject>
#include <Qt3DRender/qt3drender_global.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QStencilOpPrivate;
class QStencilOpSeparatePrivate;

class QT3DRENDERSHARED_EXPORT QStencilOpSeparate : public QObject
{
    Q_OBJECT
    Q_PROPERTY(StencilFaceMode faceMode READ faceMode NOTIFY faceModeChanged)
    Q_PROPERTY(StencilOp stencilFail READ stencilFail WRITE setStencilFail NOTIFY stencilFailChanged)
    Q_PROPERTY(StencilOp depthFail READ depthFail WRITE setDepthFail NOTIFY depthFailChanged)
    Q_PROPERTY(StencilOp stencilDepthPass READ stencilDepthPass WRITE setStencilDepthPass NOTIFY stencilDepthPassChanged)

public:
    enum StencilFaceMode
    {
        Front = 0x0404,
        Back = 0x0405,
        FrontAndBack = 0x0408
    };
    Q_ENUM(StencilFaceMode)

    enum StencilOp
    {
        Zero = 0,
        Keep = 0x1E00,
        Replace = 0x1E01,
        Incr = 0x1E02,
        Decr = 0x1E03,
        IncrWrap = 0x8507,
        DecrWrap = 0x8508,
        Invert = 0x150A
    };
    Q_ENUM(StencilOp)

    ~QStencilOpSeparate();

    StencilFaceMode faceMode() const;

    StencilOp stencilFail() const;
    StencilOp depthFail() const;
    StencilOp stencilDepthPass() const;

public Q_SLOTS:
    void setStencilFail(StencilOp op);
    void setDepthFail(StencilOp op);
    void setStencilDepthPass(StencilOp op);

Q_SIGNALS:
    void stencilFailChanged(StencilOp stencilFail);
    void depthFailChanged(StencilOp depthFail);
    void stencilDepthPassChanged(StencilOp stencilDepthPass);
    void faceModeChanged(StencilFaceMode faceMode);

private:
    explicit QStencilOpSeparate(StencilFaceMode mode, QObject *parent = Q_NULLPTR);
    Q_DECLARE_PRIVATE(QStencilOpSeparate)

    friend class QStencilOpPrivate;
};

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QSTENCILOPSEPARATE_H
