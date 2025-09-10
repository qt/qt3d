// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// Copyright (C) 2016 The Qt Company Ltd and/or its subsidiary(-ies).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_QALPHACOVERAGE_H
#define QT3DRENDER_QALPHACOVERAGE_H

#include <Qt3DRender/qrenderstate.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QAlphaCoveragePrivate;

class Q_3DRENDERSHARED_EXPORT QAlphaCoverage : public QRenderState
{
    Q_OBJECT
public:
    explicit QAlphaCoverage(Qt3DCore::QNode *parent = nullptr);
    ~QAlphaCoverage();

private:
    Q_DECLARE_PRIVATE(QAlphaCoverage)
};

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QALPHACOVERAGE_H
