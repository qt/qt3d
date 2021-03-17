/****************************************************************************
**
** Copyright (C) 2020 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qrendercapabilities.h"
#include "qrendercapabilities_p.h"

#include <QOffscreenSurface>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QOpenGLExtraFunctions>
#include <QSet>
#include <QDebug>

#ifndef GL_MAX_UNIFORM_BLOCK_SIZE
#define GL_MAX_UNIFORM_BLOCK_SIZE               0x8A30
#endif

#ifndef GL_MAX_UNIFORM_BLOCK_SIZE
#define GL_MAX_UNIFORM_BLOCK_SIZE               0x8A30
#endif

#ifndef GL_MAX_SAMPLES
#define GL_MAX_SAMPLES                          0x8D57
#endif

#ifndef GL_MAX_UNIFORM_BUFFER_BINDINGS
#define GL_MAX_UNIFORM_BUFFER_BINDINGS          0x8A2F
#endif

#ifndef GL_MAX_UNIFORM_BLOCK_SIZE
#define GL_MAX_UNIFORM_BLOCK_SIZE               0x8A30
#endif

#ifndef GL_MAX_ARRAY_TEXTURE_LAYERS
#define GL_MAX_ARRAY_TEXTURE_LAYERS             0x88FF
#endif

#ifndef GL_MAX_IMAGE_UNITS
#define GL_MAX_IMAGE_UNITS                      0x8F38
#endif

#ifndef GL_MAX_SHADER_STORAGE_BLOCK_SIZE
#define GL_MAX_SHADER_STORAGE_BLOCK_SIZE        0x90DE
#endif

#ifndef GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS
#define GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS   0x90DD
#endif

#ifndef GL_MAX_COMPUTE_WORK_GROUP_SIZE
#define GL_MAX_COMPUTE_WORK_GROUP_SIZE          0x91BF
#endif

#ifndef GL_MAX_COMPUTE_WORK_GROUP_COUNT
#define GL_MAX_COMPUTE_WORK_GROUP_COUNT         0x91BE
#endif

#ifndef GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS
#define GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS   0x90EB
#endif

#ifndef GL_MAX_COMPUTE_SHARED_MEMORY_SIZE
#define GL_MAX_COMPUTE_SHARED_MEMORY_SIZE       0x8262
#endif


QT_BEGIN_NAMESPACE

namespace Qt3DRender {

/*!
   \class Qt3DRender::QRenderCapabilities
   \brief The QRenderCapabilities class holds settings related to available rendering engines

QRenderCapabilities provides details of graphical features that are available at runtime.
It can be used to decide which code path to use for some algorithms, for example, depending
on whether compute shaders are available or not.

\since 5.15
*/

/*!
   \qmltype RenderCapabilities
   \brief The QRenderCapabilities class holds settings related to available rendering engines
   \since 5.15
   \inqmlmodule Qt3D.Render
   \instantiates Qt3DRender::QRenderCapabilities

RenderCapabilities provides details of graphical features that are available at runtime.
It can be used to decide which code path to use for some algorithms, for example, depending
on whether compute shaders are available or not.

\since 5.15
*/


/*! \internal */
QRenderCapabilitiesPrivate::QRenderCapabilitiesPrivate()
    : QObjectPrivate()
    , m_valid(false)
{
    QOffscreenSurface offscreen;
    QOpenGLContext ctx;

    offscreen.setFormat(QSurfaceFormat::defaultFormat());
    offscreen.create();
    Q_ASSERT_X(offscreen.isValid(), Q_FUNC_INFO, "Unable to create offscreen surface to gather capabilities");

    ctx.setFormat(QSurfaceFormat::defaultFormat());
    if (ctx.create()) {
        m_valid = true;
        ctx.makeCurrent(&offscreen);
        const QSurfaceFormat format = ctx.format();
        auto funcs = ctx.functions();

        if (ctx.isOpenGLES())
            m_api = QRenderCapabilities::OpenGLES;
        else
            m_api = QRenderCapabilities::OpenGL;
        m_profile = static_cast<QRenderCapabilities::Profile>(format.profile());
        m_majorVersion = format.majorVersion();
        m_minorVersion = format.minorVersion();
        const QSet<QByteArray> extensions = ctx.extensions();
        std::transform(std::begin(extensions), std::end(extensions), std::back_inserter(m_extensions), [](const QByteArray &e) {
            return QLatin1String(e.data());
        });
        std::sort(std::begin(m_extensions), std::end(m_extensions));
        m_vendor = QString::fromUtf8(reinterpret_cast<const char *>(funcs->glGetString(GL_VENDOR)));
        m_renderer = QString::fromUtf8(reinterpret_cast<const char *>(funcs->glGetString(GL_RENDERER)));
        m_version = QString::fromUtf8(reinterpret_cast<const char *>(funcs->glGetString(GL_VERSION)));
        m_glslVersion = QString::fromUtf8(reinterpret_cast<const char *>(funcs->glGetString(GL_SHADING_LANGUAGE_VERSION)));

        funcs->glGetIntegerv(GL_MAX_SAMPLES, &m_maxSamples);
        funcs->glGetIntegerv(GL_MAX_TEXTURE_SIZE, &m_maxTextureSize);
        funcs->glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &m_maxTextureUnits);

        if (m_majorVersion >= 3) {
            if (m_minorVersion >= 1) {
                m_supportsUBO = true;
                funcs->glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &m_maxUBOSize);
                funcs->glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &m_maxUBOBindings);
            }

            funcs->glGetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS, &m_maxTextureLayers);
        }
        if ((m_api == QRenderCapabilities::OpenGL && m_majorVersion >= 4 && m_minorVersion >= 3) ||
            (m_api == QRenderCapabilities::OpenGLES && m_majorVersion >= 3 && m_minorVersion >= 2)) {
            m_supportsSSBO = true;
            m_supportCompute = true;
            m_supportsImageStore = true;
            funcs->glGetIntegerv(GL_MAX_IMAGE_UNITS, &m_maxImageUnits);
            m_supportsSSBO = true;
            funcs->glGetIntegerv(GL_MAX_SHADER_STORAGE_BLOCK_SIZE, &m_maxSSBOSize);
            funcs->glGetIntegerv(GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS, &m_maxSSBOBindings);

            QOpenGLExtraFunctions *exfunc = nullptr;
            if ((exfunc = ctx.extraFunctions()) != nullptr) {
                for (int i = 0; i < 3; ++i) {
                    exfunc->glGetIntegeri_v( GL_MAX_COMPUTE_WORK_GROUP_SIZE, i, &m_maxWorkGroupSize[i] );
                    exfunc->glGetIntegeri_v( GL_MAX_COMPUTE_WORK_GROUP_COUNT, i, &m_maxWorkGroupCount[i] );
                }
            }

            funcs->glGetIntegerv( GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &m_maxComputeInvocations );
            funcs->glGetIntegerv( GL_MAX_COMPUTE_SHARED_MEMORY_SIZE, &m_maxComputeSharedMemorySize );
        }

        // TextureUnitCount, ImageUnitCount, ComputeSupport, ImageSupport, SSBO Support, UBO Support, extensions, Max RenderTargetCount would be a good start
    }
}

const QRenderCapabilitiesPrivate *QRenderCapabilitiesPrivate::get(const QRenderCapabilities *q)
{
    return q->d_func();
}

QString QRenderCapabilitiesPrivate::toString() const
{
    QString res;

    res += QString(QLatin1String("Vendor: %1\nRenderer: %2\nDriver Version: %3\nGL Version: %4.%5 (%6 Profile)\nGLSL Version: %7\n"))
               .arg(m_vendor, m_renderer, m_version)
               .arg(m_majorVersion).arg(m_minorVersion)
               .arg(m_profile == QRenderCapabilities::CoreProfile ? QLatin1String("Core") : (m_profile == QRenderCapabilities::CompatibilityProfile ? QLatin1String("Compatibility") : QLatin1String("No")))
               .arg(m_glslVersion);
    res += QString(QLatin1String("Extensions:\n  %1\n")).arg(m_extensions.join(QLatin1String("\n  ")));
    res += QString(QLatin1String("Max Texture Size: %1\nMax Texture Units: %2\nMax Texture Layers: %3\n")).arg(m_maxTextureSize).arg(m_maxTextureUnits).arg(m_maxTextureLayers);
    res += QString(QLatin1String("Supports UBO: %1\n")).arg(m_supportsUBO ? QLatin1String("True") : QLatin1String("False"));
    if (m_supportsUBO)
        res += QString(QLatin1String("  Max UBO Size: %1\n  Max UBO Bindings: %2\n")).arg(m_maxUBOSize).arg(m_maxUBOBindings);
    res += QString(QLatin1String("Supports SSBO: %1\n")).arg(m_supportsSSBO ? QLatin1String("True") : QLatin1String("False"));
    if (m_supportsSSBO)
        res += QString(QLatin1String("  Max SSBO Size: %1\n  Max SSBO Bindings: %2\n")).arg(m_maxSSBOSize).arg(m_maxSSBOBindings);
    res += QString(QLatin1String("Supports Image Store: %1\n")).arg(m_supportsImageStore ? QLatin1String("True") : QLatin1String("False"));
    if (m_supportsImageStore)
        res += QString(QLatin1String("  Max Image Units: %1\n")).arg(m_maxImageUnits);
    res += QString(QLatin1String("Supports Compute Shaders: %1\n")).arg(m_supportCompute ? QLatin1String("True") : QLatin1String("False"));
    if (m_supportCompute)
        res += QString(QLatin1String("  Max Work Group Size: %1, %2, %3\n  Max Work Group Count: %4, %5, %6\n  Max Invocations: %7\n  Max Shared Memory Size: %8\n"))
                   .arg(m_maxWorkGroupSize[0]).arg(m_maxWorkGroupSize[1]).arg(m_maxWorkGroupSize[2])
                   .arg(m_maxWorkGroupCount[0]).arg(m_maxWorkGroupCount[1]).arg(m_maxWorkGroupCount[2])
                   .arg(m_maxComputeInvocations).arg(m_maxComputeSharedMemorySize);
    return res;
}


QRenderCapabilities::QRenderCapabilities(QObject *parent)
    : QObject(*new QRenderCapabilitiesPrivate, parent)
{

}

/*! \internal */
QRenderCapabilities::~QRenderCapabilities()
{
}


/*!
\qmlproperty bool RenderCapabilities::valid

true if the data is valid, otherwise Qt 3D failed to query the available hardware.

\readonly
*/
/*!
\property QRenderCapabilities::valid

true if the data is valid, otherwise Qt 3D failed to query the available hardware.

\readonly
*/
bool QRenderCapabilities::isValid() const
{
    Q_D(const QRenderCapabilities);
    return d->m_valid;
}

/*!
\qmlproperty QRenderCapabilities::API RenderCapabilities::api

Returns which API is currently in use.

\readonly
*/
/*!
\property QRenderCapabilities::api

Returns which API is currently in use.

\readonly
*/
QRenderCapabilities::API QRenderCapabilities::api() const
{
    Q_D(const QRenderCapabilities);
    return d->m_api;
}

/*!
\qmlproperty bool RenderCapabilities::profile

Returns which profile (if applicable) is currently in use.

\readonly
*/
/*!
\property QRenderCapabilities::profile

Returns which profile (if applicable) is currently in use.

\readonly
*/
QRenderCapabilities::Profile QRenderCapabilities::profile() const
{
    Q_D(const QRenderCapabilities);
    return d->m_profile;
}

/*!
\qmlproperty int RenderCapabilities::majorVersion

Returns the major version number currently in use.

\readonly
*/
/*!
\property QRenderCapabilities::majorVersion

Returns the major version number currently in use.

\readonly
*/
int QRenderCapabilities::majorVersion() const
{
    Q_D(const QRenderCapabilities);
    return d->m_majorVersion;
}

/*!
\qmlproperty int RenderCapabilities::minorVersion

Returns the minor version number currently in use.

\readonly
*/
/*!
\property QRenderCapabilities::minorVersion

Returns the minor version number currently in use.

\readonly
*/
int QRenderCapabilities::minorVersion() const
{
    Q_D(const QRenderCapabilities);
    return d->m_minorVersion;
}

/*!
\qmlproperty QStringList RenderCapabilities::extensions

Returns the list of extensions currently available.

\readonly
*/
/*!
\property QRenderCapabilities::extensions

Returns the list of extensions currently available.

\readonly
*/
QStringList QRenderCapabilities::extensions() const
{
    Q_D(const QRenderCapabilities);
    return d->m_extensions;
}

/*!
\qmlproperty QString RenderCapabilities::vendor

Returns the vendor identification string.

\readonly
*/
/*!
\property QRenderCapabilities::vendor

Returns the vendor identification string.

\readonly
*/
QString QRenderCapabilities::vendor() const
{
    Q_D(const QRenderCapabilities);
    return d->m_vendor;
}

/*!
\qmlproperty QString RenderCapabilities::renderer

Returns the device identification string.

\readonly
*/
/*!
\property QRenderCapabilities::renderer

Returns the device identification string.

\readonly
*/
QString QRenderCapabilities::renderer() const
{
    Q_D(const QRenderCapabilities);
    return d->m_renderer;
}

/*!
\qmlproperty QString RenderCapabilities::driverVersion

Returns the driver version string.

\readonly
*/
/*!
\property QRenderCapabilities::driverVersion

Returns the driver version string.

\readonly
*/
QString QRenderCapabilities::driverVersion() const
{
    Q_D(const QRenderCapabilities);
    return d->m_version;
}

/*!
\qmlproperty QString RenderCapabilities::glslVersion

Returns the GLSL version string.

\readonly
*/
/*!
\property QRenderCapabilities::glslVersion

Returns the GLSL version string.

\readonly
*/
QString QRenderCapabilities::glslVersion() const
{
    Q_D(const QRenderCapabilities);
    return d->m_glslVersion;
}

/*!
\qmlproperty int RenderCapabilities::maxSamples

Returns the maximum number of samples available for MSAA.

\readonly
*/
/*!
\property QRenderCapabilities::maxSamples

Returns the maximum number of samples available for MSAA.

\readonly
*/
int QRenderCapabilities::maxSamples() const
{
    Q_D(const QRenderCapabilities);
    return d->m_maxSamples;
}

/*!
\qmlproperty int RenderCapabilities::maxTextureSize

Returns the maximum size of textures.

\readonly
*/
/*!
\property QRenderCapabilities::maxTextureSize

Returns the maximum size of textures.

\readonly
*/
int QRenderCapabilities::maxTextureSize() const
{
    Q_D(const QRenderCapabilities);
    return d->m_maxTextureSize;
}

/*!
\qmlproperty int RenderCapabilities::maxTextureUnits

Returns the number of available texture units.

\readonly
*/
/*!
\property QRenderCapabilities::maxTextureUnits

Returns the number of available texture units.

\readonly
*/
int QRenderCapabilities::maxTextureUnits() const
{
    Q_D(const QRenderCapabilities);
    return d->m_maxTextureUnits;
}

/*!
\qmlproperty int RenderCapabilities::maxTextureLayers

Returns the number of available texture layers.

\readonly
*/
/*!
\property QRenderCapabilities::maxTextureLayers

Returns the number of available texture layers.

\readonly
*/
int QRenderCapabilities::maxTextureLayers() const
{
    Q_D(const QRenderCapabilities);
    return d->m_maxTextureLayers;
}

/*!
\qmlproperty bool RenderCapabilities::supportsUBO

Returns true if UBOs are supported.

\readonly
*/
/*!
\property QRenderCapabilities::supportsUBO

Returns true if UBOs are supported.

\readonly
*/
bool QRenderCapabilities::supportsUBO() const
{
    Q_D(const QRenderCapabilities);
    return d->m_supportsUBO;
}

/*!
\qmlproperty int RenderCapabilities::maxUBOSize

Returns the maximum size of UBOs, if supported.

\readonly
*/
/*!
\property QRenderCapabilities::maxUBOSize

Returns the maximum size of UBOs, if supported.

\readonly
*/
int QRenderCapabilities::maxUBOSize() const
{
    Q_D(const QRenderCapabilities);
    return d->m_maxUBOSize;
}

/*!
\qmlproperty int RenderCapabilities::maxUBOBindings

Returns the maximum number of available UBO binding points, if supported.

\readonly
*/
/*!
\property QRenderCapabilities::maxUBOBindings

Returns the maximum number of available UBO binding points, if supported.

\readonly
*/
int QRenderCapabilities::maxUBOBindings() const
{
    Q_D(const QRenderCapabilities);
    return d->m_maxUBOBindings;
}

/*!
\qmlproperty bool RenderCapabilities::supportsSSBO

Returns true if SSBOs are supported.

\readonly
*/
/*!
\property QRenderCapabilities::supportsSSBO

Returns true if SSBOs are supported.

\readonly
*/
bool QRenderCapabilities::supportsSSBO() const
{
    Q_D(const QRenderCapabilities);
    return d->m_supportsSSBO;
}

/*!
\qmlproperty int RenderCapabilities::maxSSBOSize

Returns the maximum size of SSBOs, if available.

\readonly
*/
/*!
\property QRenderCapabilities::maxSSBOSize

Returns the maximum size of SSBOs, if available.

\readonly
*/
int QRenderCapabilities::maxSSBOSize() const
{
    Q_D(const QRenderCapabilities);
    return d->m_maxSSBOSize;
}

/*!
\qmlproperty int RenderCapabilities::maxSSBOBindings

Returns the maximum number of available SSBO binding points, if supported.

\readonly
*/
/*!
\property QRenderCapabilities::maxSSBOBindings

Returns the maximum number of available SSBO binding points, if supported.

\readonly
*/
int QRenderCapabilities::maxSSBOBindings() const
{
    Q_D(const QRenderCapabilities);
    return d->m_maxSSBOBindings;
}

/*!
\qmlproperty bool RenderCapabilities::supportsImageStore

Returns true if Image Store operations are supported.

\readonly
*/
/*!
\property QRenderCapabilities::supportsImageStore

Returns true if Image Store operations are supported.

\readonly
*/
bool QRenderCapabilities::supportsImageStore() const
{
    Q_D(const QRenderCapabilities);
    return d->m_supportsImageStore;
}

/*!
\qmlproperty int RenderCapabilities::maxImageUnits

Returns the maximum number of available image units.

\readonly
*/
/*!
\property QRenderCapabilities::maxImageUnits

Returns the maximum number of available image units.

\readonly
*/
int QRenderCapabilities::maxImageUnits() const
{
    Q_D(const QRenderCapabilities);
    return d->m_maxImageUnits;
}

/*!
\qmlproperty bool RenderCapabilities::supportsCompute

Returns true if Compute Shaders are supported.

\readonly
*/
/*!
\property QRenderCapabilities::supportsCompute

Returns true if Compute Shaders are supported.

\readonly
*/
bool QRenderCapabilities::supportsCompute() const
{
    Q_D(const QRenderCapabilities);
    return d->m_supportCompute;
}

/*!
\qmlproperty int RenderCapabilities::maxWorkGroupCountX

Returns the maximum number of available Compute Shader workgroups in the X axis.

\readonly
*/
/*!
\property QRenderCapabilities::maxWorkGroupCountX

Returns the maximum number of available Compute Shader workgroups in the X axis.

\readonly
*/
int QRenderCapabilities::maxWorkGroupCountX() const
{
    Q_D(const QRenderCapabilities);
    return d->m_maxWorkGroupCount[0];
}

/*!
\qmlproperty int RenderCapabilities::maxWorkGroupCountY

Returns the maximum number of available Compute Shader workgroups in the Y axis.

\readonly
*/
/*!
\property QRenderCapabilities::maxWorkGroupCountY

Returns the maximum number of available Compute Shader workgroups in the Y axis.

\readonly
*/
int QRenderCapabilities::maxWorkGroupCountY() const
{
    Q_D(const QRenderCapabilities);
    return d->m_maxWorkGroupCount[1];
}

/*!
\qmlproperty int RenderCapabilities::maxWorkGroupCountZ

Returns the maximum number of available Compute Shader workgroups in the Z axis.

\readonly
*/
/*!
\property QRenderCapabilities::maxWorkGroupCountZ

Returns the maximum number of available Compute Shader workgroups in the Z axis.

\readonly
*/
int QRenderCapabilities::maxWorkGroupCountZ() const
{
    Q_D(const QRenderCapabilities);
    return d->m_maxWorkGroupCount[2];
}

/*!
\qmlproperty int RenderCapabilities::maxWorkGroupSizeX

Returns the maximum size of Compute Shader local workgroups in the X axis.

\readonly
*/
/*!
\property QRenderCapabilities::maxWorkGroupSizeX

Returns the maximum size of Compute Shader local workgroups in the X axis.

\readonly
*/
int QRenderCapabilities::maxWorkGroupSizeX() const
{
    Q_D(const QRenderCapabilities);
    return d->m_maxWorkGroupSize[0];
}

/*!
\qmlproperty int RenderCapabilities::maxWorkGroupSizeY

Returns the maximum size of Compute Shader local workgroups in the Y axis.

\readonly
*/
/*!
\property QRenderCapabilities::maxWorkGroupSizeY

Returns the maximum size of Compute Shader local workgroups in the Y axis.

\readonly
*/
int QRenderCapabilities::maxWorkGroupSizeY() const
{
    Q_D(const QRenderCapabilities);
    return d->m_maxWorkGroupSize[1];
}

/*!
\qmlproperty int RenderCapabilities::maxWorkGroupSizeZ

Returns the maximum size of Compute Shader local workgroups in the Z axis.

\readonly
*/
/*!
\property QRenderCapabilities::maxWorkGroupSizeZ

Returns the maximum size of Compute Shader local workgroups in the Z axis.

\readonly
*/
int QRenderCapabilities::maxWorkGroupSizeZ() const
{
    Q_D(const QRenderCapabilities);
    return d->m_maxWorkGroupSize[2];
}

/*!
\qmlproperty int RenderCapabilities::maxComputeInvocations

Returns the maximum number of Compute Shaders available.

\readonly
*/
/*!
\property QRenderCapabilities::maxComputeInvocations

Returns the maximum number of Compute Shaders available.

\readonly
*/
int QRenderCapabilities::maxComputeInvocations() const
{
    Q_D(const QRenderCapabilities);
    return d->m_maxComputeInvocations;
}

/*!
\qmlproperty int RenderCapabilities::maxComputeSharedMemorySize

Returns the maximum amount of shared memory available for Compute Shaders.

\readonly
*/
/*!
\property QRenderCapabilities::maxComputeSharedMemorySize

Returns the maximum amount of shared memory available for Compute Shaders.

\readonly
*/
int QRenderCapabilities::maxComputeSharedMemorySize() const
{
    Q_D(const QRenderCapabilities);
    return d->m_maxComputeSharedMemorySize;
}

} // namespace Qt3Drender

QT_END_NAMESPACE
