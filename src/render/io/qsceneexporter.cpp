// Copyright (C) 2016 The Qt Company Ltd and/or its subsidiary(-ies).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qsceneexporter_p.h"

#include <private/renderlogging_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

QSceneExporter::QSceneExporter() : QObject()
{
}

QSceneExporter::~QSceneExporter()
{
}

QStringList QSceneExporter::errors() const
{
    return m_errors;
}

void QSceneExporter::logError(const QString &error)
{
    m_errors.append(error);
    emit errorsChanged(m_errors);
}

void QSceneExporter::logInfo(const QString &info)
{
    qCDebug(Render::Io) << info;
}

} // namespace Qt3DRender

QT_END_NAMESPACE

#include "moc_qsceneexporter_p.cpp"
