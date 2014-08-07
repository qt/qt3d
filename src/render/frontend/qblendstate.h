/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QT3D_QBLENDSTATE_H
#define QT3D_QBLENDSTATE_H

#include <Qt3DRenderer/qdrawstate.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

class QBlendStatePrivate;

class QT3DRENDERERSHARED_EXPORT QBlendState : public QDrawState
{
    Q_OBJECT
    Q_ENUMS(Blending)
    Q_PROPERTY(Blending srcRGB READ srcRGB WRITE setSrcRGB NOTIFY srcRGBChanged)
    Q_PROPERTY(Blending srcAlpha READ srcAlpha WRITE setSrcAlpha NOTIFY srcAlphaChanged)
    Q_PROPERTY(Blending dstRGB READ dstRGB WRITE setDstRGB NOTIFY dstRGBChanged)
    Q_PROPERTY(Blending dstAlpha READ dstAlpha WRITE setDstAlpha NOTIFY dstAlphaChanged)

public:

    enum Blending
    {
        Zero = 0,
        One,
        SrcColor,
        SrcAlpha,
        Src1Alpha,
        Src1Color,
        DstColor,
        DstAlpha,
        SrcAlphaSaturate,
        ConstantColor,
        ConstantAlpha,
        OneMinusSrcColor,
        OneMinusSrcAlpha,
        OneMinusDstAlpha,
        OneMinusDstColor,
        OneMinusConstantColor,
        OneMinusConstantAlpha,
        OneMinusSrc1Alpha,
        OneMinusSrc1Color0
    };

    explicit QBlendState(QNode *parent = 0);

    void copy(const QNode *ref) Q_DECL_OVERRIDE;

    Blending srcRGB() const;
    void setSrcRGB(Blending srcRGB);

    Blending dstRGB() const;
    void setDstRGB(Blending dstRGB);

    Blending srcAlpha() const;
    void setSrcAlpha(Blending srcAlpha);

    Blending dstAlpha() const;
    void setDstAlpha(Blending dstAlpha);

Q_SIGNALS:
    void srcRGBChanged();
    void srcAlphaChanged();
    void dstRGBChanged();
    void dstAlphaChanged();

private:
    Q_DECLARE_PRIVATE(QBlendState)
    QNode *doClone(QNode *clonedParent) const Q_DECL_OVERRIDE;
};

} // Qt3D

QT_END_NAMESPACE

#endif // QT3D_QBLENDSTATE_H
