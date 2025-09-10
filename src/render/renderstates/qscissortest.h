// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// Copyright (C) 2016 The Qt Company Ltd and/or its subsidiary(-ies).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

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
