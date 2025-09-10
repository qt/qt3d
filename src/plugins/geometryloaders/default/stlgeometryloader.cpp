// Copyright (C) 2017 The Qt Company Ltd and/or its subsidiary(-ies).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "stlgeometryloader.h"

#include <QtCore/QDataStream>
#include <QtCore/QLoggingCategory>
#include <QtCore/QIODevice>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

Q_LOGGING_CATEGORY(StlGeometryLoaderLog, "Qt3D.StlGeometryLoader", QtWarningMsg)

bool StlGeometryLoader::doLoad(QIODevice *ioDev, const QString &subMesh)
{
    Q_UNUSED(subMesh);

    if (loadBinary(ioDev))
        return true;

    return loadAscii(ioDev);
}

bool StlGeometryLoader::loadAscii(QIODevice *ioDev)
{
    // TODO stricter syntax checking

    ioDev->setTextModeEnabled(true);
    if (!ioDev->seek(0))
        return false;

    char signature[5];
    if (ioDev->peek(signature, sizeof(signature)) != sizeof(signature))
        return false;

    if (qstrncmp(signature, "solid", 5) != 0)
        return false;

    while (!ioDev->atEnd()) {
        QByteArray lineBuffer = ioDev->readLine();

        const char *begin = lineBuffer.constData();
        const char *end = begin + lineBuffer.size();

        const ByteArraySplitter tokens(begin, end, ' ', Qt::SkipEmptyParts);

        if (qstrncmp(tokens.charPtrAt(0), "vertex ", 7) == 0) {
            if (tokens.size() < 4) {
                qCWarning(StlGeometryLoaderLog) << "Unsupported number of components in vertex";
            } else {
                const float x = tokens.floatAt(1);
                const float y = tokens.floatAt(2);
                const float z = tokens.floatAt(3);
                m_points.push_back(QVector3D(x, y, z));
                m_indices.push_back(uint(m_indices.size()));
            }
        }
    }

    return true;
}

bool StlGeometryLoader::loadBinary(QIODevice *ioDev)
{
    static const int headerSize = 80;

    if (ioDev->read(headerSize).size() != headerSize)
        return false;

    ioDev->setTextModeEnabled(false);

    QDataStream stream(ioDev);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream.setFloatingPointPrecision(QDataStream::SinglePrecision);

    quint32 triangleCount;
    stream >> triangleCount;

    if (quint64(ioDev->size()) != headerSize + sizeof(quint32) + (triangleCount * 50))
        return false;

    m_points.reserve(triangleCount * 3);
    m_indices.reserve(triangleCount * 3);

    for (unsigned i = 0; i < triangleCount; ++i) {
        QVector3D normal;
        stream >> normal;

        for (int j = 0; j < 3; ++j) {
            QVector3D point;
            stream >> point;
            m_points.push_back(point);
            m_indices.push_back((i * 3) + j);
        }

        quint16 attributeCount;
        stream >> attributeCount;
    }

    return true;
}

} // namespace Qt3DRender

QT_END_NAMESPACE
