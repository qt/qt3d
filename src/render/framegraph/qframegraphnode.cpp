// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qframegraphnode.h"
#include "qframegraphnode_p.h"
#include <Qt3DRender/qfilterkey.h>
#include <Qt3DRender/qtechniquefilter.h>
#include <Qt3DRender/qrenderpassfilter.h>

#include <Qt3DCore/QNode>
#include <QList>
#include <QQueue>


QT_BEGIN_NAMESPACE

namespace {

QString dumpNode(const Qt3DRender::QFrameGraphNode *n) {
    QString res = QLatin1String(n->metaObject()->className());
    if (!n->objectName().isEmpty())
        res += QString(QLatin1String(" (%1)")).arg(n->objectName());
    if (!n->isEnabled())
        res += QLatin1String(" [D]");
    return res;
}

QString dumpNodeFilters(const Qt3DRender::QFrameGraphNode *n, const QList<Qt3DRender::QFilterKey *> &filters) {
    QString res = QLatin1String(n->metaObject()->className());
    if (!n->objectName().isEmpty())
        res += QString(QLatin1String(" (%1)")).arg(n->objectName());

    QStringList kv;
    for (auto filter: filters)
        kv.push_back(QString(QLatin1String("%1: %2")).arg(filter->name(), filter->value().toString()));
    if (!kv.empty())
        res += QString(QLatin1String(" <%1>")).arg(kv.join(QLatin1String(", ")));

    return res;
}

QStringList dumpFG(const Qt3DCore::QNode *n, int level = 0)
{
    QStringList reply;

    const Qt3DRender::QFrameGraphNode *fgNode = qobject_cast<const Qt3DRender::QFrameGraphNode *>(n);
    if (fgNode) {
        QString res = dumpNode(fgNode);
        reply += res.rightJustified(res.size() + level * 2, QLatin1Char(' '));
    }

    const auto children = n->childNodes();
    const int inc = fgNode ? 1 : 0;
    for (auto *child: children) {
        auto *childFGNode = qobject_cast<Qt3DCore::QNode *>(child);
        if (childFGNode != nullptr)
            reply += dumpFG(childFGNode, level + inc);
    }

    return reply;
}

struct HierarchyFGNode
{
    const Qt3DRender::QFrameGraphNode *root;
    QList<QSharedPointer<HierarchyFGNode>> children;
};
using HierarchyFGNodePtr = QSharedPointer<HierarchyFGNode>;

HierarchyFGNodePtr buildFGHierarchy(const Qt3DCore::QNode *n, HierarchyFGNodePtr lastFGParent = HierarchyFGNodePtr())
{
    const Qt3DRender::QFrameGraphNode *fgNode = qobject_cast<const Qt3DRender::QFrameGraphNode *>(n);

    // Only happens for the root case
    if (!lastFGParent) {
        lastFGParent = HierarchyFGNodePtr::create();
        lastFGParent->root = fgNode;
    } else {
        if (fgNode != nullptr) {
            HierarchyFGNodePtr hN = HierarchyFGNodePtr::create();
            hN->root = fgNode;
            if (lastFGParent)
                lastFGParent->children.push_back(hN);
            lastFGParent = hN;
        }
    }

    const auto children = n->childNodes();
    for (auto *child: children)
        buildFGHierarchy(child, lastFGParent);

    return lastFGParent;
}

void findFGLeaves(const HierarchyFGNodePtr root, QList<const Qt3DRender::QFrameGraphNode *> &fgLeaves)
{
    const auto children = root->children;
    for (const auto &child : children)
        findFGLeaves(child, fgLeaves);

    if (children.empty())
        fgLeaves.push_back(root->root);
}

void dumpFGPaths(const Qt3DRender::QFrameGraphNode *n, QStringList &result)
{
    // Build FG node hierarchy
    const HierarchyFGNodePtr rootHFg = buildFGHierarchy(n);

    // Gather FG leaves
    QList<const Qt3DRender::QFrameGraphNode *> fgLeaves;
    findFGLeaves(rootHFg, fgLeaves);

    // Traverse back to root
    int rv = 1;
    for (const Qt3DRender::QFrameGraphNode *fgNode : fgLeaves) {
        QStringList parents;
        while (fgNode != nullptr) {
            parents.prepend(dumpNode(fgNode));
            fgNode = fgNode->parentFrameGraphNode();
        }
        if (!parents.empty()) {
            result << QString(QLatin1String("%1 [ %2 ]")).arg(QString::number(rv), parents.join(QLatin1String(", ")));
            ++rv;
        }
    }
}

void dumpFGFilterState(const Qt3DRender::QFrameGraphNode *n, QStringList &result)
{
    // Build FG node hierarchy
    const HierarchyFGNodePtr rootHFg = buildFGHierarchy(n);

    // Gather FG leaves
    QList<const Qt3DRender::QFrameGraphNode *> fgLeaves;
    findFGLeaves(rootHFg, fgLeaves);

    // Traverse back to root
    int rv = 1;
    for (const Qt3DRender::QFrameGraphNode *fgNode : fgLeaves) {
        int parents = 0;
        QStringList filters;
        while (fgNode != nullptr) {
            ++parents;
            if (fgNode->isEnabled()) {
                auto techniqueFilter = qobject_cast<const Qt3DRender::QTechniqueFilter *>(fgNode);
                if (techniqueFilter && techniqueFilter->matchAll().size())
                    filters.prepend(dumpNodeFilters(techniqueFilter, techniqueFilter->matchAll()));
                auto renderPassFilter = qobject_cast<const Qt3DRender::QRenderPassFilter *>(fgNode);
                if (renderPassFilter)
                    filters.prepend(dumpNodeFilters(renderPassFilter, renderPassFilter->matchAny()));
            }
            fgNode = fgNode->parentFrameGraphNode();
        }
        if (parents) {
            if (!filters.empty())
                result << QString(QLatin1String("%1 [ %2 ]")).arg(QString::number(rv), filters.join(QLatin1String(", ")));
            else
                result << QString(QObject::tr("%1 [ No Filters ]")).arg(rv);
            ++rv;
        }
    }
}

}

namespace Qt3DRender {

QFrameGraphNodePrivate::QFrameGraphNodePrivate()
    : QNodePrivate()
{
}

/*!
    \class Qt3DRender::QFrameGraphNode
    \inmodule Qt3DRender
    \since 5.5

    \brief Base class of all FrameGraph configuration nodes.

    This class is rarely instanced directly since it doesn't provide
    any frame graph specific behavior, although it can be convenient
    to use for grouping other nodes together in dynamic frame graphs.
    The actual behavior comes from the subclasses.

    The subclasses are:
    \table
    \header
        \li class
        \li description
    \row
        \li Qt3DRender::QCameraSelector
        \li Select camera from all available cameras in the scene
    \row
        \li Qt3DRender::QClearBuffers
        \li Specify which buffers to clear and to what values
    \row
        \li Qt3DRender::QDispatchCompute
        \li Specify Compute operation kernels
    \row
        \li Qt3DRender::QFrustumCulling
        \li Enable frustum culling
    \row
        \li Qt3DRender::QLayerFilter
        \li Select which layers to draw
    \row
        \li Qt3DRender::QNoDraw
        \li Disable drawing
    \row
        \li Qt3DRender::QRenderPassFilter
        \li Select which render passes to draw
    \row
        \li Qt3DRender::QRenderStateSet
        \li Set render states
    \row
        \li Qt3DRender::QRenderSurfaceSelector
        \li Select which surface to draw to
    \row
        \li Qt3DRender::QRenderTargetSelector
        \li Select which QRenderTarget to draw to
    \row
        \li Qt3DRender::QSortPolicy
        \li Specify how entities are sorted to determine draw order
    \row
        \li Qt3DRender::QTechniqueFilter
        \li Select which techniques to draw
    \row
        \li Qt3DRender::QViewport
        \li Specify viewport
    \row
        \li Qt3DRender::QMemoryBarrier
        \li Places a memory barrier
    \endtable

 */

/*!
    \qmltype FrameGraphNode
    \inqmlmodule Qt3D.Render
    \nativetype Qt3DRender::QFrameGraphNode
    \inherits Node
    \since 5.5
    \brief Base class of all FrameGraph configuration nodes.

    This class is rarely instanced directly since it doesn't provide
    any frame graph specific behavior, although it can be convenient
    to use for grouping other nodes together in dynamic frame graphs.
    The actual behavior comes from the subclasses.

    The subclasses are:
    \table
    \header
        \li class
        \li description
    \row
        \li CameraSelector
        \li Select camera from all available cameras in the scene
    \row
        \li ClearBuffers
        \li Specify which buffers to clear and to what values
    \row
        \li DispatchCompute
        \li Specify compute operation kernels
    \row
        \li FrustumCulling
        \li Enable frustum culling
    \row
        \li LayerFilter
        \li Select which layers to draw
    \row
        \li NoDraw
        \li Disable drawing
    \row
        \li RenderPassFilter
        \li Select which render passes to draw
    \row
        \li RenderStateSet
        \li Set render states
    \row
        \li RenderSurfaceSelector
        \li Select which surface to draw to
    \row
        \li RenderTargetSelector
        \li Select which RenderTarget to draw to
    \row
        \li SortPolicy
        \li Specify how entities are sorted to determine draw order
    \row
        \li TechniqueFilter
        \li Select which techniques to draw
    \row
        \li Viewport
        \li Specify viewport
    \row
        \li MemoryBarrier
        \li Places a memory barrier
    \endtable
*/

/*!
    The constructor creates an instance with the specified \a parent.
 */
QFrameGraphNode::QFrameGraphNode(QNode *parent)
    : QNode(*new QFrameGraphNodePrivate, parent)
{
}

/*! \internal */
QFrameGraphNode::~QFrameGraphNode()
{
}

/*!
    Returns a pointer to the parent frame graph node.

    If the parent of this node is not a frame graph node,
    this method will recursively look for a parent node that is a frame graph node.
 */
QFrameGraphNode *QFrameGraphNode::parentFrameGraphNode() const
{
    QFrameGraphNode *parentFGNode = nullptr;
    QNode *parentN = parentNode();

    while (parentN) {
        if ((parentFGNode = qobject_cast<QFrameGraphNode *>(parentN)) != nullptr)
            break;
        parentN = parentN->parentNode();
    }
    return parentFGNode;
}

/*!
    \internal
 * Returns a list of the children that are frame graph nodes.
 * If this function encounters a child node that is not a frame graph node,
 * it will go through the children of the child node and look for frame graph nodes.
 * If any of these are not frame graph nodes, they will be further searched as
 * if they were direct children of this node.
 */
QList<QFrameGraphNode *> QFrameGraphNodePrivate::childFrameGraphNodes() const
{
    using namespace Qt3DCore;

    Q_Q(const QFrameGraphNode);
    QList<QFrameGraphNode *> result;
    QQueue<QNode *> queue;
    const auto childNodes = q->childNodes();
    for (auto c: childNodes)
        queue.append(c);
    result.reserve(queue.size());
    while (!queue.isEmpty()) {
        auto *child = queue.dequeue();
        auto *childFGNode = qobject_cast<QFrameGraphNode *>(child);
        if (childFGNode != nullptr)
            result.push_back(childFGNode);
        else {
            const auto childNodes = child->childNodes();
            for (auto c: childNodes)
                queue.append(c);
        }
    }
    return result;
}

QString QFrameGraphNodePrivate::dumpFrameGraph() const
{
    Q_Q(const QFrameGraphNode);
    return dumpFG(q).join(QLatin1Char('\n'));
}

QStringList QFrameGraphNodePrivate::dumpFrameGraphPaths() const
{
    Q_Q(const QFrameGraphNode);
    QStringList result;
    dumpFGPaths(q, result);
    return result;
}

QStringList QFrameGraphNodePrivate::dumpFrameGraphFilterState() const
{
    Q_Q(const QFrameGraphNode);
    QStringList result;
    dumpFGFilterState(q, result);
    return result;
}

/*! \internal */
QFrameGraphNode::QFrameGraphNode(QFrameGraphNodePrivate &dd, QNode *parent)
    : QNode(dd, parent)
{
}

void QFrameGraphNode::onParentChanged(QObject *)
{
    // Direct sync update request
    Q_D(QFrameGraphNode);
    d->update();
}

} // namespace Qt3DRender

QT_END_NAMESPACE

#include "moc_qframegraphnode.cpp"
