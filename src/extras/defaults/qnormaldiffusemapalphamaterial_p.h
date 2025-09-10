// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DEXTRAS_QNORMALDIFFUSEMAPALPHAMATERIAL_P_H
#define QT3DEXTRAS_QNORMALDIFFUSEMAPALPHAMATERIAL_P_H

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

#include <Qt3DExtras/private/qnormaldiffusemapmaterial_p.h>

QT_BEGIN_NAMESPACE


namespace Qt3DRender {

class QAlphaCoverage;
class QDepthTest;

} // namespace Qt3DRender

namespace Qt3DExtras {

class QNormalDiffuseMapAlphaMaterial;
class QNormalDiffuseMapAlphaMaterialPrivate: public QNormalDiffuseMapMaterialPrivate
{
public:
    QNormalDiffuseMapAlphaMaterialPrivate();

    void init() override;

    Qt3DRender::QAlphaCoverage *m_alphaCoverage;
    Qt3DRender::QDepthTest *m_depthTest;

    Q_DECLARE_PUBLIC(QNormalDiffuseMapAlphaMaterial)
};

} // Qt3DExtras

QT_END_NAMESPACE

#endif // QT3DEXTRAS_QNORMALDIFFUSEMAPALPHAMATERIAL_P_H

