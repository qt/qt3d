// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qshadergraph_p.h"

QT_BEGIN_NAMESPACE
namespace Qt3DRender
{

namespace
{
    QList<QShaderNode> copyOutputNodes(const QList<QShaderNode> &nodes, const QList<QShaderGraph::Edge> &edges)
    {
        auto res = QList<QShaderNode>();
        std::copy_if(nodes.cbegin(), nodes.cend(),
                     std::back_inserter(res),
                     [&edges] (const QShaderNode &node) {
                         return node.type() == QShaderNode::Output ||
                                (node.type() == QShaderNode::Function &&
                                 !std::any_of(edges.cbegin(),
                                              edges.cend(),
                                              [&node] (const QShaderGraph::Edge &edge) {
                                                  return edge.sourceNodeUuid ==
                                                         node.uuid();
                                              }));
                     });
        return res;
    }

    QList<QShaderGraph::Edge> incomingEdges(const QList<QShaderGraph::Edge> &edges, const QUuid &uuid)
    {
        auto res = QList<QShaderGraph::Edge>();
        std::copy_if(edges.cbegin(), edges.cend(),
                     std::back_inserter(res),
                     [uuid] (const QShaderGraph::Edge &edge) {
                         return edge.sourceNodeUuid == uuid;
                     });
        return res;
    }

    QList<QShaderGraph::Edge> outgoingEdges(const QList<QShaderGraph::Edge> &edges, const QUuid &uuid)
    {
        auto res = QList<QShaderGraph::Edge>();
        std::copy_if(edges.cbegin(), edges.cend(),
                     std::back_inserter(res),
                     [uuid] (const QShaderGraph::Edge &edge) {
                         return edge.targetNodeUuid == uuid;
                     });
        return res;
    }

    QShaderGraph::Statement nodeToStatement(const QShaderNode &node, int &nextVarId)
    {
        auto statement = QShaderGraph::Statement();
        statement.node = node;

        const QList<QShaderNodePort> ports = node.ports();
        for (const QShaderNodePort &port : ports) {
            if (port.direction == QShaderNodePort::Input) {
                statement.inputs.append(-1);
            } else {
                statement.outputs.append(nextVarId);
                nextVarId++;
            }
        }
        return statement;
    }

    QShaderGraph::Statement completeStatement(const QHash<QUuid, QShaderGraph::Statement> &idHash,
                                              const QList<QShaderGraph::Edge> &edges,
                                              const QUuid &uuid)
    {
        auto targetStatement = idHash.value(uuid);
        for (const QShaderGraph::Edge &edge : edges) {
            if (edge.targetNodeUuid != uuid)
                continue;

            const QShaderGraph::Statement sourceStatement = idHash.value(edge.sourceNodeUuid);
            const int sourcePortIndex = sourceStatement.portIndex(QShaderNodePort::Output, edge.sourcePortName);
            const int targetPortIndex = targetStatement.portIndex(QShaderNodePort::Input, edge.targetPortName);

            if (sourcePortIndex < 0 || targetPortIndex < 0)
                continue;

            const QList<int> sourceOutputs = sourceStatement.outputs;
            QList<int> &targetInputs = targetStatement.inputs;
            targetInputs[targetPortIndex] = sourceOutputs[sourcePortIndex];
        }
        return targetStatement;
    }

    void removeNodesWithUnboundInputs(QList<QShaderGraph::Statement> &statements,
                                      const QList<QShaderGraph::Edge> &allEdges)
    {
        // A node is invalid if any of its input ports is disconected
        // or connected to the output port of another invalid node.

        // Keeps track of the edges from the nodes we know to be valid
        // to unvisited nodes
        auto currentEdges = QList<QShaderGraph::Edge>();

        statements.erase(std::remove_if(statements.begin(),
                                        statements.end(),
                                        [&currentEdges, &allEdges] (const QShaderGraph::Statement &statement) {
            const QShaderNode &node = statement.node;
            const QList<QShaderGraph::Edge> outgoing = outgoingEdges(currentEdges, node.uuid());
            const QList<QShaderNodePort> ports = node.ports();

            bool allInputsConnected = true;
            for (const QShaderNodePort &port : node.ports()) {
                if (port.direction == QShaderNodePort::Output)
                    continue;

                const auto edgeIt = std::find_if(outgoing.cbegin(),
                                                 outgoing.cend(),
                                                 [&port] (const QShaderGraph::Edge &edge) {
                    return edge.targetPortName == port.name;
                });

                if (edgeIt != outgoing.cend())
                    currentEdges.removeAll(*edgeIt);
                else
                    allInputsConnected = false;
            }

            if (allInputsConnected) {
                const QList<QShaderGraph::Edge> incoming = incomingEdges(allEdges, node.uuid());
                currentEdges.append(incoming);
            }

            return !allInputsConnected;
        }),
                         statements.end());
    }
}

QUuid QShaderGraph::Statement::uuid() const noexcept
{
    return node.uuid();
}

int QShaderGraph::Statement::portIndex(QShaderNodePort::Direction direction, const QString &portName) const noexcept
{
    const QList<QShaderNodePort> ports = node.ports();
    int index = 0;
    for (const QShaderNodePort &port : ports) {
        if (port.name == portName && port.direction == direction)
            return index;
        else if (port.direction == direction)
            index++;
    }
    return -1;
}

void QShaderGraph::addNode(const QShaderNode &node)
{
    removeNode(node);
    m_nodes.append(node);
}

void QShaderGraph::removeNode(const QShaderNode &node)
{
    const auto it = std::find_if(m_nodes.begin(), m_nodes.end(),
                                 [node] (const QShaderNode &n) { return n.uuid() == node.uuid(); });
    if (it != m_nodes.end())
        m_nodes.erase(it);
}

QList<QShaderNode> QShaderGraph::nodes() const noexcept
{
    return m_nodes;
}

void QShaderGraph::addEdge(const QShaderGraph::Edge &edge)
{
    if (m_edges.contains(edge))
        return;
    m_edges.append(edge);
}

void QShaderGraph::removeEdge(const QShaderGraph::Edge &edge)
{
    m_edges.removeAll(edge);
}

QList<QShaderGraph::Edge> QShaderGraph::edges() const noexcept
{
    return m_edges;
}

QList<QShaderGraph::Statement> QShaderGraph::createStatements(const QStringList &enabledLayers) const
{
    const auto intersectsEnabledLayers = [enabledLayers] (const QStringList &layers) {
        return layers.isEmpty()
            || std::any_of(layers.cbegin(), layers.cend(),
                           [enabledLayers] (const QString &s) { return enabledLayers.contains(s); });
    };

    const QList<QShaderNode> enabledNodes = [this, intersectsEnabledLayers] {
        auto res = QList<QShaderNode>();
        std::copy_if(m_nodes.cbegin(), m_nodes.cend(),
                     std::back_inserter(res),
                     [intersectsEnabledLayers] (const QShaderNode &node) {
                         return intersectsEnabledLayers(node.layers());
                     });
        return res;
    }();

    const QHash<QUuid, Statement> idHash = [enabledNodes] {
        auto nextVarId = 0;
        auto res = QHash<QUuid, Statement>();
        for (const QShaderNode &node : enabledNodes)
            res.insert(node.uuid(), nodeToStatement(node, nextVarId));
        return res;
    }();

    const QList<Edge> enabledEdges = [this, intersectsEnabledLayers, &idHash] {
        auto res = QList<Edge>();
        std::copy_if(m_edges.cbegin(), m_edges.cend(),
                     std::back_inserter(res),
                     [intersectsEnabledLayers, &idHash] (const Edge &edge) {
                         return intersectsEnabledLayers(edge.layers)
                                 && idHash.contains(edge.sourceNodeUuid)
                                 && idHash.contains(edge.targetNodeUuid);
                     });
        return res;
    }();

    auto result = QList<Statement>();
    QList<Edge> currentEdges = enabledEdges;
    QList<QUuid> currentUuids = [enabledNodes, enabledEdges] {
        const QList<QShaderNode> inputs = copyOutputNodes(enabledNodes, enabledEdges);
        auto res = QList<QUuid>();
        std::transform(inputs.cbegin(), inputs.cend(),
                       std::back_inserter(res),
                       [](const QShaderNode &node) { return node.uuid(); });
        return res;
    }();

    // Implements Kahn's algorithm to flatten the graph
    // https://en.wikipedia.org/wiki/Topological_sorting#Kahn.27s_algorithm
    //
    // We implement it with a small twist though, we follow the edges backward
    // because we want to track the dependencies from the output nodes and not the
    // input nodes
    while (!currentUuids.isEmpty()) {
        const QUuid uuid = currentUuids.takeFirst();
        result.append(completeStatement(idHash, enabledEdges, uuid));

        const QList<QShaderGraph::Edge> outgoing = outgoingEdges(currentEdges, uuid);
        for (const QShaderGraph::Edge &outgoingEdge : outgoing) {
            currentEdges.removeAll(outgoingEdge);
            const QUuid nextUuid = outgoingEdge.sourceNodeUuid;
            const QList<QShaderGraph::Edge> incoming = incomingEdges(currentEdges, nextUuid);
            if (incoming.isEmpty())
                currentUuids.append(nextUuid);
        }
    }

    std::reverse(result.begin(), result.end());

    removeNodesWithUnboundInputs(result, enabledEdges);

    return result;
}

bool operator==(const QShaderGraph::Edge &lhs, const QShaderGraph::Edge &rhs) noexcept
{
    return lhs.sourceNodeUuid == rhs.sourceNodeUuid
        && lhs.sourcePortName == rhs.sourcePortName
        && lhs.targetNodeUuid == rhs.targetNodeUuid
        && lhs.targetPortName == rhs.targetPortName;
}

bool operator==(const QShaderGraph::Statement &lhs, const QShaderGraph::Statement &rhs) noexcept
{
    return lhs.inputs == rhs.inputs
        && lhs.outputs == rhs.outputs
        && lhs.node.uuid() == rhs.node.uuid();
}
}
QT_END_NAMESPACE
