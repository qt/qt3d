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
****************************************************************************/

#ifndef QT3DRENDER_QTEXTUREWRAPMODE_H
#define QT3DRENDER_QTEXTUREWRAPMODE_H

#include <Qt3DRender/qt3drender_global.h>
#include <QtCore/QObject>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QTextureWrapModePrivate;

class Q_3DRENDERSHARED_EXPORT QTextureWrapMode : public QObject
{
    Q_OBJECT
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
    Q_ENUM(WrapMode) // LCOV_EXCL_LINE

    explicit QTextureWrapMode(WrapMode wrapMode = ClampToEdge, QObject *parent = nullptr);
    explicit QTextureWrapMode(WrapMode x, WrapMode y, WrapMode z, QObject *parent = nullptr);
    ~QTextureWrapMode();

    WrapMode x() const;
    WrapMode y() const;
    WrapMode z() const;

public Q_SLOTS:
    void setX(WrapMode x);
    void setY(WrapMode y);
    void setZ(WrapMode z);

Q_SIGNALS:
    void xChanged(WrapMode x);
    void yChanged(WrapMode y);
    void zChanged(WrapMode z);

private:
    Q_DECLARE_PRIVATE(QTextureWrapMode)
};

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QWRAPMODE_H
