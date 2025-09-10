// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

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
