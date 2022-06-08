// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_RENDERVIEWJOBUTILS_P_H
#define QT3DRENDER_RENDER_RENDERVIEWJOBUTILS_P_H

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

#include <Qt3DRender/qt3drender_global.h>
#include <Qt3DRender/private/qt3drender_global_p.h>
#include <Qt3DCore/qnodeid.h>
#include <QtCore/qhash.h>
#include <QtCore/qvariant.h>
#include <Qt3DRender/private/uniform_p.h>
#include <Qt3DRender/private/handle_types_p.h>
#include <Qt3DRender/private/shaderdata_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {

class FrameGraphNode;
class ParameterManager;
class Effect;
class Entity;
class Material;
class RenderPass;
class Technique;
class TechniqueFilter;
class RenderPassFilter;
class NodeManagers;
class ShaderDataManager;
class TextureManager;
class RenderStateManager;
class RenderStateCollection;
class RenderStateSet;

Q_3DRENDERSHARED_PRIVATE_EXPORT Technique *findTechniqueForEffect(NodeManagers *manager,
                                                                  const TechniqueFilter *techniqueFilter,
                                                                  Effect *effect);

typedef QVarLengthArray<RenderPass*, 4> RenderPassList;
Q_3DRENDERSHARED_PRIVATE_EXPORT RenderPassList findRenderPassesForTechnique(NodeManagers *manager,
                                                                            const RenderPassFilter *passFilter,
                                                                            Technique *technique);

// Extracts the type T from a QVariant v without using QVariant::value which is slow
// Note: Assumes you are 100% sure about the type you requested
template<typename T>
inline T variant_value(const QVariant &v)
{
    return *reinterpret_cast<const T *>(v.data());
}

struct ParameterInfo
{
    explicit ParameterInfo(const int nameId = -1, const HParameter &handle = HParameter());

    int nameId;
    HParameter handle;

    bool operator<(const int otherNameId) const noexcept;
    bool operator<(const ParameterInfo &other) const noexcept;
};
using ParameterInfoList = QList<ParameterInfo>;

struct RenderPassParameterData
{
    RenderPass *pass;
    ParameterInfoList parameterInfo;
};
QT3D_DECLARE_TYPEINFO_2(Qt3DRender, Render, RenderPassParameterData, Q_RELOCATABLE_TYPE)

using MaterialParameterGathererData = QMultiHash<Qt3DCore::QNodeId, std::vector<RenderPassParameterData>>;

Q_3DRENDERSHARED_PRIVATE_EXPORT void parametersFromMaterialEffectTechnique(ParameterInfoList *infoList,
                                             ParameterManager *manager,
                                             Material *material,
                                             Effect *effect,
                                             Technique *technique);

Q_3DRENDERSHARED_PRIVATE_EXPORT void addParametersForIds(ParameterInfoList *params, ParameterManager *manager,
                                                         const QList<Qt3DCore::QNodeId> &parameterIds);

template<class T>
void parametersFromParametersProvider(ParameterInfoList *infoList,
                                      ParameterManager *manager,
                                      T *provider)
{
    addParametersForIds(infoList, manager, provider->parameters());
}

Q_3DRENDERSHARED_PRIVATE_EXPORT ParameterInfoList::const_iterator findParamInfo(ParameterInfoList *infoList,
                                                                                const int nameId);

Q_3DRENDERSHARED_PRIVATE_EXPORT void addStatesToRenderStateSet(RenderStateSet *stateSet,
                                                               const QList<Qt3DCore::QNodeId> stateIds,
                                                               RenderStateManager *manager);

Q_3DRENDERSHARED_PRIVATE_EXPORT int findIdealNumberOfWorkers(int elementCount, int packetSize = 100, int maxJobCount = 1)
;

Q_3DRENDERSHARED_PRIVATE_EXPORT std::vector<Entity *> entitiesInSubset(const std::vector<Entity *> &entities,
const std::vector<Entity *> &subset);

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_OPENGL_RENDERVIEWJOBUTILS_P_H
