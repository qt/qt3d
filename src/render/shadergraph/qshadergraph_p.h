// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_QSHADERGRAPH_P_H
#define QT3DRENDER_QSHADERGRAPH_P_H

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

#include <Qt3DRender/private/qshadernode_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender
{
class QShaderGraph
{
public:
    class Edge
    {
    public:
        QStringList layers;
        QUuid sourceNodeUuid;
        QString sourcePortName;
        QUuid targetNodeUuid;
        QString targetPortName;
    };

    class Statement
    {
    public:
        Q_3DRENDERSHARED_PRIVATE_EXPORT QUuid uuid() const noexcept;
        Q_3DRENDERSHARED_PRIVATE_EXPORT int portIndex(QShaderNodePort::Direction direction, const QString &portName) const noexcept;

        QShaderNode node;
        QList<int> inputs;
        QList<int> outputs;
    };

    Q_3DRENDERSHARED_PRIVATE_EXPORT void addNode(const QShaderNode &node);
    Q_3DRENDERSHARED_PRIVATE_EXPORT void removeNode(const QShaderNode &node);
    Q_3DRENDERSHARED_PRIVATE_EXPORT QList<QShaderNode> nodes() const noexcept;

    Q_3DRENDERSHARED_PRIVATE_EXPORT void addEdge(const Edge &edge);
    Q_3DRENDERSHARED_PRIVATE_EXPORT void removeEdge(const Edge &edge);
    Q_3DRENDERSHARED_PRIVATE_EXPORT QList<Edge> edges() const noexcept;

    Q_3DRENDERSHARED_PRIVATE_EXPORT QList<Statement> createStatements(const QStringList &enabledLayers = QStringList()) const;

private:
    QList<QShaderNode> m_nodes;
    QList<Edge> m_edges;
};

Q_3DRENDERSHARED_PRIVATE_EXPORT bool operator==(const QShaderGraph::Edge &lhs, const QShaderGraph::Edge &rhs) noexcept;

inline bool operator!=(const QShaderGraph::Edge &lhs, const QShaderGraph::Edge &rhs) noexcept
{
    return !(lhs == rhs);
}

Q_3DRENDERSHARED_PRIVATE_EXPORT bool operator==(const QShaderGraph::Statement &lhs, const QShaderGraph::Statement &rhs) noexcept;

inline bool operator!=(const QShaderGraph::Statement &lhs, const QShaderGraph::Statement &rhs) noexcept
{
    return !(lhs == rhs);
}

}

Q_DECLARE_TYPEINFO(Qt3DRender::QShaderGraph, Q_RELOCATABLE_TYPE);
Q_DECLARE_TYPEINFO(Qt3DRender::QShaderGraph::Edge, Q_RELOCATABLE_TYPE);
Q_DECLARE_TYPEINFO(Qt3DRender::QShaderGraph::Statement, Q_RELOCATABLE_TYPE);
QT_END_NAMESPACE

Q_DECLARE_METATYPE(Qt3DRender::QShaderGraph)
Q_DECLARE_METATYPE(Qt3DRender::QShaderGraph::Edge)
Q_DECLARE_METATYPE(Qt3DRender::QShaderGraph::Statement)

#endif // QT3DRENDER_QSHADERGRAPH_P_H
