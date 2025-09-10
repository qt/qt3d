// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qshadernode_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DRender
{
QShaderNode::Type QShaderNode::type() const noexcept
{
    int inputCount = 0;
    int outputCount = 0;
    for (const auto &port : std::as_const(m_ports)) {
        switch (port.direction) {
        case QShaderNodePort::Input:
            inputCount++;
            break;
        case QShaderNodePort::Output:
            outputCount++;
            break;
        }
    }

    return (inputCount == 0 && outputCount == 0) ? Invalid
         : (inputCount > 0 && outputCount == 0) ? Output
         : (inputCount == 0 && outputCount > 0) ? Input
         : Function;
}

QUuid QShaderNode::uuid() const noexcept
{
    return m_uuid;
}

void QShaderNode::setUuid(const QUuid &uuid) noexcept
{
    m_uuid = uuid;
}

QStringList QShaderNode::layers() const noexcept
{
    return m_layers;
}

void QShaderNode::setLayers(const QStringList &layers) noexcept
{
    m_layers = layers;
}

QList<QShaderNodePort> QShaderNode::ports() const noexcept
{
    return m_ports;
}

void QShaderNode::addPort(const QShaderNodePort &port)
{
    removePort(port);
    m_ports.append(port);
}

void QShaderNode::removePort(const QShaderNodePort &port)
{
    const auto it = std::find_if(m_ports.begin(), m_ports.end(),
                                 [port](const QShaderNodePort &p) {
                                    return p.name == port.name;
                                 });
    if (it != m_ports.end())
        m_ports.erase(it);
}

QStringList QShaderNode::parameterNames() const
{
    return m_parameters.keys();
}

QVariant QShaderNode::parameter(const QString &name) const
{
    return m_parameters.value(name);
}

void QShaderNode::setParameter(const QString &name, const QVariant &value)
{
    m_parameters.insert(name, value);
}

void QShaderNode::clearParameter(const QString &name)
{
    m_parameters.remove(name);
}

void QShaderNode::addRule(const QShaderFormat &format, const QShaderNode::Rule &rule)
{
    removeRule(format);
    m_rules << qMakePair(format, rule);
}

void QShaderNode::removeRule(const QShaderFormat &format)
{
    const auto it = std::find_if(m_rules.begin(), m_rules.end(),
                                 [format](const QPair<QShaderFormat, Rule> &entry) {
                                     return entry.first == format;
                                 });
    if (it != m_rules.end())
        m_rules.erase(it);
}

QList<QShaderFormat> QShaderNode::availableFormats() const
{
    auto res = QList<QShaderFormat>();
    std::transform(m_rules.cbegin(), m_rules.cend(),
                   std::back_inserter(res),
                   [](const QPair<QShaderFormat, Rule> &entry) { return entry.first; });
    return res;
}

QShaderNode::Rule QShaderNode::rule(const QShaderFormat &format) const
{
    const QPair<QShaderFormat, Rule> *selected = nullptr;
    for (auto it = m_rules.crbegin(); it != m_rules.crend(); ++it) {
        const auto &entry = *it;
        if (format.supports(entry.first)) {
            if (!selected || entry.first.version() > selected->first.version())
                selected = &entry;
        }
    }
    return selected ? selected->second : Rule();
}

QShaderNode::Rule::Rule(const QByteArray &subs, const QByteArrayList &snippets) noexcept
    : substitution(subs),
      headerSnippets(snippets)
{
}

bool operator==(const QShaderNode::Rule &lhs, const QShaderNode::Rule &rhs) noexcept
{
    return lhs.substitution == rhs.substitution
        && lhs.headerSnippets == rhs.headerSnippets;
}
}
QT_END_NAMESPACE
