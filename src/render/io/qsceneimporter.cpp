// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qsceneimporter_p.h"
#include <Qt3DRender/private/renderlogging_p.h>
#include <QStringList>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

QSceneImporter::QSceneImporter() : QObject(),
    m_status(Empty)
{
}

QSceneImporter::~QSceneImporter()
{
}

QSceneImporter::ParserStatus QSceneImporter::status() const
{
    return m_status;
}

QStringList QSceneImporter::errors() const
{
    return m_errors;
}

void QSceneImporter::setStatus(ParserStatus status)
{
    if (status != m_status) {
        m_status = status;
        emit statusChanged(status);
    }
}

void QSceneImporter::logError(const QString &error)
{
    m_errors.append(error);
    emit errorsChanged(m_errors);
}

void QSceneImporter::logInfo(const QString &info)
{
    qCDebug(Render::Io) << info;
}

} // namespace Qt3DRender

QT_END_NAMESPACE

#include "moc_qsceneimporter_p.cpp"
