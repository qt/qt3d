// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_BASEGEOMETRYLOADER_H
#define QT3DRENDER_BASEGEOMETRYLOADER_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of other Qt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QtCore/QObject>

#include <QtGui/QVector2D>
#include <QtGui/QVector3D>
#include <QtGui/QVector4D>

#include <Qt3DRender/private/qgeometryloaderinterface_p.h>

#include <vector>

#include <private/qlocale_tools_p.h>

QT_BEGIN_NAMESPACE

class QIODevice;
class QString;

namespace Qt3DCore {
class QGeometry;
}

namespace Qt3DRender {

class BaseGeometryLoader : public QGeometryLoaderInterface
{
    Q_OBJECT
public:
    BaseGeometryLoader();

    void setLoadTextureCoordinatesEnabled(bool b) { m_loadTextureCoords = b; }
    bool isLoadTextureCoordinatesEnabled() const { return m_loadTextureCoords; }

    void setTangentGenerationEnabled(bool b) { m_generateTangents = b; }
    bool isTangentGenerationEnabled() const { return m_generateTangents; }

    void setMeshCenteringEnabled(bool b) { m_centerMesh = b; }
    bool isMeshCenteringEnabled() const { return m_centerMesh; }

    bool hasNormals() const { return !m_normals.empty(); }
    bool hasTextureCoordinates() const { return !m_texCoords.empty(); }
    bool hasTangents() const { return !m_tangents.empty(); }

    const std::vector<QVector3D> &vertices() const { return m_points; }
    const std::vector<QVector3D> &normals() const { return m_normals; }
    const std::vector<QVector2D> &textureCoordinates() const { return m_texCoords; }
    const std::vector<QVector4D> &tangents() const { return m_tangents; }
    const std::vector<unsigned int> &indices() const { return m_indices; }

    Qt3DCore::QGeometry *geometry() const override;

    bool load(QIODevice *ioDev, const QString &subMesh = QString()) override;

protected:
    virtual bool doLoad(QIODevice *ioDev, const QString &subMesh = QString()) = 0;

    void generateAveragedNormals(const std::vector<QVector3D>& points,
                                 std::vector<QVector3D>& normals,
                                 const std::vector<unsigned int>& faces) const;
    void generateGeometry();
    void generateTangents(const std::vector<QVector3D>& points,
                          const std::vector<QVector3D>& normals,
                          const std::vector<unsigned int>& faces,
                          const std::vector<QVector2D>& texCoords,
                          std::vector<QVector4D>& tangents) const;
    void center(std::vector<QVector3D>& points);

    bool m_loadTextureCoords;
    bool m_generateTangents;
    bool m_centerMesh;

    std::vector<QVector3D> m_points;
    std::vector<QVector3D> m_normals;
    std::vector<QVector2D> m_texCoords;
    std::vector<QVector4D> m_tangents;
    std::vector<unsigned int> m_indices;

    Qt3DCore::QGeometry *m_geometry;
};

struct FaceIndices
{
    FaceIndices()
        : positionIndex(std::numeric_limits<unsigned int>::max())
        , texCoordIndex(std::numeric_limits<unsigned int>::max())
        , normalIndex(std::numeric_limits<unsigned int>::max())
    {}

    FaceIndices(unsigned int posIndex, unsigned int tcIndex, unsigned int nIndex)
        : positionIndex(posIndex)
        , texCoordIndex(tcIndex)
        , normalIndex(nIndex)
    {}

    bool operator == (const FaceIndices &other) const
    {
        return positionIndex == other.positionIndex &&
               texCoordIndex == other.texCoordIndex &&
               normalIndex == other.normalIndex;
    }

    unsigned int positionIndex;
    unsigned int texCoordIndex;
    unsigned int normalIndex;
};
QT3D_DECLARE_TYPEINFO(Qt3DRender, FaceIndices, Q_PRIMITIVE_TYPE)

struct ByteArraySplitterEntry
{
    int start;
    int size;
};
QT3D_DECLARE_TYPEINFO(Qt3DRender, ByteArraySplitterEntry, Q_PRIMITIVE_TYPE)

/*
 * A helper class to split a QByteArray and access its sections without
 * additional memory allocations.
 */
class ByteArraySplitter
{
public:
    explicit ByteArraySplitter(const char *begin, const char *end, char delimiter, Qt::SplitBehavior splitBehavior)
        : m_input(begin)
    {
        int position = 0;
        int lastPosition = 0;
        for (auto it = begin; it != end; ++it) {
            if (*it == delimiter) {
                if (position > lastPosition || splitBehavior == Qt::KeepEmptyParts) { // skip multiple consecutive delimiters
                    const ByteArraySplitterEntry entry = { lastPosition, position - lastPosition };
                    m_entries.append(entry);
                }
                lastPosition = position + 1;
            }

            ++position;
        }

        const ByteArraySplitterEntry entry = { lastPosition, position - lastPosition };
        m_entries.append(entry);
    }

    int size() const
    {
        return m_entries.size();
    }

    const char *charPtrAt(int index) const
    {
        return m_input + m_entries[index].start;
    }

    float floatAt(int index) const
    {
        return qstrntod(m_input + m_entries[index].start, m_entries[index].size, nullptr, nullptr);
    }

    int intAt(int index) const
    {
        return strtol(m_input + m_entries[index].start, nullptr, 10);
    }

    QString stringAt(int index) const
    {
        return QString::fromLatin1(m_input + m_entries[index].start, m_entries[index].size);
    }

    ByteArraySplitter splitterAt(int index, char delimiter, Qt::SplitBehavior splitBehavior) const
    {
        return ByteArraySplitter(m_input + m_entries[index].start, m_input + m_entries[index].start + m_entries[index].size, delimiter, splitBehavior);
    }

private:
    QVarLengthArray<ByteArraySplitterEntry, 16> m_entries;
    const char *m_input;
};

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_BASEGEOMETRYLOADER_H
