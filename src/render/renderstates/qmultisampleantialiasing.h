// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial

#ifndef QT3DRENDER_QMULTISAMPLEANTIALIASING_H
#define QT3DRENDER_QMULTISAMPLEANTIALIASING_H

#include <Qt3DRender/qrenderstate.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QMultiSampleAntiAliasingPrivate;

class Q_3DRENDERSHARED_EXPORT QMultiSampleAntiAliasing : public QRenderState
{
    Q_OBJECT

public:
    explicit QMultiSampleAntiAliasing(Qt3DCore::QNode *parent = nullptr);
    ~QMultiSampleAntiAliasing();

private:
    Q_DECLARE_PRIVATE(QMultiSampleAntiAliasing)
};

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QMULTISAMPLEANTIALIASING_H
