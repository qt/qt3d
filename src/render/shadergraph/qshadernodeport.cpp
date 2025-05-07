// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial

#include "qshadernodeport_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DRender
{
QShaderNodePort::QShaderNodePort() noexcept
    : direction(Output)
{
}

bool operator==(const QShaderNodePort &lhs, const QShaderNodePort &rhs) noexcept
{
    return lhs.direction == rhs.direction
        && lhs.name == rhs.name;
}
}

QT_END_NAMESPACE
