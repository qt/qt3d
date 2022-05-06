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
******************************************************************************/

#ifndef QT3DRENDER_QCOLORMASK_H
#define QT3DRENDER_QCOLORMASK_H

#include <Qt3DRender/qrenderstate.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QColorMaskPrivate;

class Q_3DRENDERSHARED_EXPORT QColorMask : public QRenderState
{
    Q_OBJECT
    Q_PROPERTY(bool redMasked READ isRedMasked WRITE setRedMasked NOTIFY redMaskedChanged)
    Q_PROPERTY(bool greenMasked READ isGreenMasked WRITE setGreenMasked NOTIFY greenMaskedChanged)
    Q_PROPERTY(bool blueMasked READ isBlueMasked WRITE setBlueMasked NOTIFY blueMaskedChanged)
    Q_PROPERTY(bool alphaMasked READ isAlphaMasked WRITE setAlphaMasked NOTIFY alphaMaskedChanged)

public:
    explicit QColorMask(Qt3DCore::QNode *parent = nullptr);
    ~QColorMask();

    bool isRedMasked() const;
    bool isGreenMasked() const;
    bool isBlueMasked() const;
    bool isAlphaMasked() const;

public Q_SLOTS:
    void setRedMasked(bool redMasked);
    void setGreenMasked(bool greenMasked);
    void setBlueMasked(bool blueMasked);
    void setAlphaMasked(bool alphaMasked);

Q_SIGNALS:
    void redMaskedChanged(bool redMasked);
    void greenMaskedChanged(bool greenMasked);
    void blueMaskedChanged(bool blueMasked);
    void alphaMaskedChanged(bool alphaMasked);

private:
    Q_DECLARE_PRIVATE(QColorMask)
};

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QCOLORMASK_H
