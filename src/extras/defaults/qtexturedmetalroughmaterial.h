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
****************************************************************************/

#ifndef QT3DEXTRAS_QTEXTUREDMETALROUGHMATERIAL_H
#define QT3DEXTRAS_QTEXTUREDMETALROUGHMATERIAL_H

#include <Qt3DExtras/qmetalroughmaterial.h>

QT_BEGIN_NAMESPACE

namespace Qt3DExtras {

class QT_DEPRECATED_X("Use Qt3DExtras::QMetalRoughMaterial") Q_3DEXTRASSHARED_EXPORT QTexturedMetalRoughMaterial : public QMetalRoughMaterial
{
    Q_OBJECT
    Q_PROPERTY(QVariant ambientOcclusion READ ambientOcclusion WRITE setAmbientOcclusion NOTIFY ambientOcclusionChanged)
    Q_PROPERTY(QVariant normal READ normal WRITE setNormal NOTIFY normalChanged)

public:
    explicit QTexturedMetalRoughMaterial(Qt3DCore::QNode *parent = nullptr);

Q_SIGNALS:
    void ambientOcclusionChanged(const QVariant &ambientOcclusion);
    void normalChanged(const QVariant &normal);
};

} // namespace Qt3DExtras

QT_END_NAMESPACE

#endif // QT3DEXTRAS_QTEXTUREDMETALROUGHMATERIAL_H
