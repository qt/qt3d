/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Copyright (C) 2016 The Qt Company Ltd and/or its subsidiary(-ies).
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

#include "renderview_p.h"
#include <Qt3DRender/qmaterial.h>
#include <Qt3DRender/qrenderaspect.h>
#include <Qt3DRender/qrendertarget.h>
#include <Qt3DRender/qabstractlight.h>
#include <Qt3DRender/private/sphere_p.h>

#include <Qt3DRender/private/cameraselectornode_p.h>
#include <Qt3DRender/private/framegraphnode_p.h>
#include <Qt3DRender/private/layerfilternode_p.h>
#include <Qt3DRender/private/qparameter_p.h>
#include <Qt3DRender/private/cameralens_p.h>
#include <Qt3DRender/private/rendercommand_p.h>
#include <Qt3DRender/private/effect_p.h>
#include <Qt3DRender/private/entity_p.h>
#include <Qt3DRender/private/renderer_p.h>
#include <Qt3DRender/private/nodemanagers_p.h>
#include <Qt3DRender/private/layer_p.h>
#include <Qt3DRender/private/renderlogging_p.h>
#include <Qt3DRender/private/renderpassfilternode_p.h>
#include <Qt3DRender/private/renderpass_p.h>
#include <Qt3DRender/private/geometryrenderer_p.h>
#include <Qt3DRender/private/renderstateset_p.h>
#include <Qt3DRender/private/techniquefilternode_p.h>
#include <Qt3DRender/private/viewportnode_p.h>
#include <Qt3DRender/private/buffermanager_p.h>
#include <Qt3DRender/private/geometryrenderermanager_p.h>
#include <Qt3DRender/private/rendercapture_p.h>
#include <Qt3DRender/private/buffercapture_p.h>
#include <Qt3DRender/private/stringtoint_p.h>
#include <Qt3DCore/qentity.h>
#include <QtGui/qsurface.h>
#include <algorithm>

#include <QDebug>
#if defined(QT3D_RENDER_VIEW_JOB_TIMINGS)
#include <QElapsedTimer>
#endif

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {


namespace  {

// register our QNodeId's as a metatype during program loading
const int Q_DECL_UNUSED qNodeIdTypeId = qMetaTypeId<Qt3DCore::QNodeId>();

const int MAX_LIGHTS = 8;

#define LIGHT_POSITION_NAME  QLatin1String(".position")
#define LIGHT_TYPE_NAME      QLatin1String(".type")
#define LIGHT_COLOR_NAME     QLatin1String(".color")
#define LIGHT_INTENSITY_NAME QLatin1String(".intensity")

int LIGHT_COUNT_NAME_ID = 0;
int LIGHT_POSITION_NAMES[MAX_LIGHTS];
int LIGHT_TYPE_NAMES[MAX_LIGHTS];
int LIGHT_COLOR_NAMES[MAX_LIGHTS];
int LIGHT_INTENSITY_NAMES[MAX_LIGHTS];
QString LIGHT_STRUCT_NAMES[MAX_LIGHTS];

int LIGHT_POSITION_UNROLL_NAMES[MAX_LIGHTS];
int LIGHT_TYPE_UNROLL_NAMES[MAX_LIGHTS];
int LIGHT_COLOR_UNROLL_NAMES[MAX_LIGHTS];
int LIGHT_INTENSITY_UNROLL_NAMES[MAX_LIGHTS];
QString LIGHT_STRUCT_UNROLL_NAMES[MAX_LIGHTS];

bool wasInitialized = false;

} // anonymous namespace

RenderView::StandardUniformsNameToTypeHash RenderView::ms_standardUniformSetters;


RenderView::StandardUniformsNameToTypeHash RenderView::initializeStandardUniformSetters()
{
    RenderView::StandardUniformsNameToTypeHash setters;

    setters.insert(StringToInt::lookupId(QLatin1String("modelMatrix")), ModelMatrix);
    setters.insert(StringToInt::lookupId(QLatin1String("viewMatrix")), ViewMatrix);
    setters.insert(StringToInt::lookupId(QLatin1String("projectionMatrix")), ProjectionMatrix);
    setters.insert(StringToInt::lookupId(QLatin1String("modelView")), ModelViewMatrix);
    setters.insert(StringToInt::lookupId(QLatin1String("viewProjectionMatrix")), ViewProjectionMatrix);
    setters.insert(StringToInt::lookupId(QLatin1String("modelViewProjection")), ModelViewProjectionMatrix);
    setters.insert(StringToInt::lookupId(QLatin1String("mvp")), ModelViewProjectionMatrix);
    setters.insert(StringToInt::lookupId(QLatin1String("inverseModelMatrix")), InverseModelMatrix);
    setters.insert(StringToInt::lookupId(QLatin1String("inverseViewMatrix")), InverseViewMatrix);
    setters.insert(StringToInt::lookupId(QLatin1String("inverseProjectionMatrix")), InverseProjectionMatrix);
    setters.insert(StringToInt::lookupId(QLatin1String("inverseModelView")), InverseModelViewMatrix);
    setters.insert(StringToInt::lookupId(QLatin1String("inverseViewProjectionMatrix")), InverseViewProjectionMatrix);
    setters.insert(StringToInt::lookupId(QLatin1String("inverseModelViewProjection")), InverseModelViewProjectionMatrix);
    setters.insert(StringToInt::lookupId(QLatin1String("modelNormalMatrix")), ModelNormalMatrix);
    setters.insert(StringToInt::lookupId(QLatin1String("modelViewNormal")), ModelViewNormalMatrix);
    setters.insert(StringToInt::lookupId(QLatin1String("viewportMatrix")), ViewportMatrix);
    setters.insert(StringToInt::lookupId(QLatin1String("inverseViewportMatrix")), InverseViewportMatrix);
    setters.insert(StringToInt::lookupId(QLatin1String("aspectRatio")), AspectRatio);
    setters.insert(StringToInt::lookupId(QLatin1String("exposure")), Exposure);
    setters.insert(StringToInt::lookupId(QLatin1String("gamma")), Gamma);
    setters.insert(StringToInt::lookupId(QLatin1String("time")), Time);
    setters.insert(StringToInt::lookupId(QLatin1String("eyePosition")), EyePosition);
    setters.insert(StringToInt::lookupId(QLatin1String("skinningPalette[0]")), SkinningPalette);

    return setters;
}

// TODO: Move this somewhere global where GraphicsContext::setViewport() can use it too
static QRectF resolveViewport(const QRectF &fractionalViewport, const QSize &surfaceSize)
{
    return QRectF(fractionalViewport.x() * surfaceSize.width(),
                  (1.0 - fractionalViewport.y() - fractionalViewport.height()) * surfaceSize.height(),
                  fractionalViewport.width() * surfaceSize.width(),
                  fractionalViewport.height() * surfaceSize.height());
}

static Matrix4x4 getProjectionMatrix(const CameraLens *lens)
{
    if (!lens)
        qWarning() << "[Qt3D Renderer] No Camera Lens found. Add a CameraSelector to your Frame Graph or make sure that no entities will be rendered.";
    return lens ? lens->projection() : Matrix4x4();
}

UniformValue RenderView::standardUniformValue(RenderView::StandardUniform standardUniformType,
                                              Entity *entity,
                                              const Matrix4x4 &model) const
{
    switch (standardUniformType) {
    case ModelMatrix:
        return UniformValue(model);
    case ViewMatrix:
        return UniformValue(m_data.m_viewMatrix);
    case ProjectionMatrix:
        return UniformValue(getProjectionMatrix(m_data.m_renderCameraLens));
    case ModelViewMatrix:
        return UniformValue(m_data.m_viewMatrix * model);
    case ViewProjectionMatrix:
        return UniformValue(getProjectionMatrix(m_data.m_renderCameraLens) * m_data.m_viewMatrix);
    case ModelViewProjectionMatrix:
        return UniformValue(m_data.m_viewProjectionMatrix * model);
    case InverseModelMatrix:
        return UniformValue(model.inverted());
    case InverseViewMatrix:
        return UniformValue(m_data.m_viewMatrix.inverted());
    case InverseProjectionMatrix: {
        return UniformValue(getProjectionMatrix(m_data.m_renderCameraLens).inverted());
    }
    case InverseModelViewMatrix:
        return UniformValue((m_data.m_viewMatrix * model).inverted());
    case InverseViewProjectionMatrix: {
        const Matrix4x4 viewProjectionMatrix = getProjectionMatrix(m_data.m_renderCameraLens) * m_data.m_viewMatrix;
        return UniformValue(viewProjectionMatrix.inverted());
    }
    case InverseModelViewProjectionMatrix:
        return UniformValue((m_data.m_viewProjectionMatrix * model).inverted());
    case ModelNormalMatrix:
        return UniformValue(convertToQMatrix4x4(model).normalMatrix());
    case ModelViewNormalMatrix:
        return UniformValue(convertToQMatrix4x4(m_data.m_viewMatrix * model).normalMatrix());
    case ViewportMatrix: {
        QMatrix4x4 viewportMatrix;
        // TO DO: Implement on Matrix4x4
        viewportMatrix.viewport(resolveViewport(m_viewport, m_surfaceSize));
        return UniformValue(Matrix4x4(viewportMatrix));
    }
    case InverseViewportMatrix: {
        QMatrix4x4 viewportMatrix;
        // TO DO: Implement on Matrix4x4
        viewportMatrix.viewport(resolveViewport(m_viewport, m_surfaceSize));
        return UniformValue(Matrix4x4(viewportMatrix.inverted()));
    }
    case AspectRatio:
        return float(m_surfaceSize.width()) / float(m_surfaceSize.height());
    case Exposure:
        return UniformValue(m_data.m_renderCameraLens ? m_data.m_renderCameraLens->exposure() : 0.0f);
    case Gamma:
        return UniformValue(m_gamma);
    case Time:
        return UniformValue(float(m_renderer->time() / 1000000000.0f));
    case EyePosition:
        return UniformValue(m_data.m_eyePos);
    case SkinningPalette: {
        const Armature *armature = entity->renderComponent<Armature>();
        if (!armature) {
            qCWarning(Jobs, "Requesting skinningPalette uniform but no armature set on entity");
            return UniformValue();
        }
        return armature->skinningPaletteUniform();
    }
    default:
        Q_UNREACHABLE();
        return UniformValue();
    }
}

RenderView::RenderView()
    : m_isDownloadBuffersEnable(false)
    , m_hasBlitFramebufferInfo(false)
    , m_renderer(nullptr)
    , m_manager(nullptr)
    , m_devicePixelRatio(1.)
    , m_viewport(QRectF(0., 0., 1., 1.))
    , m_gamma(2.2f)
    , m_surface(nullptr)
    , m_clearBuffer(QClearBuffers::None)
    , m_clearDepthValue(1.f)
    , m_clearStencilValue(0)
    , m_stateSet(nullptr)
    , m_noDraw(false)
    , m_compute(false)
    , m_frustumCulling(false)
    , m_memoryBarrier(QMemoryBarrier::None)
    , m_environmentLight(nullptr)
{
    m_workGroups[0] = 1;
    m_workGroups[1] = 1;
    m_workGroups[2] = 1;

    if (Q_UNLIKELY(!wasInitialized)) {
        // Needed as we can control the init order of static/global variables across compile units
        // and this hash relies on the static StringToInt class
        wasInitialized = true;
        RenderView::ms_standardUniformSetters = RenderView::initializeStandardUniformSetters();
        LIGHT_COUNT_NAME_ID = StringToInt::lookupId(QLatin1String("lightCount"));
        for (int i = 0; i < MAX_LIGHTS; ++i) {
            Q_STATIC_ASSERT_X(MAX_LIGHTS < 10, "can't use the QChar trick anymore");
            LIGHT_STRUCT_NAMES[i] = QLatin1String("lights[") + QLatin1Char(char('0' + i)) + QLatin1Char(']');
            LIGHT_POSITION_NAMES[i] = StringToInt::lookupId(LIGHT_STRUCT_NAMES[i] + LIGHT_POSITION_NAME);
            LIGHT_TYPE_NAMES[i] = StringToInt::lookupId(LIGHT_STRUCT_NAMES[i] + LIGHT_TYPE_NAME);
            LIGHT_COLOR_NAMES[i] = StringToInt::lookupId(LIGHT_STRUCT_NAMES[i] + LIGHT_COLOR_NAME);
            LIGHT_INTENSITY_NAMES[i] = StringToInt::lookupId(LIGHT_STRUCT_NAMES[i] + LIGHT_INTENSITY_NAME);

            LIGHT_STRUCT_UNROLL_NAMES[i] = QLatin1String("light_") + QLatin1Char(char('0' + i));
            LIGHT_POSITION_UNROLL_NAMES[i] = StringToInt::lookupId(LIGHT_STRUCT_UNROLL_NAMES[i] + LIGHT_POSITION_NAME);
            LIGHT_TYPE_UNROLL_NAMES[i] = StringToInt::lookupId(LIGHT_STRUCT_UNROLL_NAMES[i] + LIGHT_TYPE_NAME);
            LIGHT_COLOR_UNROLL_NAMES[i] = StringToInt::lookupId(LIGHT_STRUCT_UNROLL_NAMES[i] + LIGHT_COLOR_NAME);
            LIGHT_INTENSITY_UNROLL_NAMES[i] = StringToInt::lookupId(LIGHT_STRUCT_UNROLL_NAMES[i] + LIGHT_INTENSITY_NAME);
        }
    }
}

RenderView::~RenderView()
{
}

namespace {

template<int SortType>
struct AdjacentSubRangeFinder
{
    static bool adjacentSubRange(const RenderCommand &, const RenderCommand &)
    {
        Q_UNREACHABLE();
        return false;
    }
};

template<>
struct AdjacentSubRangeFinder<QSortPolicy::StateChangeCost>
{
    static bool adjacentSubRange(const RenderCommand &a, const RenderCommand &b)
    {
        return a.m_changeCost == b.m_changeCost;
    }
};

template<>
struct AdjacentSubRangeFinder<QSortPolicy::BackToFront>
{
    static bool adjacentSubRange(const RenderCommand &a, const RenderCommand &b)
    {
        return a.m_depth == b.m_depth;
    }
};

template<>
struct AdjacentSubRangeFinder<QSortPolicy::Material>
{
    static bool adjacentSubRange(const RenderCommand &a, const RenderCommand &b)
    {
        return a.m_shaderDna == b.m_shaderDna;
    }
};

template<>
struct AdjacentSubRangeFinder<QSortPolicy::FrontToBack>
{
    static bool adjacentSubRange(const RenderCommand &a, const RenderCommand &b)
    {
        return a.m_depth == b.m_depth;
    }
};

template<>
struct AdjacentSubRangeFinder<QSortPolicy::Texture>
{
    static bool adjacentSubRange(const RenderCommand &a, const RenderCommand &b)
    {
        // Two renderCommands are adjacent if one contains all the other command's textures
        QVector<ShaderParameterPack::NamedResource> texturesA = a.m_parameterPack.textures();
        QVector<ShaderParameterPack::NamedResource> texturesB = b.m_parameterPack.textures();

        if (texturesB.size() > texturesA.size())
            qSwap(texturesA, texturesB);

        // textureB.size() is always <= textureA.size()
        for (const ShaderParameterPack::NamedResource &texB : qAsConst(texturesB)) {
            if (!texturesA.contains(texB))
                return false;
        }
        return true;
    }
};

template<typename Predicate>
int advanceUntilNonAdjacent(const QVector<RenderCommand> &commands,
                            const int beg, const int end, Predicate pred)
{
    int i = beg + 1;
    while (i < end) {
        if (!pred(*(commands.begin() + beg), *(commands.begin() + i)))
            break;
        ++i;
    }
    return i;
}


using CommandIt = QVector<RenderCommand>::iterator;

template<int SortType>
struct SubRangeSorter
{
    static void sortSubRange(CommandIt begin, const CommandIt end)
    {
        Q_UNUSED(begin);
        Q_UNUSED(end);
        Q_UNREACHABLE();
    }
};

template<>
struct SubRangeSorter<QSortPolicy::StateChangeCost>
{
    static void sortSubRange(CommandIt begin, const CommandIt end)
    {
        std::stable_sort(begin, end, [] (const RenderCommand &a, const RenderCommand &b) {
            return a.m_changeCost > b.m_changeCost;
        });
    }
};

template<>
struct SubRangeSorter<QSortPolicy::BackToFront>
{
    static void sortSubRange(CommandIt begin, const CommandIt end)
    {
        std::stable_sort(begin, end, [] (const RenderCommand &a, const RenderCommand &b) {
            return a.m_depth > b.m_depth;
        });
    }
};

template<>
struct SubRangeSorter<QSortPolicy::Material>
{
    static void sortSubRange(CommandIt begin, const CommandIt end)
    {
        // First we sort by shaderDNA
        std::stable_sort(begin, end, [] (const RenderCommand &a, const RenderCommand &b) {
            return a.m_shaderDna > b.m_shaderDna;
        });
    }
};

template<>
struct SubRangeSorter<QSortPolicy::FrontToBack>
{
    static void sortSubRange(CommandIt begin, const CommandIt end)
    {
        std::stable_sort(begin, end, [] (const RenderCommand &a, const RenderCommand &b) {
            return a.m_depth < b.m_depth;
        });
    }
};

template<>
struct SubRangeSorter<QSortPolicy::Texture>
{
    static void sortSubRange(CommandIt begin, const CommandIt end)
    {
        std::stable_sort(begin, end, [] (const RenderCommand &a, const RenderCommand &b) {
            QVector<ShaderParameterPack::NamedResource> texturesA = a.m_parameterPack.textures();
            QVector<ShaderParameterPack::NamedResource> texturesB = b.m_parameterPack.textures();

            const int originalTextureASize = texturesA.size();

            if (texturesB.size() > texturesA.size())
                qSwap(texturesA, texturesB);

            int identicalTextureCount = 0;

            for (const ShaderParameterPack::NamedResource &texB : qAsConst(texturesB)) {
                if (texturesA.contains(texB))
                    ++identicalTextureCount;
            }

            return identicalTextureCount < originalTextureASize;
        });
    }
};

int findSubRange(const QVector<RenderCommand> &commands,
                 const int begin, const int end,
                 const QSortPolicy::SortType sortType)
{
    switch (sortType) {
    case QSortPolicy::StateChangeCost:
        return advanceUntilNonAdjacent(commands, begin, end, AdjacentSubRangeFinder<QSortPolicy::StateChangeCost>::adjacentSubRange);
    case QSortPolicy::BackToFront:
        return advanceUntilNonAdjacent(commands, begin, end, AdjacentSubRangeFinder<QSortPolicy::BackToFront>::adjacentSubRange);
    case QSortPolicy::Material:
        return advanceUntilNonAdjacent(commands, begin, end, AdjacentSubRangeFinder<QSortPolicy::Material>::adjacentSubRange);
    case QSortPolicy::FrontToBack:
        return advanceUntilNonAdjacent(commands, begin, end, AdjacentSubRangeFinder<QSortPolicy::FrontToBack>::adjacentSubRange);
    case QSortPolicy::Texture:
        return advanceUntilNonAdjacent(commands, begin, end, AdjacentSubRangeFinder<QSortPolicy::Texture>::adjacentSubRange);
    default:
        Q_UNREACHABLE();
        return end;
    }
}

void sortByMaterial(QVector<RenderCommand> &commands, int begin, const int end)
{
    // We try to arrange elements so that their rendering cost is minimized for a given shader
    int rangeEnd = advanceUntilNonAdjacent(commands, begin, end, AdjacentSubRangeFinder<QSortPolicy::Material>::adjacentSubRange);
    while (begin != end) {
        if (begin + 1 < rangeEnd) {
            std::stable_sort(commands.begin() + begin + 1, commands.begin() + rangeEnd, [] (const RenderCommand &a, const RenderCommand &b){
                return a.m_material.handle() < b.m_material.handle();
            });
        }
        begin = rangeEnd;
        rangeEnd = advanceUntilNonAdjacent(commands, begin, end, AdjacentSubRangeFinder<QSortPolicy::Material>::adjacentSubRange);
    }
}

void sortCommandRange(QVector<RenderCommand> &commands, int begin, const int end, const int level,
                      const QVector<Qt3DRender::QSortPolicy::SortType> &sortingTypes)
{
    if (level >= sortingTypes.size())
        return;

    switch (sortingTypes.at(level)) {
    case QSortPolicy::StateChangeCost:
        SubRangeSorter<QSortPolicy::StateChangeCost>::sortSubRange(commands.begin() + begin, commands.begin() + end);
        break;
    case QSortPolicy::BackToFront:
        SubRangeSorter<QSortPolicy::BackToFront>::sortSubRange(commands.begin() + begin, commands.begin() + end);
        break;
    case QSortPolicy::Material:
        // Groups all same shader DNA together
        SubRangeSorter<QSortPolicy::Material>::sortSubRange(commands.begin() + begin, commands.begin() + end);
        // Group all same material together (same parameters most likely)
        sortByMaterial(commands, begin, end);
        break;
    case QSortPolicy::FrontToBack:
        SubRangeSorter<QSortPolicy::FrontToBack>::sortSubRange(commands.begin() + begin, commands.begin() + end);
        break;
    case QSortPolicy::Texture:
        SubRangeSorter<QSortPolicy::Texture>::sortSubRange(commands.begin() + begin, commands.begin() + end);
        break;
    default:
        Q_UNREACHABLE();
    }

    // For all sub ranges of adjacent item for sortType[i]
    // Perform filtering with sortType[i + 1]
    int rangeEnd = findSubRange(commands, begin, end, sortingTypes.at(level));
    while (begin != end) {
        sortCommandRange(commands, begin, rangeEnd, level + 1, sortingTypes);
        begin = rangeEnd;
        rangeEnd = findSubRange(commands, begin, end, sortingTypes.at(level));
    }
}

} // anonymous

void RenderView::sort()
{
     sortCommandRange(m_commands, 0, m_commands.size(), 0, m_data.m_sortingTypes);

    // For RenderCommand with the same shader
    // We compute the adjacent change cost

    // Minimize uniform changes
    int i = 0;
    const int commandSize = m_commands.size();
    while (i < commandSize) {
        int j = i;

        // Advance while commands share the same shader
        while (i < commandSize && m_commands[j].m_shaderDna == m_commands[i].m_shaderDna)
            ++i;

        if (i - j > 0) { // Several commands have the same shader, so we minimize uniform changes
            PackUniformHash cachedUniforms = m_commands[j++].m_parameterPack.uniforms();

            while (j < i) {
                // We need the reference here as we are modifying the original container
                // not the copy
                PackUniformHash &uniforms = m_commands[j].m_parameterPack.m_uniforms;

                for (int u = 0; u < uniforms.keys.size();) {
                    // We are comparing the values:
                    // - raw uniform values
                    // - the texture Node id if the uniform represents a texture
                    // since all textures are assigned texture units before the RenderCommands
                    // sharing the same material (shader) are rendered, we can't have the case
                    // where two uniforms, referencing the same texture eventually have 2 different
                    // texture unit values
                    const int uniformNameId = uniforms.keys.at(u);
                    const UniformValue &refValue = cachedUniforms.value(uniformNameId);
                    const UniformValue &newValue = uniforms.values.at(u);
                    if (newValue == refValue) {
                        uniforms.erase(u);
                    } else {
                        // Record updated value so that subsequent comparison
                        // for the next command will be made againts latest
                        // uniform value
                        cachedUniforms.insert(uniformNameId, newValue);
                        ++u;
                    }
                }
                ++j;
            }
        }
    }
}

void RenderView::setRenderer(Renderer *renderer)
{
    m_renderer = renderer;
    m_manager = renderer->nodeManagers();
}

void RenderView::addClearBuffers(const ClearBuffers *cb) {
    QClearBuffers::BufferTypeFlags type = cb->type();

    if (type & QClearBuffers::StencilBuffer) {
        m_clearStencilValue = cb->clearStencilValue();
        m_clearBuffer |= QClearBuffers::StencilBuffer;
    }
    if (type & QClearBuffers::DepthBuffer) {
        m_clearDepthValue = cb->clearDepthValue();
        m_clearBuffer |= QClearBuffers::DepthBuffer;
    }
    // keep track of global ClearColor (if set) and collect all DrawBuffer-specific
    // ClearColors
    if (type & QClearBuffers::ColorBuffer) {
        ClearBufferInfo clearBufferInfo;
        clearBufferInfo.clearColor = cb->clearColor();

        if (cb->clearsAllColorBuffers()) {
            m_globalClearColorBuffer = clearBufferInfo;
            m_clearBuffer |= QClearBuffers::ColorBuffer;
        } else {
            if (cb->bufferId()) {
                const RenderTargetOutput *targetOutput = m_manager->attachmentManager()->lookupResource(cb->bufferId());
                if (targetOutput) {
                    clearBufferInfo.attchmentPoint = targetOutput->point();
                    // Note: a job is later performed to find the drawIndex from the buffer attachment point
                    // using the AttachmentPack
                    m_specificClearColorBuffers.push_back(clearBufferInfo);
                }
            }
        }
    }
}

// If we are there, we know that entity had a GeometryRenderer + Material
EntityRenderCommandData RenderView::buildDrawRenderCommands(const QVector<Entity *> &entities) const
{
    EntityRenderCommandData commands;

    commands.reserve(entities.size());

    for (Entity *entity : entities) {
        GeometryRenderer *geometryRenderer = nullptr;
        HGeometryRenderer geometryRendererHandle = entity->componentHandle<GeometryRenderer>();

        // There is a geometry renderer with geometry
        if ((geometryRenderer = m_manager->geometryRendererManager()->data(geometryRendererHandle)) != nullptr
                && geometryRenderer->isEnabled()
                && !geometryRenderer->geometryId().isNull()) {

            const Qt3DCore::QNodeId materialComponentId = entity->componentUuid<Material>();
            const HMaterial materialHandle = entity->componentHandle<Material>();
            const  QVector<RenderPassParameterData> renderPassData = m_parameters.value(materialComponentId);

            // 1 RenderCommand per RenderPass pass on an Entity with a Mesh
            for (const RenderPassParameterData &passData : renderPassData) {
                // Add the RenderPass Parameters
                RenderCommand command = {};
                command.m_geometryRenderer = geometryRendererHandle;
                command.m_geometry = m_manager->geometryManager()->lookupHandle(geometryRenderer->geometryId());

                command.m_material = materialHandle;
                // For RenderPass based states we use the globally set RenderState
                // if no renderstates are defined as part of the pass. That means:
                // RenderPass { renderStates: [] } will use the states defined by
                // StateSet in the FrameGraph
                RenderPass *pass = passData.pass;
                if (pass->hasRenderStates()) {
                    command.m_stateSet = RenderStateSetPtr::create();
                    addStatesToRenderStateSet(command.m_stateSet.data(), pass->renderStates(), m_manager->renderStateManager());
                    if (m_stateSet != nullptr)
                        command.m_stateSet->merge(m_stateSet);
                    command.m_changeCost = m_renderer->defaultRenderState()->changeCost(command.m_stateSet.data());
                }
                command.m_shader = m_manager->lookupHandle<Shader, ShaderManager, HShader>(pass->shaderProgram());

                commands.push_back(entity,
                                   std::move(command),
                                   std::move(passData));
            }
        }
    }

    return commands;
}

EntityRenderCommandData RenderView::buildComputeRenderCommands(const QVector<Entity *> &entities) const
{
    // If the RenderView contains only a ComputeDispatch then it cares about
    // A ComputeDispatch is also implicitely a NoDraw operation
    // enabled flag
    // layer component
    // material/effect/technique/parameters/filters/
    EntityRenderCommandData commands;

    commands.reserve(entities.size());

    for (Entity *entity : entities) {
        ComputeCommand *computeJob = nullptr;
        HComputeCommand computeCommandHandle = entity->componentHandle<ComputeCommand>();
        if ((computeJob = nodeManagers()->computeJobManager()->data(computeCommandHandle)) != nullptr
                && computeJob->isEnabled()) {

            const Qt3DCore::QNodeId materialComponentId = entity->componentUuid<Material>();
            const  QVector<RenderPassParameterData> renderPassData = m_parameters.value(materialComponentId);

            // 1 RenderCommand per RenderPass pass on an Entity with a Mesh
            for (const RenderPassParameterData &passData : renderPassData) {
                // Add the RenderPass Parameters
                RenderCommand command = {};
                RenderPass *pass = passData.pass;

                if (pass->hasRenderStates()) {
                    command.m_stateSet = RenderStateSetPtr::create();
                    addStatesToRenderStateSet(command.m_stateSet.data(), pass->renderStates(), m_manager->renderStateManager());

                    // Merge per pass stateset with global stateset
                    // so that the local stateset only overrides
                    if (m_stateSet != nullptr)
                        command.m_stateSet->merge(m_stateSet);
                    command.m_changeCost = m_renderer->defaultRenderState()->changeCost(command.m_stateSet.data());
                }
                command.m_shader = m_manager->lookupHandle<Shader, ShaderManager, HShader>(pass->shaderProgram());
                command.m_computeCommand = computeCommandHandle;
                command.m_type = RenderCommand::Compute;
                command.m_workGroups[0] = std::max(m_workGroups[0], computeJob->x());
                command.m_workGroups[1] = std::max(m_workGroups[1], computeJob->y());
                command.m_workGroups[2] = std::max(m_workGroups[2], computeJob->z());

                commands.push_back(entity,
                                   std::move(command),
                                   std::move(passData));
            }
        }
    }

    return commands;
}

void RenderView::updateRenderCommand(EntityRenderCommandData &renderCommandData)
{
    // Note: since many threads can be building render commands
    // we need to ensure that the UniformBlockValueBuilder they are using
    // is only accessed from the same thread
    UniformBlockValueBuilder *builder = new UniformBlockValueBuilder();
    builder->shaderDataManager = m_manager->shaderDataManager();
    builder->textureManager = m_manager->textureManager();
    m_localData.setLocalData(builder);

    for (int i = 0, m = renderCommandData.size(); i < m; ++i) {
        Entity *entity = renderCommandData.entities.at(i);
        const RenderPassParameterData passData = renderCommandData.passesData.at(i);
        RenderCommand &command = renderCommandData.commands[i];

        // Pick which lights to take in to account.
        // For now decide based on the distance by taking the MAX_LIGHTS closest lights.
        // Replace with more sophisticated mechanisms later.
        // Copy vector so that we can sort it concurrently and we only want to sort the one for the current command
        QVector<LightSource> lightSources;
        EnvironmentLight *environmentLight = nullptr;

        if (command.m_type == RenderCommand::Draw) {
            // Project the camera-to-object-center vector onto the camera
            // view vector. This gives a depth value suitable as the key
            // for BackToFront sorting.
            command.m_depth = Vector3D::dotProduct(entity->worldBoundingVolume()->center() - m_data.m_eyePos, m_data.m_eyeViewDir);

            environmentLight = m_environmentLight;
            lightSources = m_lightSources;

            if (lightSources.size() > 1) {
                const Vector3D entityCenter = entity->worldBoundingVolume()->center();
                std::sort(lightSources.begin(), lightSources.end(),
                          [&] (const LightSource &a, const LightSource &b) {
                    const float distA = entityCenter.distanceToPoint(a.entity->worldBoundingVolume()->center());
                    const float distB = entityCenter.distanceToPoint(b.entity->worldBoundingVolume()->center());
                    return distA < distB;
                });
            }
            lightSources = lightSources.mid(0, std::max(lightSources.size(), MAX_LIGHTS));
        } else { // Compute
            // Note: if frameCount has reached 0 in the previous frame, isEnabled
            // would be false
            ComputeCommand *computeJob = m_manager->computeJobManager()->data(command.m_computeCommand);
            if (computeJob->runType() == QComputeCommand::Manual)
                computeJob->updateFrameCount();
        }

        ParameterInfoList globalParameters = passData.parameterInfo;
        // setShaderAndUniforms can initialize a localData
        // make sure this is cleared before we leave this function
        setShaderAndUniforms(&command,
                             globalParameters,
                             entity,
                             lightSources,
                             environmentLight);
    }

    // We reset the local data once we are done with it
    m_localData.setLocalData(nullptr);
}

void RenderView::updateMatrices()
{
    if (m_data.m_renderCameraNode && m_data.m_renderCameraLens && m_data.m_renderCameraLens->isEnabled()) {
        const Matrix4x4 cameraWorld = *(m_data.m_renderCameraNode->worldTransform());
        setViewMatrix(m_data.m_renderCameraLens->viewMatrix(cameraWorld));

        setViewProjectionMatrix(m_data.m_renderCameraLens->projection() * viewMatrix());
        //To get the eyePosition of the camera, we need to use the inverse of the
        //camera's worldTransform matrix.
        const Matrix4x4 inverseWorldTransform = viewMatrix().inverted();
        const Vector3D eyePosition(inverseWorldTransform.column(3));
        setEyePosition(eyePosition);

        // Get the viewing direction of the camera. Use the normal matrix to
        // ensure non-uniform scale works too.
        const QMatrix3x3 normalMat = convertToQMatrix4x4(m_data.m_viewMatrix).normalMatrix();
        // dir = normalize(QVector3D(0, 0, -1) * normalMat)
        setEyeViewDirection(Vector3D(-normalMat(2, 0), -normalMat(2, 1), -normalMat(2, 2)).normalized());
    }
}

void RenderView::setUniformValue(ShaderParameterPack &uniformPack, int nameId, const UniformValue &value) const
{
    // At this point a uniform value can only be a scalar type
    // or a Qt3DCore::QNodeId corresponding to a Texture or Image
    // ShaderData/Buffers would be handled as UBO/SSBO and would therefore
    // not be in the default uniform block
    if (value.valueType() == UniformValue::NodeId) {
        const Qt3DCore::QNodeId *nodeIds = value.constData<Qt3DCore::QNodeId>();

        const int uniformArraySize = value.byteSize() / sizeof(Qt3DCore::QNodeId);
        UniformValue::ValueType resourceType = UniformValue::TextureValue;

        for (int i = 0; i < uniformArraySize; ++i) {
            const Qt3DCore::QNodeId resourceId = nodeIds[i];

            const Texture *tex =  m_manager->textureManager()->lookupResource(resourceId);
            if (tex != nullptr) {
                uniformPack.setTexture(nameId, i, resourceId);
            } else {
                const ShaderImage *img = m_manager->shaderImageManager()->lookupResource(resourceId);
                if (img != nullptr) {
                    resourceType = UniformValue::ShaderImageValue;
                    uniformPack.setImage(nameId, i, resourceId);
                }
            }
        }

        // This uniform will be overridden in SubmissionContext::setParameters
        // and -1 values will be replaced by valid Texture or Image units
        UniformValue uniformValue(uniformArraySize * sizeof(int), resourceType);
        std::fill(uniformValue.data<int>(), uniformValue.data<int>() + uniformArraySize, -1);
        uniformPack.setUniform(nameId, uniformValue);
    } else {
        uniformPack.setUniform(nameId, value);
    }
}

void RenderView::setStandardUniformValue(ShaderParameterPack &uniformPack,
                                         int glslNameId,
                                         int nameId,
                                         Entity *entity,
                                         const Matrix4x4 &worldTransform) const
{
    uniformPack.setUniform(glslNameId, standardUniformValue(ms_standardUniformSetters[nameId], entity, worldTransform));
}

void RenderView::setUniformBlockValue(ShaderParameterPack &uniformPack,
                                      Shader *shader,
                                      const ShaderUniformBlock &block,
                                      const UniformValue &value) const
{
    Q_UNUSED(shader)

    if (value.valueType() == UniformValue::NodeId) {

        Buffer *buffer = nullptr;
        if ((buffer = m_manager->bufferManager()->lookupResource(*value.constData<Qt3DCore::QNodeId>())) != nullptr) {
            BlockToUBO uniformBlockUBO;
            uniformBlockUBO.m_blockIndex = block.m_index;
            uniformBlockUBO.m_bufferID = buffer->peerId();
            uniformBlockUBO.m_needsUpdate = false;
            uniformPack.setUniformBuffer(std::move(uniformBlockUBO));
            // Buffer update to GL buffer will be done at render time
        }
    }
}

void RenderView::setShaderStorageValue(ShaderParameterPack &uniformPack,
                                       Shader *shader,
                                       const ShaderStorageBlock &block,
                                       const UniformValue &value) const
{
    Q_UNUSED(shader)
    if (value.valueType() == UniformValue::NodeId) {
        Buffer *buffer = nullptr;
        if ((buffer = m_manager->bufferManager()->lookupResource(*value.constData<Qt3DCore::QNodeId>())) != nullptr) {
            BlockToSSBO shaderStorageBlock;
            shaderStorageBlock.m_blockIndex = block.m_index;
            shaderStorageBlock.m_bufferID = buffer->peerId();
            shaderStorageBlock.m_bindingIndex = block.m_binding;
            uniformPack.setShaderStorageBuffer(shaderStorageBlock);
            // Buffer update to GL buffer will be done at render time
        }
    }
}

void RenderView::setDefaultUniformBlockShaderDataValue(ShaderParameterPack &uniformPack, Shader *shader, ShaderData *shaderData, const QString &structName) const
{
    UniformBlockValueBuilder *builder = m_localData.localData();
    builder->activeUniformNamesToValue.clear();

    // Set the view matrix to be used to transform "Transformed" properties in the ShaderData
    builder->viewMatrix = m_data.m_viewMatrix;
    // Force to update the whole block
    builder->updatedPropertiesOnly = false;
    // Retrieve names and description of each active uniforms in the uniform block
    builder->uniforms = shader->activeUniformsForUniformBlock(-1);
    // Build name-value map for the block
    builder->buildActiveUniformNameValueMapStructHelper(shaderData, structName);
    // Set uniform values for each entrie of the block name-value map
    QHash<int, QVariant>::const_iterator activeValuesIt = builder->activeUniformNamesToValue.constBegin();
    const QHash<int, QVariant>::const_iterator activeValuesEnd = builder->activeUniformNamesToValue.constEnd();

    // TO DO: Make the ShaderData store UniformValue
    while (activeValuesIt != activeValuesEnd) {
        setUniformValue(uniformPack, activeValuesIt.key(), UniformValue::fromVariant(activeValuesIt.value()));
        ++activeValuesIt;
    }
}

void RenderView::setShaderAndUniforms(RenderCommand *command,
                                      ParameterInfoList &parameters,
                                      Entity *entity,
                                      const QVector<LightSource> &activeLightSources,
                                      EnvironmentLight *environmentLight) const
{
    // The VAO Handle is set directly in the renderer thread so as to avoid having to use a mutex here
    // Set shader, technique, and effect by basically doing :
    // ShaderProgramManager[MaterialManager[frontentEntity->id()]->Effect->Techniques[TechniqueFilter->name]->RenderPasses[RenderPassFilter->name]];
    // The Renderer knows that if one of those is null, a default material / technique / effect as to be used

    // Find all RenderPasses (in order) matching values set in the RenderPassFilter
    // Get list of parameters for the Material, Effect, and Technique
    // For each ParameterBinder in the RenderPass -> create a QUniformPack
    // Once that works, improve that to try and minimize QUniformPack updates

    // Index Shader by Shader UUID
    Shader *shader = m_manager->data<Shader, ShaderManager>(command->m_shader);
    if (shader != nullptr && shader->isLoaded()) {
        command->m_shaderDna = shader->dna();

        // Builds the QUniformPack, sets shader standard uniforms and store attributes name / glname bindings
        // If a parameter is defined and not found in the bindings it is assumed to be a binding of Uniform type with the glsl name
        // equals to the parameter name
        const QVector<int> uniformNamesIds = shader->uniformsNamesIds();
        const QVector<int> uniformBlockNamesIds = shader->uniformBlockNamesIds();
        const QVector<int> shaderStorageBlockNamesIds = shader->storageBlockNamesIds();
        const QVector<int> attributeNamesIds = shader->attributeNamesIds();

        // Set fragData Name and index
        // Later on we might want to relink the shader if attachments have changed
        // But for now we set them once and for all
        if (!m_renderTarget.isNull() && !shader->isLoaded()) {
            QHash<QString, int> fragOutputs;
            const auto atts = m_attachmentPack.attachments();
            for (const Attachment &att : atts) {
                if (att.m_point <= QRenderTargetOutput::Color15)
                    fragOutputs.insert(att.m_name, att.m_point);
            }
            // Set frag outputs in the shaders if hash not empty
            if (!fragOutputs.isEmpty())
                shader->setFragOutputs(fragOutputs);
        }

        if (!uniformNamesIds.isEmpty() || !attributeNamesIds.isEmpty() ||
                !shaderStorageBlockNamesIds.isEmpty() || !attributeNamesIds.isEmpty()) {

            // Set default standard uniforms without bindings
            const Matrix4x4 worldTransform = *(entity->worldTransform());

            for (const int uniformNameId : uniformNamesIds) {
                if (ms_standardUniformSetters.contains(uniformNameId))
                    setStandardUniformValue(command->m_parameterPack, uniformNameId, uniformNameId, entity, worldTransform);
            }

            // Set default attributes
            command->m_activeAttributes = attributeNamesIds;

            // Parameters remaining could be
            // -> uniform scalar / vector
            // -> uniform struct / arrays
            // -> uniform block / array (4.3)
            // -> ssbo block / array (4.3)

            ParameterInfoList::const_iterator it = parameters.cbegin();
            const ParameterInfoList::const_iterator parametersEnd = parameters.cend();

            while (it != parametersEnd) {
                Parameter *param = m_manager->data<Parameter, ParameterManager>(it->handle);
                const UniformValue &uniformValue = param->uniformValue();
                if (uniformNamesIds.contains(it->nameId)) { // Parameter is a regular uniform
                    setUniformValue(command->m_parameterPack, it->nameId, uniformValue);
                } else if (uniformBlockNamesIds.indexOf(it->nameId) != -1) { // Parameter is a uniform block
                    setUniformBlockValue(command->m_parameterPack, shader, shader->uniformBlockForBlockNameId(it->nameId), uniformValue);
                } else if (shaderStorageBlockNamesIds.indexOf(it->nameId) != -1) { // Parameters is a SSBO
                    setShaderStorageValue(command->m_parameterPack, shader, shader->storageBlockForBlockNameId(it->nameId), uniformValue);
                } else { // Parameter is a struct
                    ShaderData *shaderData = nullptr;
                    if (uniformValue.valueType() == UniformValue::NodeId &&
                            (shaderData = m_manager->shaderDataManager()->lookupResource(*uniformValue.constData<Qt3DCore::QNodeId>())) != nullptr) {
                        // Try to check if we have a struct or array matching a QShaderData parameter
                        setDefaultUniformBlockShaderDataValue(command->m_parameterPack, shader, shaderData, StringToInt::lookupString(it->nameId));
                    }
                    // Otherwise: param unused by current shader
                }
                ++it;
            }

            // Lights

            int lightIdx = 0;
            for (const LightSource &lightSource : activeLightSources) {
                if (lightIdx == MAX_LIGHTS)
                    break;
                Entity *lightEntity = lightSource.entity;
                const Matrix4x4 lightWorldTransform = *(lightEntity->worldTransform());
                const Vector3D worldPos = lightWorldTransform * Vector3D(0.0f, 0.0f, 0.0f);
                for (Light *light : lightSource.lights) {
                    if (!light->isEnabled())
                        continue;

                    ShaderData *shaderData = m_manager->shaderDataManager()->lookupResource(light->shaderData());
                    if (!shaderData)
                        continue;

                    if (lightIdx == MAX_LIGHTS)
                        break;

                    // Note: implicit conversion of values to UniformValue
                    setUniformValue(command->m_parameterPack, LIGHT_POSITION_NAMES[lightIdx], worldPos);
                    setUniformValue(command->m_parameterPack, LIGHT_TYPE_NAMES[lightIdx], int(QAbstractLight::PointLight));
                    setUniformValue(command->m_parameterPack, LIGHT_COLOR_NAMES[lightIdx], Vector3D(1.0f, 1.0f, 1.0f));
                    setUniformValue(command->m_parameterPack, LIGHT_INTENSITY_NAMES[lightIdx], 0.5f);

                    setUniformValue(command->m_parameterPack, LIGHT_POSITION_UNROLL_NAMES[lightIdx], worldPos);
                    setUniformValue(command->m_parameterPack, LIGHT_TYPE_UNROLL_NAMES[lightIdx], int(QAbstractLight::PointLight));
                    setUniformValue(command->m_parameterPack, LIGHT_COLOR_UNROLL_NAMES[lightIdx], Vector3D(1.0f, 1.0f, 1.0f));
                    setUniformValue(command->m_parameterPack, LIGHT_INTENSITY_UNROLL_NAMES[lightIdx], 0.5f);


                    // There is no risk in doing that even if multithreaded
                    // since we are sure that a shaderData is unique for a given light
                    // and won't ever be referenced as a Component either
                    Matrix4x4 *worldTransform = lightEntity->worldTransform();
                    if (worldTransform)
                        shaderData->updateWorldTransform(*worldTransform);

                    setDefaultUniformBlockShaderDataValue(command->m_parameterPack, shader, shaderData, LIGHT_STRUCT_NAMES[lightIdx]);
                    setDefaultUniformBlockShaderDataValue(command->m_parameterPack, shader, shaderData, LIGHT_STRUCT_UNROLL_NAMES[lightIdx]);
                    ++lightIdx;
                }
            }

            if (uniformNamesIds.contains(LIGHT_COUNT_NAME_ID))
                setUniformValue(command->m_parameterPack, LIGHT_COUNT_NAME_ID, UniformValue(qMax((environmentLight ? 0 : 1), lightIdx)));

            // If no active light sources and no environment light, add a default light
            if (activeLightSources.isEmpty() && !environmentLight) {
                // Note: implicit conversion of values to UniformValue
                setUniformValue(command->m_parameterPack, LIGHT_POSITION_NAMES[0], Vector3D(10.0f, 10.0f, 0.0f));
                setUniformValue(command->m_parameterPack, LIGHT_TYPE_NAMES[0], int(QAbstractLight::PointLight));
                setUniformValue(command->m_parameterPack, LIGHT_COLOR_NAMES[0], Vector3D(1.0f, 1.0f, 1.0f));
                setUniformValue(command->m_parameterPack, LIGHT_INTENSITY_NAMES[0], 0.5f);

                setUniformValue(command->m_parameterPack, LIGHT_POSITION_UNROLL_NAMES[0], Vector3D(10.0f, 10.0f, 0.0f));
                setUniformValue(command->m_parameterPack, LIGHT_TYPE_UNROLL_NAMES[0], int(QAbstractLight::PointLight));
                setUniformValue(command->m_parameterPack, LIGHT_COLOR_UNROLL_NAMES[0], Vector3D(1.0f, 1.0f, 1.0f));
                setUniformValue(command->m_parameterPack, LIGHT_INTENSITY_UNROLL_NAMES[0], 0.5f);
            }

            // Environment Light
            int envLightCount = 0;
            if (environmentLight && environmentLight->isEnabled()) {
                ShaderData *shaderData = m_manager->shaderDataManager()->lookupResource(environmentLight->shaderData());
                if (shaderData) {
                    setDefaultUniformBlockShaderDataValue(command->m_parameterPack, shader, shaderData, QStringLiteral("envLight"));
                    envLightCount = 1;
                }
            } else {
                // with some drivers, samplers (like the envbox sampler) need to be bound even though
                // they may not be actually used, otherwise draw calls can fail
                static const int irradianceId = StringToInt::lookupId(QLatin1String("envLight.irradiance"));
                static const int specularId = StringToInt::lookupId(QLatin1String("envLight.specular"));
                setUniformValue(command->m_parameterPack, irradianceId, m_renderer->submissionContext()->maxTextureUnitsCount());
                setUniformValue(command->m_parameterPack, specularId, m_renderer->submissionContext()->maxTextureUnitsCount());
            }
            setUniformValue(command->m_parameterPack, StringToInt::lookupId(QStringLiteral("envLightCount")), envLightCount);
        }
    }
}

bool RenderView::hasBlitFramebufferInfo() const
{
    return m_hasBlitFramebufferInfo;
}

void RenderView::setHasBlitFramebufferInfo(bool hasBlitFramebufferInfo)
{
    m_hasBlitFramebufferInfo = hasBlitFramebufferInfo;
}

BlitFramebufferInfo RenderView::blitFrameBufferInfo() const
{
    return m_blitFrameBufferInfo;
}

void RenderView::setBlitFrameBufferInfo(const BlitFramebufferInfo &blitFrameBufferInfo)
{
    m_blitFrameBufferInfo = blitFrameBufferInfo;
}

bool RenderView::isDownloadBuffersEnable() const
{
    return m_isDownloadBuffersEnable;
}

void RenderView::setIsDownloadBuffersEnable(bool isDownloadBuffersEnable)
{
    m_isDownloadBuffersEnable = isDownloadBuffersEnable;
}

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
