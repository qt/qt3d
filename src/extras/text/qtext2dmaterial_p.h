// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DEXTRAS_QTEXT2DMATERIAL_P_H
#define QT3DEXTRAS_QTEXT2DMATERIAL_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of other Qt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <Qt3DExtras/qt3dextras_global.h>
#include <Qt3DRender/qmaterial.h>
#include <QColor>
#include <private/qglobal_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DExtras {

class QText2DMaterialPrivate;

class QText2DMaterial : public Qt3DRender::QMaterial
{
    Q_OBJECT

public:
    explicit QText2DMaterial(Qt3DCore::QNode *parent = nullptr);
    ~QText2DMaterial();

    void setColor(const QColor &color);
    void setDistanceFieldTexture(Qt3DRender::QAbstractTexture *tex);

private:
    Q_DECLARE_PRIVATE(QText2DMaterial)
};

} // namespace Qt3DExtras

QT_END_NAMESPACE

#endif // QT3DEXTRAS_QTEXT2DMATERIAL_P_H
