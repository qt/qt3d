// Copyright (C) 2015 Lorenz Esch (TU Ilmenau).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DEXTRAS_QPERVERTEXCOLORMATERIAL_H
#define QT3DEXTRAS_QPERVERTEXCOLORMATERIAL_H

#include <Qt3DExtras/qt3dextras_global.h>
#include <Qt3DRender/qmaterial.h>
#include <QtGui/QColor>

QT_BEGIN_NAMESPACE

namespace Qt3DExtras {

class QPerVertexColorMaterialPrivate;

class Q_3DEXTRASSHARED_EXPORT QPerVertexColorMaterial : public Qt3DRender::QMaterial
{
    Q_OBJECT

public:
    explicit QPerVertexColorMaterial(Qt3DCore::QNode *parent = nullptr);
    ~QPerVertexColorMaterial();

private:
    Q_DECLARE_PRIVATE(QPerVertexColorMaterial)
};

} // namespace Qt3DExtras

QT_END_NAMESPACE

#endif // QT3DEXTRAS_QPERVERTEXCOLORMATERIAL_H
