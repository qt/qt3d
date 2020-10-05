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
#include <Qt3DRender/private/effect_p.h>
#include <Qt3DRender/private/entity_p.h>
#include <Qt3DRender/private/nodemanagers_p.h>
#include <Qt3DRender/private/layer_p.h>
#include <Qt3DRender/private/renderlogging_p.h>
#include <Qt3DRender/private/renderpassfilternode_p.h>
#include <Qt3DRender/private/renderpass_p.h>
#include <Qt3DRender/private/geometryrenderer_p.h>
#include <Qt3DRender/private/techniquefilternode_p.h>
#include <Qt3DRender/private/viewportnode_p.h>
#include <Qt3DRender/private/buffermanager_p.h>
#include <Qt3DRender/private/geometryrenderermanager_p.h>
#include <Qt3DRender/private/rendercapture_p.h>
#include <Qt3DRender/private/buffercapture_p.h>
#include <Qt3DRender/private/stringtoint_p.h>
#include <Qt3DRender/private/renderlogging_p.h>
#include <Qt3DRender/private/renderstateset_p.h>
#include <rendercommand_p.h>
#include <renderer_p.h>
#include <graphicscontext_p.h>
#include <submissioncontext_p.h>
#include <glresourcemanagers_p.h>
#include <Qt3DCore/qentity.h>
#include <QtGui/qsurface.h>
#include <algorithm>
#include <atomic>
#include <gllights_p.h>
#include <QDebug>
#if defined(QT3D_RENDER_VIEW_JOB_TIMINGS)
#include <QElapsedTimer>
#endif

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {
namespace OpenGL {

namespace  {

// register our QNodeId's as a metatype during program loading
const int Q_DECL_UNUSED qNodeIdTypeId = qMetaTypeId<Qt3DCore::QNodeId>();

std::atomic_bool wasInitialized{};

} // anonymous namespace

RenderView::StandardUniformsNameToTypeHash RenderView::ms_standardUniformSetters;


RenderView::StandardUniformsNameToTypeHash RenderView::initializeStandardUniformSetters()
{
    RenderView::StandardUniformsNameToTypeHash setters;

    setters.insert(Shader::modelMatrixNameId, ModelMatrix);
    setters.insert(Shader::viewMatrixNameId, ViewMatrix);
    setters.insert(Shader::projectionMatrixNameId, ProjectionMatrix);
    setters.insert(Shader::modelViewMatrixNameId, ModelViewMatrix);
    setters.insert(Shader::viewProjectionMatrixNameId, ViewProjectionMatrix);
    setters.insert(Shader::modelViewProjectionNameId, ModelViewProjectionMatrix);
    setters.insert(Shader::mvpNameId, ModelViewProjectionMatrix);
    setters.insert(Shader::inverseModelMatrixNameId, InverseModelMatrix);
    setters.insert(Shader::inverseViewMatrixNameId, InverseViewMatrix);
    setters.insert(Shader::inverseProjectionMatrixNameId, InverseProjectionMatrix);
    setters.insert(Shader::inverseModelViewNameId, InverseModelViewMatrix);
    setters.insert(Shader::inverseViewProjectionMatrixNameId, InverseViewProjectionMatrix);
    setters.insert(Shader::inverseModelViewProjectionNameId, InverseModelViewProjectionMatrix);
    setters.insert(Shader::modelNormalMatrixNameId, ModelNormalMatrix);
    setters.insert(Shader::modelViewNormalNameId, ModelViewNormalMatrix);
    setters.insert(Shader::viewportMatrixNameId, ViewportMatrix);
    setters.insert(Shader::inverseViewportMatrixNameId, InverseViewportMatrix);
    setters.insert(Shader::aspectRatioNameId, AspectRatio);
    setters.insert(Shader::exposureNameId, Exposure);
    setters.insert(Shader::gammaNameId, Gamma);
    setters.insert(Shader::timeNameId, Time);
    setters.insert(Shader::eyePositionNameId, EyePosition);
    setters.insert(Shader::skinningPaletteNameId, SkinningPalette);

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
    return lens ? lens->projection() : Matrix4x4();
}

UniformValue RenderView::standardUniformValue(RenderView::StandardUniform standardUniformType,
                                              const Entity *entity) const
{
    const Matrix4x4 &model = *(entity->worldTransform());

    switch (standardUniformType) {
    case ModelMatrix:
        return UniformValue(model);
    case ViewMatrix:
        return UniformValue(m_viewMatrix);
    case ProjectionMatrix:
        return UniformValue(getProjectionMatrix(m_renderCameraLens));
    case ModelViewMatrix:
        return UniformValue(m_viewMatrix * model);
    case ViewProjectionMatrix:
        return UniformValue(getProjectionMatrix(m_renderCameraLens) * m_viewMatrix);
    case ModelViewProjectionMatrix:
        return UniformValue(m_viewProjectionMatrix * model);
    case InverseModelMatrix:
        return UniformValue(model.inverted());
    case InverseViewMatrix:
        return UniformValue(m_viewMatrix.inverted());
    case InverseProjectionMatrix: {
        return UniformValue(getProjectionMatrix(m_renderCameraLens).inverted());
    }
    case InverseModelViewMatrix:
        return UniformValue((m_viewMatrix * model).inverted());
    case InverseViewProjectionMatrix: {
        const Matrix4x4 viewProjectionMatrix = getProjectionMatrix(m_renderCameraLens) * m_viewMatrix;
        return UniformValue(viewProjectionMatrix.inverted());
    }
    case InverseModelViewProjectionMatrix:
        return UniformValue((m_viewProjectionMatrix * model).inverted());
    case ModelNormalMatrix:
        return UniformValue(convertToQMatrix4x4(model).normalMatrix());
    case ModelViewNormalMatrix:
        return UniformValue(convertToQMatrix4x4(m_viewMatrix * model).normalMatrix());
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
        return float(m_surfaceSize.width()) / std::max(1.f, float(m_surfaceSize.height()));
    case Exposure:
        return UniformValue(m_renderCameraLens ? m_renderCameraLens->exposure() : 0.0f);
    case Gamma:
        return UniformValue(m_gamma);
    case Time:
        return UniformValue(float(m_renderer->time() / 1000000000.0f));
    case EyePosition:
        return UniformValue(m_eyePos);
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
{
    if (Q_UNLIKELY(!wasInitialized.exchange(true))) {
        // Needed as we can control the init order of static/global variables across compile units
        // and this hash relies on the static StringToInt class

        RenderView::ms_standardUniformSetters = RenderView::initializeStandardUniformSetters();
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
        return qFuzzyCompare(a.m_depth, b.m_depth);
    }
};

template<>
struct AdjacentSubRangeFinder<QSortPolicy::Material>
{
    static bool adjacentSubRange(const RenderCommand &a, const RenderCommand &b)
    {
        return a.m_glShader == b.m_glShader;
    }
};

template<>
struct AdjacentSubRangeFinder<QSortPolicy::FrontToBack>
{
    static bool adjacentSubRange(const RenderCommand &a, const RenderCommand &b)
    {
        return qFuzzyCompare(a.m_depth, b.m_depth);
    }
};

template<>
struct AdjacentSubRangeFinder<QSortPolicy::Texture>
{
    static bool adjacentSubRange(const RenderCommand &a, const RenderCommand &b)
    {
        // Two renderCommands are adjacent if one contains all the other command's textures
        const std::vector<ShaderParameterPack::NamedResource> &texturesA = a.m_parameterPack.textures();
        const std::vector<ShaderParameterPack::NamedResource> &texturesB = b.m_parameterPack.textures();

        const bool bBigger = texturesB.size() > texturesA.size();
        const std::vector<ShaderParameterPack::NamedResource> &smallestVector = bBigger ? texturesA : texturesB;
        const std::vector<ShaderParameterPack::NamedResource> &biggestVector = bBigger ? texturesB : texturesA;

        const auto e = biggestVector.cend();
        for (const ShaderParameterPack::NamedResource &tex : smallestVector) {
            if (std::find(biggestVector.begin(), e, tex) == e)
                return false;
        }

        return true;
    }
};

template<typename Predicate>
int advanceUntilNonAdjacent(const EntityRenderCommandDataView *view,
                            const size_t beg, const size_t end, Predicate pred)
{
    const std::vector<size_t> &commandIndices = view->indices;
    const std::vector<RenderCommand> &commands = view->data.commands;
    size_t i = beg + 1;
    if (i < end) {
        const size_t startIdx = commandIndices[beg];
        while (i < end) {
            const size_t targetIdx = commandIndices[i];
            if (!pred(commands[startIdx], commands[targetIdx]))
                break;
            ++i;
        }
    }
    return i;
}


template<int SortType>
struct SubRangeSorter
{
    static void sortSubRange(EntityRenderCommandDataView *view, size_t begin, const size_t end)
    {
        Q_UNUSED(view)
        Q_UNUSED(begin)
        Q_UNUSED(end)
        Q_UNREACHABLE();
    }
};

template<>
struct SubRangeSorter<QSortPolicy::StateChangeCost>
{
    static void sortSubRange(EntityRenderCommandDataView *view, size_t begin, const size_t end)
    {
        std::vector<size_t> &commandIndices = view->indices;
        const std::vector<RenderCommand> &commands = view->data.commands;
        std::stable_sort(commandIndices.begin() + begin, commandIndices.begin() + end,
                         [&commands] (const size_t &iA, const size_t &iB) {
            const RenderCommand &a = commands[iA];
            const RenderCommand &b = commands[iB];
            return a.m_changeCost > b.m_changeCost;
        });
    }
};

template<>
struct SubRangeSorter<QSortPolicy::BackToFront>
{
    static void sortSubRange(EntityRenderCommandDataView *view, size_t begin, const size_t end)
    {
        std::vector<size_t> &commandIndices = view->indices;
        const std::vector<RenderCommand> &commands = view->data.commands;
        std::stable_sort(commandIndices.begin() + begin, commandIndices.begin() + end,
                         [&commands] (const size_t &iA, const size_t &iB) {
            const RenderCommand &a = commands[iA];
            const RenderCommand &b = commands[iB];
            return a.m_depth > b.m_depth;
        });
    }
};

template<>
struct SubRangeSorter<QSortPolicy::Material>
{
    static void sortSubRange(EntityRenderCommandDataView *view, size_t begin, const size_t end)
    {
        std::vector<size_t> &commandIndices = view->indices;
        const std::vector<RenderCommand> &commands = view->data.commands;
        std::stable_sort(commandIndices.begin() + begin, commandIndices.begin() + end,
                         [&commands] (const size_t &iA, const size_t &iB) {
            const RenderCommand &a = commands[iA];
            const RenderCommand &b = commands[iB];
            return a.m_glShader > b.m_glShader;
        });
    }
};

template<>
struct SubRangeSorter<QSortPolicy::FrontToBack>
{
    static void sortSubRange(EntityRenderCommandDataView *view, size_t begin, const size_t end)
    {
        std::vector<size_t> &commandIndices = view->indices;
        const std::vector<RenderCommand> &commands = view->data.commands;
        std::stable_sort(commandIndices.begin() + begin, commandIndices.begin() + end,
                         [&commands] (const size_t &iA, const size_t &iB) {
            const RenderCommand &a = commands[iA];
            const RenderCommand &b = commands[iB];
            return a.m_depth < b.m_depth;
        });
    }
};

template<>
struct SubRangeSorter<QSortPolicy::Texture>
{
    static void sortSubRange(EntityRenderCommandDataView *view, size_t begin, const size_t end)
    {
#ifndef Q_OS_WIN
        std::vector<size_t> &commandIndices = view->indices;
        const std::vector<RenderCommand> &commands = view->data.commands;
        std::stable_sort(commandIndices.begin() + begin, commandIndices.begin() + end,
                         [&commands] (const int &iA, const int &iB) {
            const RenderCommand &a = commands[iA];
            const RenderCommand &b = commands[iB];
            const std::vector<ShaderParameterPack::NamedResource> &texturesA = a.m_parameterPack.textures();
            const std::vector<ShaderParameterPack::NamedResource> &texturesB = b.m_parameterPack.textures();

            const bool bBigger = texturesB.size() > texturesA.size();
            const std::vector<ShaderParameterPack::NamedResource> &smallestVector = bBigger ? texturesA : texturesB;
            const std::vector<ShaderParameterPack::NamedResource> &biggestVector = bBigger ? texturesB : texturesA;

            int identicalTextureCount = 0;
            const auto e = biggestVector.cend();
            for (const ShaderParameterPack::NamedResource &tex : smallestVector) {
                if (std::find(biggestVector.begin(), e, tex) != e)
                    ++identicalTextureCount;
            }

            return identicalTextureCount < smallestVector.size();
        });
#endif
    }
};

int findSubRange(const EntityRenderCommandDataView *view,
                 const int begin, const int end,
                 const QSortPolicy::SortType sortType)
{
    switch (sortType) {
    case QSortPolicy::StateChangeCost:
        return advanceUntilNonAdjacent(view, begin, end, AdjacentSubRangeFinder<QSortPolicy::StateChangeCost>::adjacentSubRange);
    case QSortPolicy::BackToFront:
        return advanceUntilNonAdjacent(view, begin, end, AdjacentSubRangeFinder<QSortPolicy::BackToFront>::adjacentSubRange);
    case QSortPolicy::Material:
        return advanceUntilNonAdjacent(view, begin, end, AdjacentSubRangeFinder<QSortPolicy::Material>::adjacentSubRange);
    case QSortPolicy::FrontToBack:
        return advanceUntilNonAdjacent(view, begin, end, AdjacentSubRangeFinder<QSortPolicy::FrontToBack>::adjacentSubRange);
    case QSortPolicy::Texture:
        return advanceUntilNonAdjacent(view, begin, end, AdjacentSubRangeFinder<QSortPolicy::Texture>::adjacentSubRange);
    case QSortPolicy::Uniform:
        return end;
    default:
        Q_UNREACHABLE();
        return end;
    }
}

void sortByMaterial(EntityRenderCommandDataView *view, int begin, const int end)
{
    // We try to arrange elements so that their rendering cost is minimized for a given shader
    std::vector<size_t> &commandIndices = view->indices;
    const std::vector<RenderCommand> &commands = view->data.commands;
    int rangeEnd = advanceUntilNonAdjacent(view, begin, end, AdjacentSubRangeFinder<QSortPolicy::Material>::adjacentSubRange);
    while (begin != end) {
        if (begin + 1 < rangeEnd) {
            std::stable_sort(commandIndices.begin() + begin + 1, commandIndices.begin() + rangeEnd,
                             [&commands] (const int &iA, const int &iB) {
                const RenderCommand &a = commands[iA];
                const RenderCommand &b = commands[iB];
                return a.m_material.handle() < b.m_material.handle();
            });
        }
        begin = rangeEnd;
        rangeEnd = advanceUntilNonAdjacent(view, begin, end, AdjacentSubRangeFinder<QSortPolicy::Material>::adjacentSubRange);
    }
}

void sortCommandRange(EntityRenderCommandDataView *view, int begin, int end, const int level,
                      const QVector<Qt3DRender::QSortPolicy::SortType> &sortingTypes)
{
    if (level >= sortingTypes.size())
        return;

    switch (sortingTypes.at(level)) {
    case QSortPolicy::StateChangeCost:
        SubRangeSorter<QSortPolicy::StateChangeCost>::sortSubRange(view, begin, end);
        break;
    case QSortPolicy::BackToFront:
        SubRangeSorter<QSortPolicy::BackToFront>::sortSubRange(view, begin, end);
        break;
    case QSortPolicy::Material:
        // Groups all same shader DNA together
        SubRangeSorter<QSortPolicy::Material>::sortSubRange(view, begin, end);
        // Group all same material together (same parameters most likely)
        sortByMaterial(view, begin, end);
        break;
    case QSortPolicy::FrontToBack:
        SubRangeSorter<QSortPolicy::FrontToBack>::sortSubRange(view, begin, end);
        break;
    case QSortPolicy::Texture:
        SubRangeSorter<QSortPolicy::Texture>::sortSubRange(view, begin, end);
        break;
    case QSortPolicy::Uniform:
        break;
    default:
        Q_UNREACHABLE();
    }

    // For all sub ranges of adjacent item for sortType[i]
    // Perform filtering with sortType[i + 1]
    int rangeEnd = findSubRange(view, begin, end, sortingTypes.at(level));
    while (begin != end) {
        sortCommandRange(view, begin, rangeEnd, level + 1, sortingTypes);
        begin = rangeEnd;
        rangeEnd = findSubRange(view, begin, end, sortingTypes.at(level));
    }
}

} // anonymous

void RenderView::sort()
{
    assert(m_renderCommandDataView);
    // Compares the bitsetKey of the RenderCommands
    // Key[Depth | StateCost | Shader]
    sortCommandRange(m_renderCommandDataView.data(), 0, m_renderCommandDataView->size(), 0, m_sortingTypes);

    // For RenderCommand with the same shader
    // We compute the adjacent change cost

     // Only perform uniform minimization if we explicitly asked for it
     if (!m_sortingTypes.contains(QSortPolicy::Uniform))
        return;

    // Minimize uniform changes
    int i = 0;
    std::vector<RenderCommand> &commands = m_renderCommandDataView->data.commands;
    const std::vector<size_t> &indices = m_renderCommandDataView->indices;
    const size_t commandSize = indices.size();

    while (i < commandSize) {
        size_t j = i;

        // Advance while commands share the same shader
        while (i < commandSize &&
               commands[indices[j]].m_glShader == commands[indices[i]].m_glShader)
            ++i;

        if (i - j > 0) { // Several commands have the same shader, so we minimize uniform changes
            PackUniformHash cachedUniforms = commands[indices[j++]].m_parameterPack.uniforms();

            while (j < i) {
                // We need the reference here as we are modifying the original container
                // not the copy
                PackUniformHash &uniforms = commands[indices[j]].m_parameterPack.m_uniforms;

                for (size_t u = 0; u < uniforms.keys.size();) {
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

RenderStateSet *RenderView::getOrCreateStateSet()
{
    if (!m_stateSet)
        m_stateSet.reset(new RenderStateSet());
    return m_stateSet.data();
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
EntityRenderCommandData RenderView::buildDrawRenderCommands(const QVector<Entity *> &entities,
                                                            int offset, int count) const
{
    GLShaderManager *glShaderManager = m_renderer->glResourceManagers()->glShaderManager();
    EntityRenderCommandData commands;

    commands.reserve(count);

    for (int i = 0; i < count; ++i) {
        const int idx = offset + i;
        Entity *entity = entities.at(idx);
        GeometryRenderer *geometryRenderer = nullptr;
        HGeometryRenderer geometryRendererHandle = entity->componentHandle<GeometryRenderer>();

        // There is a geometry renderer with geometry
        if ((geometryRenderer = m_manager->geometryRendererManager()->data(geometryRendererHandle)) != nullptr
                && geometryRenderer->isEnabled()
                && !geometryRenderer->geometryId().isNull()) {

            const Qt3DCore::QNodeId materialComponentId = entity->componentUuid<Material>();
            const HMaterial materialHandle = entity->componentHandle<Material>();
            const  QVector<RenderPassParameterData> renderPassData = m_parameters.value(materialComponentId);

            HGeometry geometryHandle = m_manager->geometryManager()->lookupHandle(geometryRenderer->geometryId());
            Geometry *geometry = m_manager->geometryManager()->data(geometryHandle);

            // 1 RenderCommand per RenderPass pass on an Entity with a Mesh
            for (const RenderPassParameterData &passData : renderPassData) {
                // Add the RenderPass Parameters
                RenderCommand command = {};
                command.m_geometryRenderer = geometryRendererHandle;
                command.m_geometry = geometryHandle;

                command.m_material = materialHandle;
                // For RenderPass based states we use the globally set RenderState
                // if no renderstates are defined as part of the pass. That means:
                // RenderPass { renderStates: [] } will use the states defined by
                // StateSet in the FrameGraph
                RenderPass *pass = passData.pass;
                if (pass->hasRenderStates()) {
                    command.m_stateSet = RenderStateSetPtr::create();
                    addStatesToRenderStateSet(command.m_stateSet.data(), pass->renderStates(), m_manager->renderStateManager());
                    if (m_stateSet)
                        command.m_stateSet->merge(m_stateSet.data());
                    command.m_changeCost = m_renderer->defaultRenderState()->changeCost(command.m_stateSet.data());
                }
                command.m_shaderId = pass->shaderProgram();
                command.m_glShader = glShaderManager->lookupResource(command.m_shaderId);

                // It takes two frames to have a valid command as we can only
                // reference a glShader at frame n if it has been loaded at frame n - 1
                if (!command.m_glShader)
                    continue;

                { // Scoped to show extent

                    // Update the draw command with what's going to be needed for the drawing
                    int primitiveCount = geometryRenderer->vertexCount();
                    int estimatedCount = 0;
                    Attribute *indexAttribute = nullptr;
                    Attribute *indirectAttribute = nullptr;

                    const QVector<Qt3DCore::QNodeId> attributeIds = geometry->attributes();
                    for (Qt3DCore::QNodeId attributeId : attributeIds) {
                        Attribute *attribute = m_manager->attributeManager()->lookupResource(attributeId);
                        switch (attribute->attributeType()) {
                        case QAttribute::IndexAttribute:
                            indexAttribute = attribute;
                            break;
                        case QAttribute::DrawIndirectAttribute:
                            indirectAttribute = attribute;
                            break;
                        case QAttribute::VertexAttribute:
                            estimatedCount = std::max(int(attribute->count()), estimatedCount);
                            break;
                        default:
                            Q_UNREACHABLE();
                            break;
                        }
                    }

                    command.m_drawIndexed = (indexAttribute != nullptr);
                    command.m_drawIndirect = (indirectAttribute != nullptr);

                    // Update the draw command with all the information required for the drawing
                    if (command.m_drawIndexed) {
                        command.m_indexAttributeDataType = GraphicsContext::glDataTypeFromAttributeDataType(indexAttribute->vertexBaseType());
                        command.m_indexAttributeByteOffset = indexAttribute->byteOffset() + geometryRenderer->indexBufferByteOffset();
                    }

                    // Note: we only care about the primitiveCount when using direct draw calls
                    // For indirect draw calls it is assumed the buffer was properly set already
                    if (command.m_drawIndirect) {
                        command.m_indirectAttributeByteOffset = indirectAttribute->byteOffset();
                        command.m_indirectDrawBuffer = m_manager->bufferManager()->lookupHandle(indirectAttribute->bufferId());
                    } else {
                        // Use the count specified by the GeometryRender
                        // If not specify use the indexAttribute count if present
                        // Otherwise tries to use the count from the attribute with the highest count
                        if (primitiveCount == 0) {
                            if (indexAttribute)
                                primitiveCount = indexAttribute->count();
                            else
                                primitiveCount = estimatedCount;
                        }
                    }

                    command.m_primitiveCount = primitiveCount;
                    command.m_primitiveType = geometryRenderer->primitiveType();
                    command.m_primitiveRestartEnabled = geometryRenderer->primitiveRestartEnabled();
                    command.m_restartIndexValue = geometryRenderer->restartIndexValue();
                    command.m_firstInstance = geometryRenderer->firstInstance();
                    command.m_instanceCount = geometryRenderer->instanceCount();
                    command.m_firstVertex = geometryRenderer->firstVertex();
                    command.m_indexOffset = geometryRenderer->indexOffset();
                    command.m_verticesPerPatch = geometryRenderer->verticesPerPatch();
                } // scope


                commands.push_back(entity,
                                   std::move(command),
                                   std::move(passData));
            }
        }
    }

    return commands;
}

EntityRenderCommandData RenderView::buildComputeRenderCommands(const QVector<Entity *> &entities,
                                                               int offset, int count) const
{
    // If the RenderView contains only a ComputeDispatch then it cares about
    // A ComputeDispatch is also implicitely a NoDraw operation
    // enabled flag
    // layer component
    // material/effect/technique/parameters/filters/
    EntityRenderCommandData commands;
    GLShaderManager *glShaderManager = m_renderer->glResourceManagers()->glShaderManager();

    commands.reserve(count);

    for (int i = 0; i < count; ++i) {
        const int idx = offset + i;
        Entity *entity = entities.at(idx);
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
                        command.m_stateSet->merge(m_stateSet.data());
                    command.m_changeCost = m_renderer->defaultRenderState()->changeCost(command.m_stateSet.data());
                }
                command.m_shaderId = pass->shaderProgram();
                command.m_glShader = glShaderManager->lookupResource(command.m_shaderId);

                // It takes two frames to have a valid command as we can only
                // reference a glShader at frame n if it has been loaded at frame n - 1
                if (!command.m_glShader)
                    continue;

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

void RenderView::updateRenderCommand(const EntityRenderCommandDataSubView &subView)
{
    // Note: since many threads can be building render commands
    // we need to ensure that the UniformBlockValueBuilder they are using
    // is only accessed from the same thread
    UniformBlockValueBuilder *builder = new UniformBlockValueBuilder();
    builder->shaderDataManager = m_manager->shaderDataManager();
    builder->textureManager = m_manager->textureManager();
    m_localData.setLocalData(builder);

    subView.forEach([this] (const Entity *entity,
                            const RenderPassParameterData &passData,
                            RenderCommand &command) {
        if (command.m_type == RenderCommand::Draw) {
            // Project the camera-to-object-center vector onto the camera
            // view vector. This gives a depth value suitable as the key
            // for BackToFront sorting.
            command.m_depth = Vector3D::dotProduct(entity->worldBoundingVolume()->center() - m_eyePos, m_eyeViewDir);
        } else { // Compute
            // Note: if frameCount has reached 0 in the previous frame, isEnabled
            // would be false
            ComputeCommand *computeJob = m_manager->computeJobManager()->data(command.m_computeCommand);
            if (computeJob->runType() == QComputeCommand::Manual)
                computeJob->updateFrameCount();
        }

        // setShaderAndUniforms can initialize a localData
        // make sure this is cleared before we leave this function
        setShaderAndUniforms(&command,
                             passData.parameterInfo,
                             entity);
    });

    // We reset the local data once we are done with it
    m_localData.setLocalData(nullptr);
}

void RenderView::updateMatrices()
{
    if (m_renderCameraNode && m_renderCameraLens && m_renderCameraLens->isEnabled()) {
        const Matrix4x4 cameraWorld = *(m_renderCameraNode->worldTransform());
        setViewMatrix(m_renderCameraLens->viewMatrix(cameraWorld));

        setViewProjectionMatrix(m_renderCameraLens->projection() * viewMatrix());
        //To get the eyePosition of the camera, we need to use the inverse of the
        //camera's worldTransform matrix.
        const Matrix4x4 inverseWorldTransform = viewMatrix().inverted();
        const Vector3D eyePosition(inverseWorldTransform.column(3));
        setEyePosition(eyePosition);

        // Get the viewing direction of the camera. Use the normal matrix to
        // ensure non-uniform scale works too.
        const QMatrix3x3 normalMat = convertToQMatrix4x4(m_viewMatrix).normalMatrix();
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
                                         int nameId,
                                         const Entity *entity) const
{
    uniformPack.setUniform(nameId, standardUniformValue(ms_standardUniformSetters[nameId], entity));
}

void RenderView::setUniformBlockValue(ShaderParameterPack &uniformPack,
                                      const ShaderUniformBlock &block,
                                      const UniformValue &value) const
{
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
                                       const ShaderStorageBlock &block,
                                       const UniformValue &value) const
{
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

void RenderView::setDefaultUniformBlockShaderDataValue(ShaderParameterPack &uniformPack,
                                                       const GLShader *shader,
                                                       const ShaderData *shaderData,
                                                       const QString &structName) const
{
    UniformBlockValueBuilder *builder = m_localData.localData();
    builder->activeUniformNamesToValue.clear();

    // Set the view matrix to be used to transform "Transformed" properties in the ShaderData
    builder->viewMatrix = m_viewMatrix;
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

void RenderView::applyParameter(const Parameter *param,
                                RenderCommand *command,
                                const GLShader *shader) const noexcept
{
    const int nameId = param->nameId();
    const UniformValue &uniformValue = param->uniformValue();
    const GLShader::ParameterKind kind = shader->categorizeVariable(nameId);

    switch (kind) {
    case GLShader::Uniform: {
        setUniformValue(command->m_parameterPack, nameId, uniformValue);
        break;
    }
    case GLShader::UBO: {
        setUniformBlockValue(command->m_parameterPack, shader->uniformBlockForBlockNameId(nameId), uniformValue);
        break;
    }
    case GLShader::SSBO: {
        setShaderStorageValue(command->m_parameterPack, shader->storageBlockForBlockNameId(nameId), uniformValue);
        break;
    }
    case GLShader::Struct: {
        ShaderData *shaderData = nullptr;
        if (uniformValue.valueType() == UniformValue::NodeId &&
                (shaderData = m_manager->shaderDataManager()->lookupResource(*uniformValue.constData<Qt3DCore::QNodeId>())) != nullptr) {
            // Try to check if we have a struct or array matching a QShaderData parameter
            setDefaultUniformBlockShaderDataValue(command->m_parameterPack, shader, shaderData, StringToInt::lookupString(nameId));
        }
        break;
    }
    default:
        break;
    }
}


void RenderView::setShaderAndUniforms(RenderCommand *command,
                                      const ParameterInfoList &parameters,
                                      const Entity *entity) const
{
    // The VAO Handle is set directly in the renderer thread so as to avoid having to use a mutex here
    // Set shader, technique, and effect by basically doing :
    // ShaderProgramManager[MaterialManager[frontentEntity->id()]->Effect->Techniques[TechniqueFilter->name]->RenderPasses[RenderPassFilter->name]];
    // The Renderer knows that if one of those is null, a default material / technique / effect as to be used

    // Find all RenderPasses (in order) matching values set in the RenderPassFilter
    // Get list of parameters for the Material, Effect, and Technique
    // For each ParameterBinder in the RenderPass -> create a QUniformPack
    // Once that works, improve that to try and minimize QUniformPack updates

    GLShader *shader = command->m_glShader;
    if (shader == nullptr || !shader->isLoaded())
        return;

    // If we have already build the uniforms previously, we should
    // only update values of uniforms that have changed
    // If parameters add been added/removed, the command would have been rebuild
    // and the parameter pack would be empty
    const bool updateUniformsOnly = command->m_parameterPack.submissionUniformIndices().size() > 0;

    if (!updateUniformsOnly) {
        // Builds the QUniformPack, sets shader standard uniforms and store attributes name / glname bindings
        // If a parameter is defined and not found in the bindings it is assumed to be a binding of Uniform type with the glsl name
        // equals to the parameter name

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

        // Set default attributes
        command->m_activeAttributes = shader->attributeNamesIds();

        // At this point we know whether the command is a valid draw command or not
        // We still need to process the uniforms as the command could be a compute command
        command->m_isValid = !command->m_activeAttributes.empty();

        // Reserve amount of uniforms we are going to need
        command->m_parameterPack.reserve(shader->parameterPackSize());
    }

    if (shader->hasActiveVariables()) {
        const QVector<int> &standardUniformNamesIds = shader->standardUniformNameIds();

        // It only makes sense to update the standard uniforms if:
        // - Camera changed
        // - Entity transform changed
        // - Viewport/Surface changed

        for (const int uniformNameId : standardUniformNamesIds)
            setStandardUniformValue(command->m_parameterPack, uniformNameId, entity);

        ParameterInfoList::const_iterator it = parameters.cbegin();
        const ParameterInfoList::const_iterator parametersEnd = parameters.cend();

        while (it != parametersEnd) {
            const Parameter *param = m_manager->data<Parameter, ParameterManager>(it->handle);
            applyParameter(param, command, shader);
            ++it;
        }

        // Lights
        updateLightUniforms(command, entity);
    }

    // Prepare the ShaderParameterPack based on the active uniforms of the shader
    if (!updateUniformsOnly)
        shader->prepareUniforms(command->m_parameterPack);
}

void RenderView::updateLightUniforms(RenderCommand *command, const Entity *entity) const
{
    GLShader *shader = command->m_glShader;
    const QVector<int> &lightUniformNamesIds = shader->lightUniformsNamesIds();
    if (!lightUniformNamesIds.empty()) {
        // Pick which lights to take in to account.
        // For now decide based on the distance by taking the MAX_LIGHTS closest lights.
        // Replace with more sophisticated mechanisms later.
        // Copy vector so that we can sort it concurrently and we only want to sort the one for the current command
        QVector<LightSource> lightSources = m_lightSources;

        if (lightSources.size() > 1) {
            const Vector3D entityCenter = entity->worldBoundingVolume()->center();
            std::sort(lightSources.begin(), lightSources.end(),
                      [&] (const LightSource &a, const LightSource &b) {
                const float distA = entityCenter.distanceToPoint(a.entity->worldBoundingVolume()->center());
                const float distB = entityCenter.distanceToPoint(b.entity->worldBoundingVolume()->center());
                return distA < distB;
            });
        }
        m_lightSources = lightSources.mid(0, std::min(lightSources.size(), MAX_LIGHTS));

        int lightIdx = 0;
        for (const LightSource &lightSource : qAsConst(m_lightSources)) {
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
                if (lightUniformNamesIds.contains(GLLights::LIGHT_TYPE_NAMES[lightIdx])) {
                    setUniformValue(command->m_parameterPack, GLLights::LIGHT_POSITION_NAMES[lightIdx], worldPos);
                    setUniformValue(command->m_parameterPack, GLLights::LIGHT_TYPE_NAMES[lightIdx], int(QAbstractLight::PointLight));
                    setUniformValue(command->m_parameterPack, GLLights::LIGHT_COLOR_NAMES[lightIdx], Vector3D(1.0f, 1.0f, 1.0f));
                    setUniformValue(command->m_parameterPack, GLLights::LIGHT_INTENSITY_NAMES[lightIdx], 0.5f);
                } else if (lightUniformNamesIds.contains(GLLights::LIGHT_TYPE_UNROLL_NAMES[lightIdx])) {
                    setUniformValue(command->m_parameterPack, GLLights::LIGHT_POSITION_UNROLL_NAMES[lightIdx], worldPos);
                    setUniformValue(command->m_parameterPack, GLLights::LIGHT_TYPE_UNROLL_NAMES[lightIdx], int(QAbstractLight::PointLight));
                    setUniformValue(command->m_parameterPack, GLLights::LIGHT_COLOR_UNROLL_NAMES[lightIdx], Vector3D(1.0f, 1.0f, 1.0f));
                    setUniformValue(command->m_parameterPack, GLLights::LIGHT_INTENSITY_UNROLL_NAMES[lightIdx], 0.5f);
                }

                // There is no risk in doing that even if multithreaded
                // since we are sure that a shaderData is unique for a given light
                // and won't ever be referenced as a Component either
                Matrix4x4 *worldTransform = lightEntity->worldTransform();
                if (worldTransform)
                    shaderData->updateWorldTransform(*worldTransform);

                setDefaultUniformBlockShaderDataValue(command->m_parameterPack, shader, shaderData, GLLights::LIGHT_STRUCT_NAMES[lightIdx]);
                setDefaultUniformBlockShaderDataValue(command->m_parameterPack, shader, shaderData, GLLights::LIGHT_STRUCT_UNROLL_NAMES[lightIdx]);
                ++lightIdx;
            }
        }

        setUniformValue(command->m_parameterPack, GLLights::LIGHT_COUNT_NAME_ID, UniformValue(qMax((m_environmentLight ? 0 : 1), lightIdx)));

        // If no active light sources and no environment light, add a default light
        if (m_lightSources.isEmpty() && !m_environmentLight) {
            // Note: implicit conversion of values to UniformValue
            if (lightUniformNamesIds.contains(GLLights::LIGHT_TYPE_NAMES[0])) {
                setUniformValue(command->m_parameterPack, GLLights::LIGHT_POSITION_NAMES[0], Vector3D(10.0f, 10.0f, 0.0f));
                setUniformValue(command->m_parameterPack, GLLights::LIGHT_TYPE_NAMES[0], int(QAbstractLight::PointLight));
                setUniformValue(command->m_parameterPack, GLLights::LIGHT_COLOR_NAMES[0], Vector3D(1.0f, 1.0f, 1.0f));
                setUniformValue(command->m_parameterPack, GLLights::LIGHT_INTENSITY_NAMES[0], 0.5f);
            } else if (lightUniformNamesIds.contains(GLLights::LIGHT_TYPE_UNROLL_NAMES[lightIdx])) {
                setUniformValue(command->m_parameterPack, GLLights::LIGHT_POSITION_UNROLL_NAMES[0], Vector3D(10.0f, 10.0f, 0.0f));
                setUniformValue(command->m_parameterPack, GLLights::LIGHT_TYPE_UNROLL_NAMES[0], int(QAbstractLight::PointLight));
                setUniformValue(command->m_parameterPack, GLLights::LIGHT_COLOR_UNROLL_NAMES[0], Vector3D(1.0f, 1.0f, 1.0f));
                setUniformValue(command->m_parameterPack, GLLights::LIGHT_INTENSITY_UNROLL_NAMES[0], 0.5f);
            }
        }
    }

    // Environment Light
    int envLightCount = 0;
    static const int irradianceStructId = StringToInt::lookupId(QLatin1String("envLight.irradiance"));
    static const int specularStructId = StringToInt::lookupId(QLatin1String("envLight.specular"));
    static const int irradianceId = StringToInt::lookupId(QLatin1String("envLightIrradiance"));
    static const int specularId = StringToInt::lookupId(QLatin1String("envLightSpecular"));
    if (m_environmentLight && m_environmentLight->isEnabled()) {
        ShaderData *shaderData = m_manager->shaderDataManager()->lookupResource(m_environmentLight->shaderData());
        if (shaderData) {
            setDefaultUniformBlockShaderDataValue(command->m_parameterPack, shader, shaderData, QStringLiteral("envLight"));
            auto irr =
                    shaderData->properties()["irradiance"].value.value<Qt3DCore::QNodeId>();
            auto spec =
                    shaderData->properties()["specular"].value.value<Qt3DCore::QNodeId>();
            setUniformValue(command->m_parameterPack, irradianceId, irr);
            setUniformValue(command->m_parameterPack, specularId, spec);
            envLightCount = 1;
        }
    } else {
        // with some drivers, samplers (like the envbox sampler) need to be bound even though
        // they may not be actually used, otherwise draw calls can fail
        setUniformValue(command->m_parameterPack, irradianceId, m_renderer->submissionContext()->maxTextureUnitsCount());
        setUniformValue(command->m_parameterPack, irradianceStructId, m_renderer->submissionContext()->maxTextureUnitsCount());
        setUniformValue(command->m_parameterPack, specularId, m_renderer->submissionContext()->maxTextureUnitsCount());
        setUniformValue(command->m_parameterPack, specularStructId, m_renderer->submissionContext()->maxTextureUnitsCount());
    }
    setUniformValue(command->m_parameterPack, StringToInt::lookupId(QStringLiteral("envLightCount")), envLightCount);
}

bool RenderView::hasBlitFramebufferInfo() const
{
    return m_hasBlitFramebufferInfo;
}

void RenderView::setHasBlitFramebufferInfo(bool hasBlitFramebufferInfo)
{
    m_hasBlitFramebufferInfo = hasBlitFramebufferInfo;
}

bool RenderView::shouldSkipSubmission() const
{
    if (commandCount() > 0)
        return false;

    if (m_hasBlitFramebufferInfo)
        return false;

    if (m_isDownloadBuffersEnable)
        return false;

    if (m_showDebugOverlay)
        return false;

    if (!m_waitFences.empty() || !m_insertFenceIds.empty())
        return false;

    if (m_clearBuffer != QClearBuffers::None)
        return false;

    if (!m_renderCaptureNodeId.isNull())
        return false;

    return true;
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

} // namespace OpenGL
} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
