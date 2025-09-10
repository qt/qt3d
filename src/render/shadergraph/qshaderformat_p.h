// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_QSHADERFORMAT_P_H
#define QT3DRENDER_QSHADERFORMAT_P_H

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

#include <QtCore/qstringlist.h>
#include <QtCore/qversionnumber.h>

QT_BEGIN_NAMESPACE
namespace Qt3DRender
{
class QShaderFormat
{
public:
    enum Api : int {
        NoApi,
        OpenGLNoProfile,
        OpenGLCoreProfile,
        OpenGLCompatibilityProfile,
        OpenGLES,
        VulkanFlavoredGLSL,
        RHI
    };

    enum ShaderType : int {
        Vertex = 0,
        TessellationControl,
        TessellationEvaluation,
        Geometry,
        Fragment,
        Compute
    };

    Q_3DRENDERSHARED_PRIVATE_EXPORT QShaderFormat() noexcept;

    Q_3DRENDERSHARED_PRIVATE_EXPORT Api api() const noexcept;
    Q_3DRENDERSHARED_PRIVATE_EXPORT void setApi(Api api) noexcept;

    Q_3DRENDERSHARED_PRIVATE_EXPORT QVersionNumber version() const noexcept;
    Q_3DRENDERSHARED_PRIVATE_EXPORT void setVersion(const QVersionNumber &version) noexcept;

    Q_3DRENDERSHARED_PRIVATE_EXPORT QStringList extensions() const noexcept;
    Q_3DRENDERSHARED_PRIVATE_EXPORT void setExtensions(const QStringList &extensions) noexcept;

    Q_3DRENDERSHARED_PRIVATE_EXPORT QString vendor() const noexcept;
    Q_3DRENDERSHARED_PRIVATE_EXPORT void setVendor(const QString &vendor) noexcept;

    Q_3DRENDERSHARED_PRIVATE_EXPORT bool isValid() const noexcept;
    Q_3DRENDERSHARED_PRIVATE_EXPORT bool supports(const QShaderFormat &other) const noexcept;

    Q_3DRENDERSHARED_PRIVATE_EXPORT ShaderType shaderType() const noexcept;
    Q_3DRENDERSHARED_PRIVATE_EXPORT void setShaderType(ShaderType shaderType) noexcept;

private:
    Api m_api;
    QVersionNumber m_version;
    QStringList m_extensions;
    QString m_vendor;
    ShaderType m_shaderType;
};

Q_3DRENDERSHARED_PRIVATE_EXPORT bool operator==(const QShaderFormat &lhs, const QShaderFormat &rhs) noexcept;

inline bool operator!=(const QShaderFormat &lhs, const QShaderFormat &rhs) noexcept
{
    return !(lhs == rhs);
}


}
Q_DECLARE_TYPEINFO(Qt3DRender::QShaderFormat, Q_RELOCATABLE_TYPE);
QT_END_NAMESPACE

Q_DECLARE_METATYPE(Qt3DRender::QShaderFormat)

#endif // QT3DRENDER_QSHADERFORMAT_P_H
