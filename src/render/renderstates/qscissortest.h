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

#ifndef QT3DRENDER_QSCISSORTEST_H
#define QT3DRENDER_QSCISSORTEST_H

#include <Qt3DRender/qrenderstate.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QScissorTestPrivate;

class Q_3DRENDERSHARED_EXPORT QScissorTest  : public QRenderState
{
    Q_OBJECT
    Q_PROPERTY(int left READ left WRITE setLeft NOTIFY leftChanged)
    Q_PROPERTY(int bottom READ bottom WRITE setBottom NOTIFY bottomChanged)
    Q_PROPERTY(int width READ width WRITE setWidth NOTIFY widthChanged)
    Q_PROPERTY(int height READ height WRITE setHeight NOTIFY heightChanged)

public:
    explicit QScissorTest(Qt3DCore::QNode *parent = nullptr);
    ~QScissorTest();

    int left() const;
    int bottom() const;
    int width() const;
    int height() const;

public Q_SLOTS:
    void setLeft(int left);
    void setBottom(int bottom);
    void setWidth(int width);
    void setHeight(int height);

Q_SIGNALS:
    void leftChanged(int left);
    void bottomChanged(int bottom);
    void widthChanged(int width);
    void heightChanged(int height);

private:
    Q_DECLARE_PRIVATE(QScissorTest)
};

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QSCISSORTEST_H
