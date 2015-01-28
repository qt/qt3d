/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
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

#ifndef QT3D_QWRAPMODE_H
#define QT3D_QWRAPMODE_H

#include <Qt3DRenderer/qt3drenderer_global.h>
#include <QObject>

QT_BEGIN_NAMESPACE

namespace Qt3D {

class QTextureWrapModePrivate;

class QT3DRENDERERSHARED_EXPORT QTextureWrapMode: public QObject
{
    Q_OBJECT
    Q_ENUMS(WrapMode)
    Q_PROPERTY(WrapMode x READ x WRITE setX NOTIFY xChanged)
    Q_PROPERTY(WrapMode y READ y WRITE setY NOTIFY yChanged)
    Q_PROPERTY(WrapMode z READ z WRITE setZ NOTIFY zChanged)

public:
    enum WrapMode {
        Repeat         = 0x2901, // GL_REPEAT
        MirroredRepeat = 0x8370, // GL_MIRRORED_REPEAT
        ClampToEdge    = 0x812F, // GL_CLAMP_TO_EDGE
        ClampToBorder  = 0x812D  // GL_CLAMP_TO_BORDER
    };

    explicit QTextureWrapMode(WrapMode wrapMode = ClampToEdge, QObject *parent = 0);
    explicit QTextureWrapMode(WrapMode x, WrapMode y, WrapMode z, QObject *parent = 0);

    void setX(WrapMode x);
    WrapMode x() const;

    void setY(WrapMode y);
    WrapMode y() const;

    void setZ(WrapMode z);
    WrapMode z() const;

Q_SIGNALS:
    void xChanged();
    void yChanged();
    void zChanged();

private:
    Q_DECLARE_PRIVATE(QTextureWrapMode)
};

} // Qt3D

QT_END_NAMESPACE

Q_DECLARE_METATYPE(Qt3D::QTextureWrapMode*)

#endif // QT3D_QWRAPMODE_H
