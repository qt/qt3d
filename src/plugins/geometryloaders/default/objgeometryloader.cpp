// Copyright (C) 2017 The Qt Company Ltd and/or its subsidiary(-ies).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "objgeometryloader.h"

#include <QtCore/QHash>
#include <QtCore/QLoggingCategory>
#include <QtCore/QRegularExpression>
#include <QtCore/QIODevice>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

Q_LOGGING_CATEGORY(ObjGeometryLoaderLog, "Qt3D.ObjGeometryLoader", QtWarningMsg)

static void addFaceVertex(const FaceIndices &faceIndices,
                          QList<FaceIndices> &faceIndexVector,
                          QHash<FaceIndices, unsigned int> &faceIndexMap);

inline uint qHash(const FaceIndices &faceIndices)
{
    return faceIndices.positionIndex
            + 10 * faceIndices.texCoordIndex
            + 100 * faceIndices.normalIndex;
}

bool ObjGeometryLoader::doLoad(QIODevice *ioDev, const QString &subMesh)
{
    // Parse faces taking into account each vertex in a face can index different indices
    // for the positions, normals and texture coords;
    // Generate unique vertices (in OpenGL parlance) and output to points, texCoords,
    // normals and calculate mapping from faces to unique indices
    QList<QVector3D> positions;
    QList<QVector3D> normals;
    QList<QVector2D> texCoords;
    QHash<FaceIndices, unsigned int> faceIndexMap;
    QList<FaceIndices> faceIndexVector;

    bool skipping = false;
    int positionsOffset = 0;
    int normalsOffset = 0;
    int texCoordsOffset = 0;

    QRegularExpression subMeshMatch(subMesh);
    if (!subMeshMatch.isValid())
        subMeshMatch.setPattern(QLatin1String("^(") + subMesh + QLatin1String(")$"));
    Q_ASSERT(subMeshMatch.isValid());

    char lineBuffer[1024];
    const char *line;
    QByteArray longLine;
    while (!ioDev->atEnd()) {
        // try to read into lineBuffer first, if the line fits (common case) we can do this without expensive allocations
        // if not, fall back to dynamically allocated QByteArrays
        auto lineSize = ioDev->readLine(lineBuffer, sizeof(lineBuffer));
        if (lineSize == sizeof(lineBuffer) - 1 && lineBuffer[lineSize - 1] != '\n') {
            longLine = QByteArray(lineBuffer, lineSize);
            longLine += ioDev->readLine();
            line = longLine.constData();
            lineSize = longLine.size();
        } else {
            line = lineBuffer;
        }

        if (lineSize > 0 && line[0] != '#') {
            if (line[lineSize - 1] == '\n')
                --lineSize; // chop newline
            if (lineSize <= 0)
                continue;

            if (line[lineSize - 1] == '\r')
                --lineSize; // chop newline also for CRLF format
            if (lineSize <= 0)
                continue;

            while (lineSize > 0 && (line[lineSize - 1] == ' ' || line[lineSize - 1] == '\t')) {
                --lineSize; // chop trailing spaces
            }
            if (lineSize <= 0)
                continue;

            const ByteArraySplitter tokens(line, line + lineSize, ' ', Qt::SkipEmptyParts);

            if (qstrncmp(tokens.charPtrAt(0), "v ", 2) == 0) {
                if (tokens.size() < 4) {
                    qCWarning(ObjGeometryLoaderLog) << "Unsupported number of components in vertex";
                } else {
                    if (!skipping) {
                        const float x = tokens.floatAt(1);
                        const float y = tokens.floatAt(2);
                        const float z = tokens.floatAt(3);
                        positions.append(QVector3D(x, y, z));
                    } else {
                        positionsOffset++;
                    }
                }
            } else if (m_loadTextureCoords && qstrncmp(tokens.charPtrAt(0), "vt ", 3) == 0) {
                if (tokens.size() < 3) {
                    qCWarning(ObjGeometryLoaderLog) << "Unsupported number of components in texture coordinate";
                } else {
                    if (!skipping) {
                        // Process texture coordinate
                        const float s = tokens.floatAt(1);
                        const float t = tokens.floatAt(2);
                        texCoords.append(QVector2D(s, t));
                    } else {
                        ++texCoordsOffset;
                    }
                }
            } else if (qstrncmp(tokens.charPtrAt(0), "vn ", 3) == 0) {
                if (tokens.size() < 4) {
                    qCWarning(ObjGeometryLoaderLog) << "Unsupported number of components in vertex normal";
                } else {
                    if (!skipping) {
                        const float x = tokens.floatAt(1);
                        const float y = tokens.floatAt(2);
                        const float z = tokens.floatAt(3);
                        normals.append(QVector3D(x, y, z));
                    } else {
                        ++normalsOffset;
                    }
                }
            } else if (!skipping && tokens.size() >= 4 && qstrncmp(tokens.charPtrAt(0), "f ", 2) == 0) {
                // Process face
                int faceVertices = tokens.size() - 1;

                QVarLengthArray<FaceIndices, 4> face; // try to avoid allocations in the common case of triangulated data
                face.reserve(faceVertices);

                for (int i = 0; i < faceVertices; i++) {
                    FaceIndices faceIndices;
                    const ByteArraySplitter indices = tokens.splitterAt(i + 1, '/', Qt::KeepEmptyParts);
                    switch (indices.size()) {
                    case 3:
                        faceIndices.normalIndex = indices.intAt(2) - 1 - normalsOffset;  // fall through
                        Q_FALLTHROUGH();
                    case 2:
                        faceIndices.texCoordIndex = indices.intAt(1) - 1 - texCoordsOffset; // fall through
                        Q_FALLTHROUGH();
                    case 1:
                        faceIndices.positionIndex = indices.intAt(0) - 1 - positionsOffset;
                        break;
                    default:
                        qCWarning(ObjGeometryLoaderLog) << "Unsupported number of indices in face element";
                    }

                    face.append(faceIndices);
                }

                // If number of edges in face is greater than 3,
                // decompose into triangles as a triangle fan.
                FaceIndices v0 = face[0];
                FaceIndices v1 = face[1];
                FaceIndices v2 = face[2];

                // First face
                addFaceVertex(v0, faceIndexVector, faceIndexMap);
                addFaceVertex(v1, faceIndexVector, faceIndexMap);
                addFaceVertex(v2, faceIndexVector, faceIndexMap);

                for (int i = 3; i < face.size(); ++i) {
                    v1 = v2;
                    v2 = face[i];
                    addFaceVertex(v0, faceIndexVector, faceIndexMap);
                    addFaceVertex(v1, faceIndexVector, faceIndexMap);
                    addFaceVertex(v2, faceIndexVector, faceIndexMap);
                }

                // end of face
            } else if (qstrncmp(tokens.charPtrAt(0), "o ", 2) == 0) {
                if (tokens.size() < 2) {
                    qCWarning(ObjGeometryLoaderLog) << "Missing submesh name";
                } else {
                    if (!subMesh.isEmpty() ) {
                        const QString objName = tokens.stringAt(1);
                        QRegularExpressionMatch match = subMeshMatch.match(objName);
                        skipping = !match.hasMatch();
                    }
                }
            }
        } // empty input line
    } // while (!ioDev->atEnd())

    // Iterate over the faceIndexMap and pull out pos, texCoord and normal data
    // thereby generating unique vertices of data (by OpenGL definition)
    const qsizetype vertexCount = faceIndexMap.size();
    const bool hasTexCoords = !texCoords.isEmpty();
    const bool hasNormals = !normals.isEmpty();

    m_points.resize(vertexCount);
    m_texCoords.clear();
    if (hasTexCoords)
        m_texCoords.resize(vertexCount);
    m_normals.clear();
    if (hasNormals)
        m_normals.resize(vertexCount);

    for (auto it = faceIndexMap.cbegin(), endIt = faceIndexMap.cend(); it != endIt; ++it) {
        const uint positionIndex = it.key().positionIndex;
        const uint texCoordIndex = it.key().texCoordIndex;
        const uint normalIndex = it.key().normalIndex;

        m_points[it.value()] = (positionIndex < uint(positions.size())) ? positions[positionIndex] : QVector3D();
        if (hasTexCoords)
            m_texCoords[it.value()] = (texCoordIndex < uint(texCoords.size())) ? texCoords[texCoordIndex] : QVector2D();
        if (hasNormals)
            m_normals[it.value()] = (normalIndex < uint(normals.size())) ? normals[normalIndex] : QVector3D();
    }

    // Now iterate over the face indices and lookup the unique vertex index
    const qsizetype indexCount = faceIndexVector.size();
    m_indices.clear();
    m_indices.reserve(indexCount);
    for (const FaceIndices &faceIndices : std::as_const(faceIndexVector)) {
        const unsigned int i = faceIndexMap.value(faceIndices);
        m_indices.push_back(i);
    }

    return true;
}

static void addFaceVertex(const FaceIndices &faceIndices,
                          QList<FaceIndices> &faceIndexVector,
                          QHash<FaceIndices, unsigned int> &faceIndexMap)
{
    if (faceIndices.positionIndex != std::numeric_limits<unsigned int>::max()) {
        faceIndexVector.append(faceIndices);
        if (!faceIndexMap.contains(faceIndices))
            faceIndexMap.insert(faceIndices, faceIndexMap.size());
    } else {
        qCWarning(ObjGeometryLoaderLog) << "Missing position index";
    }
}

} // namespace Qt3DRender

QT_END_NAMESPACE
