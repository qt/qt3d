// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_QSHADERNODEPORT_P_H
#define QT3DRENDER_QSHADERNODEPORT_P_H

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

#include <QtCore/qstring.h>
#include <QtCore/qvariant.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender
{
class QShaderNodePort
{
public:
    enum Direction : char {
        Input,
        Output
    };

    Q_3DRENDERSHARED_PRIVATE_EXPORT QShaderNodePort() noexcept;

    QShaderNodePort::Direction direction;
    QString name;
};

Q_3DRENDERSHARED_PRIVATE_EXPORT bool operator==(const QShaderNodePort &lhs, const QShaderNodePort &rhs) noexcept;

inline bool operator!=(const QShaderNodePort &lhs, const QShaderNodePort &rhs) noexcept
{
    return !(lhs == rhs);
}


}
Q_DECLARE_TYPEINFO(Qt3DRender::QShaderNodePort, Q_RELOCATABLE_TYPE);
QT_END_NAMESPACE

Q_DECLARE_METATYPE(Qt3DRender::QShaderNodePort)

#endif // QT3DRENDER_QSHADERNODEPORT_P_H
