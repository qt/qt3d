/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QT3D_RENDERVIEWJOBUTILS_P_H
#define QT3D_RENDERVIEWJOBUTILS_P_H

#include <Qt3DRenderer/qt3drenderer_global.h>

#include <QtCore/qhash.h>
#include <QtCore/qvariant.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

class QFrameAllocator;

namespace Render {

class FrameGraphNode;
class ParameterManager;
class RenderEffect;
class RenderEntity;
class RenderMaterial;
class RenderRenderPass;
class RenderStateSet;
class RenderTechnique;
class RenderView;
class Renderer;

Q_AUTOTEST_EXPORT void setRenderViewConfigFromFrameGraphLeafNode(RenderView *rv,
                                                                 const FrameGraphNode *fgLeaf);

Q_AUTOTEST_EXPORT void preprocessRenderTree(RenderView *rv, const RenderEntity *node);

Q_AUTOTEST_EXPORT RenderTechnique *findTechniqueForEffect(Renderer *renderer,
                                                          RenderView *renderView,
                                                          RenderEffect *effect);

typedef QVarLengthArray<RenderRenderPass*, 4> RenderRenderPassList;
Q_AUTOTEST_EXPORT RenderRenderPassList findRenderPassesForTechnique(Renderer *renderer,
                                                                    RenderView *renderView,
                                                                    RenderTechnique *technique);

struct ParameterInfo
{
    ParameterInfo(const QString &name = QString(), const QVariant &value = QVariant())
      : name(name)
      , value(value)
    {}

    QString name;
    QVariant value;

    bool operator<(const QString &otherName) const
    {
        return name < otherName;
    }
};

typedef QVarLengthArray<ParameterInfo, 16> ParameterInfoList;

Q_AUTOTEST_EXPORT void parametersFromMaterialEffectTechnique(ParameterInfoList *infoList,
                                                             ParameterManager *manager,
                                                             RenderMaterial *material,
                                                             RenderEffect *effect,
                                                             RenderTechnique *technique);

Q_AUTOTEST_EXPORT ParameterInfoList::iterator findParamInfo(ParameterInfoList *infoList,
                                                            const QString &name);

Q_AUTOTEST_EXPORT RenderStateSet *buildRenderStateSet(RenderRenderPass *pass,
                                                      QFrameAllocator *allocator);

} // namespace Render
} // namespace Qt3D

Q_DECLARE_TYPEINFO(Qt3D::Render::ParameterInfo, Q_MOVABLE_TYPE);

QT_END_NAMESPACE

#endif // QT3D_RENDERVIEWJOBUTILS_P_H
