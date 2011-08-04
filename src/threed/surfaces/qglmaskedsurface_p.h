/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtQuick3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QGLMASKEDSURFACE_H
#define QGLMASKEDSURFACE_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "qglabstractsurface.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QGLMaskedSurfacePrivate;

class QGLMaskedSurface : public QGLAbstractSurface
{
public:
    enum BufferMaskBit
    {
        RedMask     = 0x0001,
        GreenMask   = 0x0002,
        BlueMask    = 0x0004,
        AlphaMask   = 0x0008
    };
    Q_DECLARE_FLAGS(BufferMask, BufferMaskBit)

    QGLMaskedSurface();
    QGLMaskedSurface
        (QGLAbstractSurface *surface, QGLMaskedSurface::BufferMask mask);
    ~QGLMaskedSurface();

    QGLAbstractSurface *surface() const;
    void setSurface(QGLAbstractSurface *surface);

    QGLMaskedSurface::BufferMask mask() const;
    void setMask(QGLMaskedSurface::BufferMask mask);

    QPaintDevice *device() const;
    bool activate(QGLAbstractSurface *prevSurface = 0);
    void deactivate(QGLAbstractSurface *nextSurface = 0);
    QRect viewportGL() const;

private:
    QScopedPointer<QGLMaskedSurfacePrivate> d_ptr;

    Q_DECLARE_PRIVATE(QGLMaskedSurface)
    Q_DISABLE_COPY(QGLMaskedSurface)
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QGLMaskedSurface::BufferMask)

QT_END_NAMESPACE

QT_END_HEADER

#endif
