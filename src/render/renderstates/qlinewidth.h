// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
#ifndef QT3DRENDER_QLINEWIDTH_H
#define QT3DRENDER_QLINEWIDTH_H

#include <Qt3DRender/qrenderstate.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QLineWidthPrivate;

class Q_3DRENDERSHARED_EXPORT QLineWidth : public QRenderState
{
    Q_OBJECT
    Q_PROPERTY(float value READ value WRITE setValue NOTIFY valueChanged)
    Q_PROPERTY(bool smooth READ smooth WRITE setSmooth NOTIFY smoothChanged)

public:
    explicit QLineWidth(Qt3DCore::QNode *parent = nullptr);
    ~QLineWidth();

    float value() const;
    bool smooth() const;

public Q_SLOTS:
    void setValue(float value);
    void setSmooth(bool enabled);

Q_SIGNALS:
    void valueChanged(float value);
    void smoothChanged(bool enabled);

private:
    Q_DECLARE_PRIVATE(QLineWidth)
};

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QTRENDER_QLINEWIDTH_H
