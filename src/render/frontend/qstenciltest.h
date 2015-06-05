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

#ifndef QT3D_QSTENCILTEST_H
#define QT3D_QSTENCILTEST_H

#include <Qt3DRenderer/qrenderstate.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

class QStencilTestPrivate;

class QT3DRENDERERSHARED_EXPORT QStencilTest : public QRenderState
{
    Q_OBJECT
    Q_PROPERTY(uint mask READ mask WRITE setMask NOTIFY maskChanged)
    Q_PROPERTY(StencilFaceMode faceMode READ faceMode WRITE setFaceMode NOTIFY faceModeChanged)
    Q_PROPERTY(StencilFunc func READ func WRITE setFunc NOTIFY funcChanged)
public:

    enum StencilFaceMode
    {
        Front = 0x0404,
        Back = 0x0405,
        FrontAndBack = 0x0408
    };
    Q_ENUM(StencilFaceMode)

    enum StencilFunc
    {
        Never = 0x0200,
        Always = 0x0207,
        Less = 0x0201,
        LessOrEqual = 0x0203,
        Equal = 0x0202,
        GreaterOrEqual = 0x0206,
        Greater = 0x0204,
        NotEqual = 0x0205
    };
    Q_ENUM(StencilFunc)

    explicit QStencilTest(QNode *parent = 0);

    uint mask() const;
    void setMask(uint mask);

    StencilFaceMode faceMode() const;
    void setFaceMode(StencilFaceMode mode);

    StencilFunc func() const;
    void setFunc(StencilFunc func);

Q_SIGNALS:
    void maskChanged();
    void faceModeChanged();
    void funcChanged();

protected:
    void copy(const QNode *ref) Q_DECL_OVERRIDE;

private:
    Q_DECLARE_PRIVATE(QStencilTest)
    QT3D_CLONEABLE(QStencilTest)
};

} // Qt3D

QT_END_NAMESPACE


#endif // QT3D_QSTENCILTEST_H
