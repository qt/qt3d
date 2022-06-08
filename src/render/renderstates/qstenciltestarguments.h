// Copyright (C) 2015 Paul Lemire
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_QSTENCILTESTARGUMENTS_H
#define QT3DRENDER_QSTENCILTESTARGUMENTS_H

#include <QtCore/QObject>
#include <Qt3DRender/qt3drender_global.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QStencilTestPrivate;
class QStencilTestArgumentsPrivate;

class Q_3DRENDERSHARED_EXPORT QStencilTestArguments : public QObject
{
    Q_OBJECT
    Q_PROPERTY(StencilFaceMode faceMode READ faceMode NOTIFY faceModeChanged)
    Q_PROPERTY(uint comparisonMask READ comparisonMask WRITE setComparisonMask NOTIFY comparisonMaskChanged)
    Q_PROPERTY(int referenceValue READ referenceValue WRITE setReferenceValue NOTIFY referenceValueChanged)
    Q_PROPERTY(StencilFunction stencilFunction READ stencilFunction WRITE setStencilFunction NOTIFY stencilFunctionChanged)

public:
    enum StencilFaceMode
    {
        Front = 0x0404,
        Back = 0x0405,
        FrontAndBack = 0x0408
    };
    Q_ENUM(StencilFaceMode) // LCOV_EXCL_LINE

    enum StencilFunction
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
    Q_ENUM(StencilFunction) // LCOV_EXCL_LINE

    ~QStencilTestArguments();

    uint comparisonMask() const;
    int referenceValue() const;
    StencilFunction stencilFunction() const;

    StencilFaceMode faceMode() const;

public Q_SLOTS:
    void setComparisonMask(uint comparisonMask);
    void setReferenceValue(int referenceValue);
    void setStencilFunction(StencilFunction stencilFunction);

Q_SIGNALS:
    void comparisonMaskChanged(uint comparisonMask);
    void stencilFunctionChanged(StencilFunction stencilFunction);
    void referenceValueChanged(int referenceValue);
    void faceModeChanged(StencilFaceMode faceMode);

private:
    explicit QStencilTestArguments(StencilFaceMode face, QObject *parent = nullptr);

    friend class QStencilTestPrivate;

    Q_DECLARE_PRIVATE(QStencilTestArguments)
};

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QSTENCILTESTARGUMENTS_H
