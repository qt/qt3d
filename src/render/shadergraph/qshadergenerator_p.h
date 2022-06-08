// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_QSHADERGENERATOR_P_H
#define QT3DRENDER_QSHADERGENERATOR_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <Qt3DRender/private/qt3drender_global_p.h>

#include <Qt3DRender/private/qshadergraph_p.h>
#include <QtCore/QLoggingCategory>


QT_BEGIN_NAMESPACE

namespace Qt3DRender
{
Q_DECLARE_LOGGING_CATEGORY(ShaderGenerator)

class QShaderGenerator
{
public:
    Q_3DRENDERSHARED_PRIVATE_EXPORT QByteArray createShaderCode(const QStringList &enabledLayers = QStringList()) const;

    QShaderGraph graph;
    QShaderFormat format;
};

}
Q_DECLARE_TYPEINFO(Qt3DRender::QShaderGenerator, Q_RELOCATABLE_TYPE);
QT_END_NAMESPACE

Q_DECLARE_METATYPE(Qt3DRender::QShaderGenerator)

#endif // QT3DRENDER_QSHADERGENERATOR_P_H
