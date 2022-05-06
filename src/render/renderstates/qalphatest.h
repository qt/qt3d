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

#ifndef QT3DRENDER_QALPHATEST_H
#define QT3DRENDER_QALPHATEST_H

#include <Qt3DRender/qrenderstate.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QAlphaTestPrivate;

class Q_3DRENDERSHARED_EXPORT QAlphaTest : public QRenderState
{
    Q_OBJECT
    Q_PROPERTY(AlphaFunction alphaFunction READ alphaFunction WRITE setAlphaFunction NOTIFY alphaFunctionChanged)
    Q_PROPERTY(float referenceValue READ referenceValue WRITE setReferenceValue NOTIFY referenceValueChanged)
public:

    enum AlphaFunction {
        Never = 0x0200,
        Always = 0x0207,
        Less = 0x0201,
        LessOrEqual = 0x0203,
        Equal = 0x0202,
        GreaterOrEqual = 0x0206,
        Greater = 0x0204,
        NotEqual = 0x0205
    };
    Q_ENUM(AlphaFunction) // LCOV_EXCL_LINE

    explicit QAlphaTest(Qt3DCore::QNode *parent = nullptr);
    ~QAlphaTest();

    AlphaFunction alphaFunction() const;
    float referenceValue() const;

public Q_SLOTS:
    void setAlphaFunction(AlphaFunction alphaFunction);
    void setReferenceValue(float referenceValue);

Q_SIGNALS:
    void alphaFunctionChanged(AlphaFunction alphaFunction);
    void referenceValueChanged(float referenceValue);

private:
    Q_DECLARE_PRIVATE(QAlphaTest)
};

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QALPHATEST_H
