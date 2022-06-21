// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qshaderlanguage_p.h"

#include <QtCore/qcoreapplication.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender
{
// Note: to be invoked explicitly. Relying for example on
// Q_COREAPP_STARTUP_FUNCTION would not be acceptable in static builds.
void qt_register_ShaderLanguage_enums()
{
    qRegisterMetaType<QShaderLanguage::StorageQualifier>();
    qRegisterMetaType<QShaderLanguage::VariableType>();
}
}

QT_END_NAMESPACE

#include "moc_qshaderlanguage_p.cpp"
