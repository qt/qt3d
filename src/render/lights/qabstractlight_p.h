// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_QABSTRACTLIGHT_P_H
#define QT3DRENDER_QABSTRACTLIGHT_P_H

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

#include <private/qcomponent_p.h>
#include <Qt3DRender/private/qt3drender_global_p.h>
#include <qshaderdata.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QAbstractLight;

class Q_3DRENDERSHARED_PRIVATE_EXPORT QAbstractLightPrivate : public Qt3DCore::QComponentPrivate
{
public:
    explicit QAbstractLightPrivate(QAbstractLight::Type type);
    ~QAbstractLightPrivate();

    Q_DECLARE_PUBLIC(QAbstractLight)
    QAbstractLight::Type m_type;
    QShaderData *m_shaderData;
};

}

QT_END_NAMESPACE

#endif // QT3DRENDER_QABSTRACTLIGHT_P_H
