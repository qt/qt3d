/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
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

#ifndef QT3DEXTRAS_QNORMALDIFFUSEMAPALPHAMATERIAL_H
#define QT3DEXTRAS_QNORMALDIFFUSEMAPALPHAMATERIAL_H

#include <Qt3DExtras/qt3dextras_global.h>
#include <Qt3DExtras/qnormaldiffusemapmaterial.h>

QT_BEGIN_NAMESPACE

namespace Qt3DExtras {

class QNormalDiffuseMapAlphaMaterialPrivate;

class Q_3DEXTRASSHARED_EXPORT QNormalDiffuseMapAlphaMaterial : public QNormalDiffuseMapMaterial
{
    Q_OBJECT
public:
    explicit QNormalDiffuseMapAlphaMaterial(Qt3DCore::QNode *parent = nullptr);
    ~QNormalDiffuseMapAlphaMaterial();

private:
    Q_DECLARE_PRIVATE(QNormalDiffuseMapAlphaMaterial)
};

} // namespace Qt3DExtras

QT_END_NAMESPACE

#endif // QT3DEXTRAS_QNORMALDIFFUSEMAPALPHAMATERIAL_H
