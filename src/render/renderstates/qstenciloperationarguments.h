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

#ifndef QT3DRENDER_QSTENCILOPERATIONARGUMENTS_H
#define QT3DRENDER_QSTENCILOPERATIONARGUMENTS_H

#include <QtCore/QObject>
#include <Qt3DRender/qt3drender_global.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QStencilOperationPrivate;
class QStencilOperationArgumentsPrivate;

class Q_3DRENDERSHARED_EXPORT QStencilOperationArguments : public QObject
{
    Q_OBJECT
    Q_PROPERTY(FaceMode faceMode READ faceMode NOTIFY faceModeChanged)
    Q_PROPERTY(Operation stencilTestFailureOperation READ stencilTestFailureOperation WRITE setStencilTestFailureOperation NOTIFY stencilTestFailureOperationChanged)
    Q_PROPERTY(Operation depthTestFailureOperation READ depthTestFailureOperation WRITE setDepthTestFailureOperation NOTIFY depthTestFailureOperationChanged)
    Q_PROPERTY(Operation allTestsPassOperation READ allTestsPassOperation WRITE setAllTestsPassOperation NOTIFY allTestsPassOperationChanged)

public:
    enum FaceMode
    {
        Front = 0x0404,
        Back = 0x0405,
        FrontAndBack = 0x0408
    };
    Q_ENUM(FaceMode) // LCOV_EXCL_LINE

    enum Operation
    {
        Zero = 0,
        Keep = 0x1E00,
        Replace = 0x1E01,
        Increment = 0x1E02,
        Decrement = 0x1E03,
        IncrementWrap = 0x8507,
        DecrementWrap = 0x8508,
        Invert = 0x150A
    };
    Q_ENUM(Operation) // LCOV_EXCL_LINE

    ~QStencilOperationArguments();

    FaceMode faceMode() const;

    Operation stencilTestFailureOperation() const;
    Operation depthTestFailureOperation() const;
    Operation allTestsPassOperation() const;

public Q_SLOTS:
    void setStencilTestFailureOperation(Operation operation);
    void setDepthTestFailureOperation(Operation operation);
    void setAllTestsPassOperation(Operation operation);

Q_SIGNALS:
    void stencilTestFailureOperationChanged(Operation stencilFail);
    void depthTestFailureOperationChanged(Operation depthFail);
    void allTestsPassOperationChanged(Operation stencilDepthPass);
    void faceModeChanged(FaceMode faceMode);

private:
    explicit QStencilOperationArguments(FaceMode mode, QObject *parent = nullptr);
    Q_DECLARE_PRIVATE(QStencilOperationArguments)

    friend class QStencilOperationPrivate;
};

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QSTENCILOPERATIONARGUMENTS_H
