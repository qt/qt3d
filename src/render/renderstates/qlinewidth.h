/****************************************************************************
**
** Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
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
