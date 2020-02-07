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
 */

/*!
   \qmltype RenderCapabilities
   \brief The QRenderCapabilities class holds settings related to available rendering engines
   \since 5.15
   \inqmlmodule Qt3D.Render
   \instantiates Qt3DRender::QRenderCapabilities
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
        res += QString(QLatin1String("  Max Work Group Size: %1, %2, %3\n  Max Work Group Count: %4, %5, %6\n  Max Invocations: %6\n  Max Shared Memory Size: %7\n"))
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


bool QRenderCapabilities::isValid() const
{
    Q_D(const QRenderCapabilities);
    return d->m_valid;
}

QRenderCapabilities::API QRenderCapabilities::api() const
{
    Q_D(const QRenderCapabilities);
    return d->m_api;
}

QRenderCapabilities::Profile QRenderCapabilities::profile() const
{
    Q_D(const QRenderCapabilities);
    return d->m_profile;
}

int QRenderCapabilities::majorVersion() const
{
    Q_D(const QRenderCapabilities);
    return d->m_majorVersion;
}

int QRenderCapabilities::minorVersion() const
{
    Q_D(const QRenderCapabilities);
    return d->m_minorVersion;
}

QStringList QRenderCapabilities::extensions() const
{
    Q_D(const QRenderCapabilities);
    return d->m_extensions;
}

QString QRenderCapabilities::vendor() const
{
    Q_D(const QRenderCapabilities);
    return d->m_vendor;
}

QString QRenderCapabilities::renderer() const
{
    Q_D(const QRenderCapabilities);
    return d->m_renderer;
}

QString QRenderCapabilities::driverVersion() const
{
    Q_D(const QRenderCapabilities);
    return d->m_version;
}

QString QRenderCapabilities::glslVersion() const
{
    Q_D(const QRenderCapabilities);
    return d->m_glslVersion;
}

int QRenderCapabilities::maxSamples() const
{
    Q_D(const QRenderCapabilities);
    return d->m_maxSamples;
}

int QRenderCapabilities::maxTextureSize() const
{
    Q_D(const QRenderCapabilities);
    return d->m_maxTextureSize;
}

int QRenderCapabilities::maxTextureUnits() const
{
    Q_D(const QRenderCapabilities);
    return d->m_maxTextureUnits;
}

int QRenderCapabilities::maxTextureLayers() const
{
    Q_D(const QRenderCapabilities);
    return d->m_maxTextureLayers;
}

bool QRenderCapabilities::supportsUBO() const
{
    Q_D(const QRenderCapabilities);
    return d->m_supportsUBO;
}

int QRenderCapabilities::maxUBOSize() const
{
    Q_D(const QRenderCapabilities);
    return d->m_maxUBOSize;
}

int QRenderCapabilities::maxUBOBindings() const
{
    Q_D(const QRenderCapabilities);
    return d->m_maxUBOBindings;
}

bool QRenderCapabilities::supportsSSBO() const
{
    Q_D(const QRenderCapabilities);
    return d->m_supportsSSBO;
}

int QRenderCapabilities::maxSSBOSize() const
{
    Q_D(const QRenderCapabilities);
    return d->m_maxSSBOSize;
}

int QRenderCapabilities::maxSSBOBindings() const
{
    Q_D(const QRenderCapabilities);
    return d->m_maxSSBOBindings;
}

bool QRenderCapabilities::supportsImageStore() const
{
    Q_D(const QRenderCapabilities);
    return d->m_supportsImageStore;
}

int QRenderCapabilities::maxImageUnits() const
{
    Q_D(const QRenderCapabilities);
    return d->m_maxImageUnits;
}

bool QRenderCapabilities::supportsCompute() const
{
    Q_D(const QRenderCapabilities);
    return d->m_supportCompute;
}

int QRenderCapabilities::maxWorkGroupCountX() const
{
    Q_D(const QRenderCapabilities);
    return d->m_maxWorkGroupCount[0];
}

int QRenderCapabilities::maxWorkGroupCountY() const
{
    Q_D(const QRenderCapabilities);
    return d->m_maxWorkGroupCount[1];
}

int QRenderCapabilities::maxWorkGroupCountZ() const
{
    Q_D(const QRenderCapabilities);
    return d->m_maxWorkGroupCount[2];
}

int QRenderCapabilities::maxWorkGroupSizeX() const
{
    Q_D(const QRenderCapabilities);
    return d->m_maxWorkGroupSize[0];
}

int QRenderCapabilities::maxWorkGroupSizeY() const
{
    Q_D(const QRenderCapabilities);
    return d->m_maxWorkGroupSize[1];
}

int QRenderCapabilities::maxWorkGroupSizeZ() const
{
    Q_D(const QRenderCapabilities);
    return d->m_maxWorkGroupSize[2];
}

int QRenderCapabilities::maxComputeInvocations() const
{
    Q_D(const QRenderCapabilities);
    return d->m_maxComputeInvocations;
}

int QRenderCapabilities::maxComputeSharedMemorySize() const
{
    Q_D(const QRenderCapabilities);
    return d->m_maxComputeSharedMemorySize;
}

} // namespace Qt3Drender

QT_END_NAMESPACE
