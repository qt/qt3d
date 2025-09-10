// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_QSHADERNODE_P_H
#define QT3DRENDER_QSHADERNODE_P_H

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

#include <Qt3DRender/private/qshaderformat_p.h>
#include <Qt3DRender/private/qshadernodeport_p.h>

#include <QtCore/quuid.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender
{
class QShaderNode
{
public:
    enum Type : char {
        Invalid,
        Input,
        Output,
        Function
    };

    class Rule
    {
    public:
        Q_3DRENDERSHARED_PRIVATE_EXPORT Rule(const QByteArray &substitution = QByteArray(), const QByteArrayList &headerSnippets = QByteArrayList()) noexcept;

        QByteArray substitution;
        QByteArrayList headerSnippets;
    };

    Q_3DRENDERSHARED_PRIVATE_EXPORT Type type() const noexcept;

    Q_3DRENDERSHARED_PRIVATE_EXPORT QUuid uuid() const noexcept;
    Q_3DRENDERSHARED_PRIVATE_EXPORT void setUuid(const QUuid &uuid) noexcept;

    Q_3DRENDERSHARED_PRIVATE_EXPORT QStringList layers() const noexcept;
    Q_3DRENDERSHARED_PRIVATE_EXPORT void setLayers(const QStringList &layers) noexcept;

    Q_3DRENDERSHARED_PRIVATE_EXPORT QList<QShaderNodePort> ports() const noexcept;
    Q_3DRENDERSHARED_PRIVATE_EXPORT void addPort(const QShaderNodePort &port);
    Q_3DRENDERSHARED_PRIVATE_EXPORT void removePort(const QShaderNodePort &port);

    Q_3DRENDERSHARED_PRIVATE_EXPORT QStringList parameterNames() const;
    Q_3DRENDERSHARED_PRIVATE_EXPORT QVariant parameter(const QString &name) const;
    Q_3DRENDERSHARED_PRIVATE_EXPORT void setParameter(const QString &name, const QVariant &value);
    Q_3DRENDERSHARED_PRIVATE_EXPORT void clearParameter(const QString &name);

    Q_3DRENDERSHARED_PRIVATE_EXPORT void addRule(const QShaderFormat &format, const Rule &rule);
    Q_3DRENDERSHARED_PRIVATE_EXPORT void removeRule(const QShaderFormat &format);

    Q_3DRENDERSHARED_PRIVATE_EXPORT QList<QShaderFormat> availableFormats() const;
    Q_3DRENDERSHARED_PRIVATE_EXPORT Rule rule(const QShaderFormat &format) const;

private:
    QUuid m_uuid;
    QStringList m_layers;
    QList<QShaderNodePort> m_ports;
    QHash<QString, QVariant> m_parameters;
    QList<QPair<QShaderFormat, QShaderNode::Rule>> m_rules;
};

Q_3DRENDERSHARED_PRIVATE_EXPORT bool operator==(const QShaderNode::Rule &lhs, const QShaderNode::Rule &rhs) noexcept;

inline bool operator!=(const QShaderNode::Rule &lhs, const QShaderNode::Rule &rhs) noexcept
{
    return !(lhs == rhs);
}

}

Q_DECLARE_TYPEINFO(Qt3DRender::QShaderNode, Q_RELOCATABLE_TYPE);
Q_DECLARE_TYPEINFO(Qt3DRender::QShaderNode::Rule, Q_RELOCATABLE_TYPE);
QT_END_NAMESPACE

Q_DECLARE_METATYPE(Qt3DRender::QShaderNode)
Q_DECLARE_METATYPE(Qt3DRender::QShaderNode::Rule)

#endif // QT3DRENDER_QSHADERNODE_P_H
