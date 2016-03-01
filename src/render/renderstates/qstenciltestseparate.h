/****************************************************************************
**
** Copyright (C) 2015 Paul Lemire
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

#ifndef QT3DRENDER_QSTENCILTESTSEPARATE_H
#define QT3DRENDER_QSTENCILTESTSEPARATE_H

#include <QObject>
#include <Qt3DRender/qt3drender_global.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QStencilTestPrivate;
class QStencilTestSeparatePrivate;

class QT3DRENDERSHARED_EXPORT QStencilTestSeparate : public QObject
{
    Q_OBJECT
    Q_PROPERTY(StencilFaceMode faceMode READ faceMode NOTIFY faceModeChanged)
    Q_PROPERTY(uint mask READ mask WRITE setMask NOTIFY maskChanged)
    Q_PROPERTY(int ref READ ref WRITE setRef NOTIFY refChanged)
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

    ~QStencilTestSeparate();

    uint mask() const;
    int ref() const;
    StencilFunc func() const;

    StencilFaceMode faceMode() const;

public Q_SLOTS:
    void setMask(uint mask);
    void setRef(int ref);
    void setFunc(StencilFunc func);

Q_SIGNALS:
    void maskChanged(uint mask);
    void funcChanged(StencilFunc func);
    void refChanged(int ref);
    void faceModeChanged(StencilFaceMode faceMode);

private:
    explicit QStencilTestSeparate(StencilFaceMode face, QObject *parent = Q_NULLPTR);

    friend class QStencilTestPrivate;

    Q_DECLARE_PRIVATE(QStencilTestSeparate)
};

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QSTENCILTESTSEPARATE_H
