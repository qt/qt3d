/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd and/or its subsidiary(-ies).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "gltfexporter.h"

#include <QtCore/qiodevice.h>
#include <QtCore/qfile.h>
#include <QtCore/qfileinfo.h>
#include <QtCore/qdir.h>
#include <QtCore/qhash.h>
#include <QtCore/qdebug.h>
#include <QtCore/qcoreapplication.h>
#include <QtCore/qjsondocument.h>
#include <QtCore/qjsonobject.h>
#include <QtCore/qjsonarray.h>
#include <QtCore/qmath.h>
#include <QtCore/qtemporarydir.h>
#include <QtCore/qregularexpression.h>
#include <QtGui/qvector2d.h>
#include <QtGui/qvector4d.h>
#include <QtGui/qmatrix4x4.h>

#include <Qt3DCore/qentity.h>
#include <Qt3DCore/qtransform.h>
#include <Qt3DRender/qcameralens.h>
#include <Qt3DRender/qcamera.h>
#include <Qt3DRender/qblendequation.h>
#include <Qt3DRender/qblendequationarguments.h>
#include <Qt3DRender/qeffect.h>
#include <Qt3DRender/qattribute.h>
#include <Qt3DRender/qbuffer.h>
#include <Qt3DRender/qbufferdatagenerator.h>
#include <Qt3DRender/qmaterial.h>
#include <Qt3DRender/qgraphicsapifilter.h>
#include <Qt3DRender/qparameter.h>
#include <Qt3DRender/qtexture.h>
#include <Qt3DRender/qabstractlight.h>
#include <Qt3DRender/qpointlight.h>
#include <Qt3DRender/qspotlight.h>
#include <Qt3DRender/qdirectionallight.h>
#include <Qt3DRender/qgeometry.h>
#include <Qt3DRender/qgeometryrenderer.h>
#include <Qt3DRender/qgeometryfactory.h>
#include <Qt3DExtras/qconemesh.h>
#include <Qt3DExtras/qcuboidmesh.h>
#include <Qt3DExtras/qcylindermesh.h>
#include <Qt3DExtras/qplanemesh.h>
#include <Qt3DExtras/qspheremesh.h>
#include <Qt3DExtras/qtorusmesh.h>
#include <Qt3DExtras/qphongmaterial.h>
#include <Qt3DExtras/qphongalphamaterial.h>
#include <Qt3DExtras/qdiffusemapmaterial.h>
#include <Qt3DExtras/qdiffusespecularmapmaterial.h>
#include <Qt3DExtras/qnormaldiffusemapmaterial.h>
#include <Qt3DExtras/qnormaldiffusemapalphamaterial.h>
#include <Qt3DExtras/qnormaldiffusespecularmapmaterial.h>
#include <Qt3DExtras/qgoochmaterial.h>
#include <Qt3DExtras/qpervertexcolormaterial.h>

#include <private/qurlhelper_p.h>

#define GLT_UNSIGNED_SHORT 0x1403
#define GLT_UNSIGNED_INT 0x1405
#define GLT_FLOAT 0x1406
#define GLT_ARRAY_BUFFER 0x8892
#define GLT_ELEMENT_ARRAY_BUFFER 0x8893

#ifndef qUtf16PrintableImpl
#  define qUtf16PrintableImpl(string) \
    static_cast<const wchar_t*>(static_cast<const void*>(string.utf16()))
#endif

QT_BEGIN_NAMESPACE

using namespace Qt3DCore;
using namespace Qt3DExtras;

namespace Qt3DRender {

Q_LOGGING_CATEGORY(GLTFExporterLog, "Qt3D.GLTFExport")

const QString MATERIAL_DIFFUSE_COLOR = QStringLiteral("kd");
const QString MATERIAL_SPECULAR_COLOR = QStringLiteral("ks");
const QString MATERIAL_AMBIENT_COLOR = QStringLiteral("ka");

const QString MATERIAL_DIFFUSE_TEXTURE = QStringLiteral("diffuseTexture");
const QString MATERIAL_SPECULAR_TEXTURE = QStringLiteral("specularTexture");
const QString MATERIAL_NORMALS_TEXTURE = QStringLiteral("normalTexture");

const QString MATERIAL_SHININESS = QStringLiteral("shininess");
const QString MATERIAL_ALPHA = QStringLiteral("alpha");

// Custom extension for Qt3D
const QString MATERIAL_TEXTURE_SCALE = QStringLiteral("texCoordScale");

// Custom gooch material values
const QString MATERIAL_BETA = QStringLiteral("beta");
const QString MATERIAL_COOL_COLOR = QStringLiteral("kblue");
const QString MATERIAL_WARM_COLOR = QStringLiteral("kyellow");

const QString VERTICES_ATTRIBUTE_NAME = QAttribute::defaultPositionAttributeName();
const QString NORMAL_ATTRIBUTE_NAME =  QAttribute::defaultNormalAttributeName();
const QString TANGENT_ATTRIBUTE_NAME = QAttribute::defaultTangentAttributeName();
const QString TEXTCOORD_ATTRIBUTE_NAME = QAttribute::defaultTextureCoordinateAttributeName();
const QString COLOR_ATTRIBUTE_NAME = QAttribute::defaultColorAttributeName();

GLTFExporter::GLTFExporter() : QSceneExporter()
  , m_sceneRoot(nullptr)
  , m_rootNode(nullptr)
  , m_rootNodeEmpty(false)

{
}

GLTFExporter::~GLTFExporter()
{
}

// Calculate bounding box
void calcBB(QVector<float> &minVal, QVector<float> &maxVal, const float *data,
            int vertexCount, int compCount, int offset, int stride)
{
    minVal.resize(compCount);
    maxVal.resize(compCount);
    int dataIndex = offset;
    const int adjustedStride = stride > 0 ? stride - compCount : 0;
    for (int i = 0; i < vertexCount; ++i) {
        for (int j = 0; j < compCount; ++j) {
            if (i == 0) {
                minVal[j] = data[dataIndex];
                maxVal[j] = data[dataIndex];
            } else {
                if (data[dataIndex] < minVal[j])
                    minVal[j] = data[dataIndex];
                if (data[dataIndex] > maxVal[j])
                    maxVal[j] = data[dataIndex];
            }
            dataIndex++;
        }
        dataIndex += adjustedStride;
    }
}

// sceneRoot  : The root entity that contains the exported scene. If the sceneRoot doesn't have
//              any exportable components, it is not exported itself. This is because importing a
//              scene creates an empty top level entity to hold the scene.
// outDir     : The directory where the scene export directory is created in.
// exportName : Name of the directory created in outDir to hold the exported scene. Also used as
//              the file name base for generated files.
// options    : Export options.
//
// Supported options are:
// "binaryJson"  (bool): Generates a binary JSON file, which is more efficient to parse.
// "compactJson" (bool): Removes unnecessary whitespace from the generated JSON file.
//                       Ignored if "binaryJson" option is true.
bool GLTFExporter::exportScene(QEntity *sceneRoot, const QString &outDir,
                               const QString &exportName, const QVariantHash &options)
{
    m_buffer.clear();
    m_meshMap.clear();
    m_materialMap.clear();
    m_cameraMap.clear();
    m_lightMap.clear();
    m_imageMap.clear();
    m_meshInfo.clear();
    m_materialInfo.clear();
    m_cameraInfo.clear();
    m_exportedFiles.clear();

    delNode(m_rootNode);

    m_bufferViewCount = 0;
    m_accessorCount = 0;
    m_meshCount = 0;
    m_materialCount = 0;
    m_techniqueCount = 0;
    m_textureCount = 0;
    m_imageCount = 0;
    m_shaderCount = 0;
    m_programCount = 0;
    m_nodeCount = 0;
    m_cameraCount = 0;
    m_lightCount = 0;

    m_gltfOpts.binaryJson = options.value(QStringLiteral("binaryJson"),
                                          QVariant(false)).toBool();
    m_gltfOpts.compactJson = options.value(QStringLiteral("compactJson"),
                                           QVariant(false)).toBool();

    QFileInfo outDirFileInfo(outDir);
    QString absoluteOutDir = outDirFileInfo.absoluteFilePath();
    if (!absoluteOutDir.endsWith(QLatin1Char('/')))
        absoluteOutDir.append(QLatin1Char('/'));
    m_exportName = exportName;
    m_sceneRoot = sceneRoot;
    QString finalExportDir = absoluteOutDir + m_exportName;
    if (!finalExportDir.endsWith(QLatin1Char('/')))
        finalExportDir.append(QLatin1Char('/'));

    QDir outDirDir(absoluteOutDir);

    // Make sure outDir exists
    if (outDirFileInfo.exists()) {
        if (!outDirFileInfo.isDir()) {
            qCWarning(GLTFExporterLog, "outDir is not a directory: '%ls'",
                      qUtf16PrintableImpl(absoluteOutDir));
            return false;
        }
    } else {
        if (!outDirDir.mkpath(outDirFileInfo.absoluteFilePath())) {
            qCWarning(GLTFExporterLog, "outDir could not be created: '%ls'",
                      qUtf16PrintableImpl(absoluteOutDir));
            return false;
        }
    }

    // Create temporary directory for exporting
    QTemporaryDir exportDir;

    if (!exportDir.isValid()) {
        qCWarning(GLTFExporterLog, "Temporary export directory could not be created");
        return false;
    }
    m_exportDir = exportDir.path();
    m_exportDir.append(QStringLiteral("/"));

    qCDebug(GLTFExporterLog, "Output directory: &ls", qUtf16PrintableImpl(absoluteOutDir));
    qCDebug(GLTFExporterLog, "Export name: &ls", qUtf16PrintableImpl(m_exportName));
    qCDebug(GLTFExporterLog, "Temp export dir: &ls", qUtf16PrintableImpl(m_exportDir));
    qCDebug(GLTFExporterLog, "Final export dir: &ls", qUtf16PrintableImpl(finalExportDir));

    parseScene();

    // Copy textures into temporary directory
    copyTextures();

    // Export scene to temporary directory
    if (!saveScene()) {
        qCWarning(GLTFExporterLog, "Exporting GLTF scene failed");
        return false;
    }

    // Create final export directory
    if (!outDirDir.mkpath(m_exportName)) {
        qCWarning(GLTFExporterLog, "Final export directory could not be created: '%ls'",
                  qUtf16PrintableImpl(finalExportDir));
        return false;
    }

    // As a safety feature, we don't indiscriminately delete existing directory or it's contents,
    // but instead look for an old export and delete only related files.
    clearOldExport(finalExportDir);

    // Files copied from resources will have read-only permissions, which isn't ideal in cases
    // where export is done on top of an existing export.
    // Since different file systems handle permissions differently, we grab the target permissions
    // from the qgltf file, which we created ourselves.
    QFile gltfFile(m_exportDir + m_exportName + QStringLiteral(".qgltf"));
    QFile::Permissions targetPermissions = gltfFile.permissions();

    // Copy exported scene to actual export directory
    for (auto sourceFileStr : m_exportedFiles) {
        QFileInfo fiSource(m_exportDir + sourceFileStr);
        QFileInfo fiDestination(finalExportDir + sourceFileStr);
        if (fiDestination.exists()) {
            QFile(fiDestination.absoluteFilePath()).remove();
            qCDebug(GLTFExporterLog, "Removed old file: '%ls'",
                    qUtf16PrintableImpl(fiDestination.absoluteFilePath()));
        }
        QString srcPath = fiSource.absoluteFilePath();
        QString destPath = fiDestination.absoluteFilePath();
        if (!QFile(srcPath).copy(destPath)) {
            qCWarning(GLTFExporterLog, "  Failed to copy file: '%ls' -> '%ls'",
                      qUtf16PrintableImpl(srcPath), qUtf16PrintableImpl(destPath));
            // Don't fail entire export because file copy failed - if there is somehow a read-only
            // file with same name already in the export dir after cleanup we did, let's just assume
            // it's the same file we want rather than risk deleting unrelated protected file.
        } else {
            qCDebug(GLTFExporterLog, "  Copied file: '%ls' -> '%ls'",
                    qUtf16PrintableImpl(srcPath), qUtf16PrintableImpl(destPath));
            QFile(destPath).setPermissions(targetPermissions);
        }
    }

    return true;
}

// Copies textures from original locations to the temporary export directory.
// If texture names conflict, they are renamed.
void GLTFExporter::copyTextures()
{
    qCDebug(GLTFExporterLog, "Copying textures...");
    QHash<QString, QString> copiedMap;
    for (auto it = m_materialInfo.constBegin(); it != m_materialInfo.constEnd(); ++it) {
        const MaterialInfo &matInfo = it.value();
        for (auto texIt = matInfo.textures.constBegin(); texIt != matInfo.textures.constEnd();
             ++texIt) {
            QString texKey = texIt.key();
            QFileInfo fi(texIt.value());
            QString absoluteFilePath;
            if (texIt.value().startsWith(QStringLiteral(":")))
                absoluteFilePath = texIt.value();
            else
                absoluteFilePath = fi.absoluteFilePath();
            if (copiedMap.contains(absoluteFilePath)) {
                // Texture has already been copied
                qCDebug(GLTFExporterLog, "  Skipped copying duplicate texture: '%ls'",
                        qUtf16PrintableImpl(absoluteFilePath));
                if (!m_imageMap.contains(texIt.value()))
                    m_imageMap.insert(texIt.value(), copiedMap.value(absoluteFilePath));
            } else {
                QString fileName = fi.fileName();
                QString outFile = m_exportDir;
                outFile.append(fileName);
                QFileInfo fiTry(outFile);
                if (fiTry.exists()) {
                    static const QString outFileTemplate = QStringLiteral("%2_%3.%4");
                    int counter = 0;
                    QString tryFile = outFile;
                    QString suffix = fiTry.suffix();
                    QString base = fiTry.baseName();
                    while (fiTry.exists()) {
                        fileName = outFileTemplate.arg(base).arg(counter++).arg(suffix);
                        tryFile = m_exportDir;
                        tryFile.append(fileName);
                        fiTry.setFile(tryFile);
                    }
                    outFile = tryFile;
                }
                if (!QFile(absoluteFilePath).copy(outFile)) {
                    qCWarning(GLTFExporterLog, "  Failed to copy texture: '%ls' -> '%ls'",
                              qUtf16PrintableImpl(absoluteFilePath), qUtf16PrintableImpl(outFile));
                } else {
                    qCDebug(GLTFExporterLog, "  Copied texture: '%ls' -> '%ls'",
                            qUtf16PrintableImpl(absoluteFilePath), qUtf16PrintableImpl(outFile));
                }
                // Generate actual target file (as current exportDir is temp dir)
                copiedMap.insert(absoluteFilePath, fileName);
                m_exportedFiles.insert(fileName);
                m_imageMap.insert(texIt.value(), fileName);
            }
        }
    }
}


void GLTFExporter::parseEntities(const QEntity *entity, Node *parentNode)
{
    if (entity) {
        Node *node =  new Node;
        node->name = entity->objectName();
        node->uniqueName = newNodeName();

        int irrelevantComponents = 0;
        for (auto component : entity->components()) {
            if (auto mesh = qobject_cast<QGeometryRenderer *>(component))
                m_meshMap.insert(node, mesh);
            else if (auto material = qobject_cast<QMaterial *>(component))
                m_materialMap.insert(node, material);
            else if (auto transform = qobject_cast<Qt3DCore::QTransform *>(component))
                m_transformMap.insert(node, transform);
            else if (auto camera = qobject_cast<QCameraLens *>(component))
                m_cameraMap.insert(node, camera);
            else if (auto light = qobject_cast<QAbstractLight *>(component))
                m_lightMap.insert(node, light);
            else
                irrelevantComponents++;
        }
        if (!parentNode) {
            m_rootNode = node;
            if (irrelevantComponents == entity->components().size())
                m_rootNodeEmpty = true;
        } else {
            parentNode->children.append(node);
        }
        qCDebug(GLTFExporterLog, "Parsed entity '%ls' -> '%ls'",
                qUtf16PrintableImpl(entity->objectName()), qUtf16PrintableImpl(node->uniqueName));

        for (auto child : entity->children())
            parseEntities(qobject_cast<QEntity *>(child), node);
    }
}

void GLTFExporter::parseScene()
{
    parseEntities(m_sceneRoot, nullptr);
    parseMaterials();
    parseMeshes();
    parseCameras();
    parseLights();
}

void GLTFExporter::parseMaterials()
{
    qCDebug(GLTFExporterLog, "Parsing materials...");

    int materialCount = 0;
    for (auto it = m_materialMap.constBegin(); it != m_materialMap.constEnd(); ++it) {
        QMaterial *material = it.value();

        MaterialInfo matInfo;
        matInfo.name = newMaterialName();
        matInfo.originalName = material->objectName();

        // Is material common or custom?
        if (qobject_cast<QPhongMaterial *>(material)) {
            matInfo.type = MaterialInfo::TypePhong;
        } else if (auto phongAlpha = qobject_cast<QPhongAlphaMaterial *>(material)) {
            matInfo.type = MaterialInfo::TypePhongAlpha;
            matInfo.blendArguments.resize(4);
            matInfo.blendEquations.resize(2);
            matInfo.blendArguments[0] = int(phongAlpha->sourceRgbArg());
            matInfo.blendArguments[1] = int(phongAlpha->sourceAlphaArg());
            matInfo.blendArguments[2] = int(phongAlpha->destinationRgbArg());
            matInfo.blendArguments[3] = int(phongAlpha->destinationAlphaArg());
            matInfo.blendEquations[0] = int(phongAlpha->blendFunctionArg());
            matInfo.blendEquations[1] = int(phongAlpha->blendFunctionArg());
        } else if (qobject_cast<QDiffuseMapMaterial *>(material)) {
            matInfo.type = MaterialInfo::TypeDiffuseMap;
        } else if (qobject_cast<QDiffuseSpecularMapMaterial *>(material)) {
            matInfo.type = MaterialInfo::TypeDiffuseSpecularMap;
        } else if (qobject_cast<QNormalDiffuseMapAlphaMaterial *>(material)) {
            matInfo.values.insert(QStringLiteral("transparent"), QVariant(true));
            matInfo.type = MaterialInfo::TypeNormalDiffuseMapAlpha;
        } else if (qobject_cast<QNormalDiffuseMapMaterial *>(material)) {
            matInfo.type = MaterialInfo::TypeNormalDiffuseMap;
        } else if (qobject_cast<QNormalDiffuseSpecularMapMaterial *>(material)) {
            matInfo.type = MaterialInfo::TypeNormalDiffuseSpecularMap;
        } else if (qobject_cast<QGoochMaterial *>(material)) {
            matInfo.type = MaterialInfo::TypeGooch;
        } else if (qobject_cast<QPerVertexColorMaterial *>(material)) {
            matInfo.type = MaterialInfo::TypePerVertex;
        } else {
            matInfo.type = MaterialInfo::TypeCustom;
            // TODO: Implement support for custom materials
            qCDebug(GLTFExporterLog, "Exporting custom materials is not supported: %s",
                    material->metaObject()->className());
            continue;
        }

        if (material->effect()) {
            QVector<QParameter *> parameters = material->effect()->parameters();
            for (auto param : parameters) {
                if (param->value().type() == QVariant::Color) {
                    QColor color = param->value().value<QColor>();
                    if (param->name() == MATERIAL_AMBIENT_COLOR)
                        matInfo.colors.insert(QStringLiteral("ambient"), color);
                    else if (param->name() == MATERIAL_DIFFUSE_COLOR)
                        matInfo.colors.insert(QStringLiteral("diffuse"), color);
                    else if (param->name() == MATERIAL_SPECULAR_COLOR)
                        matInfo.colors.insert(QStringLiteral("specular"), color);
                    else if (param->name() == MATERIAL_COOL_COLOR) // Custom Qt3D param for gooch
                        matInfo.colors.insert(QStringLiteral("cool"), color);
                    else if (param->name() == MATERIAL_WARM_COLOR) // Custom Qt3D param for gooch
                        matInfo.colors.insert(QStringLiteral("warm"), color);
                    else
                        matInfo.colors.insert(param->name(), color);
                } else if (param->value().canConvert<QAbstractTexture *>()) {
                    QAbstractTexture *texture = param->value().value<QAbstractTexture *>();
                    for (auto ti : texture->textureImages()) {
                        QString urlString;
                        Qt3DRender::QTextureImage *image =
                                qobject_cast<Qt3DRender::QTextureImage *>(ti);
                        if (image)
                            urlString = QUrlHelper::urlToLocalFileOrQrc(image->source());

                        if (param->name() == MATERIAL_DIFFUSE_TEXTURE)
                            matInfo.textures.insert(QStringLiteral("diffuse"), urlString);
                        else if (param->name() == MATERIAL_SPECULAR_TEXTURE)
                            matInfo.textures.insert(QStringLiteral("specular"), urlString);
                        else if (param->name() == MATERIAL_NORMALS_TEXTURE)
                            matInfo.textures.insert(QStringLiteral("normal"), urlString);
                        else
                            matInfo.textures.insert(param->name(), urlString);
                    }
                } else if (param->name() == MATERIAL_SHININESS) {
                    matInfo.values.insert(QStringLiteral("shininess"), param->value());
                } else if (param->name() == MATERIAL_BETA) { // Custom Qt3D param for gooch
                    matInfo.values.insert(QStringLiteral("beta"), param->value());
                } else if (param->name() == MATERIAL_ALPHA) {
                    if (matInfo.type == MaterialInfo::TypeGooch)
                        matInfo.values.insert(QStringLiteral("alpha"), param->value());
                    else
                        matInfo.values.insert(QStringLiteral("transparency"), param->value());
                } else if (param->name() == MATERIAL_TEXTURE_SCALE) { // Custom Qt3D param
                    matInfo.values.insert(QStringLiteral("textureScale"), param->value());
                } else {
                    if (matInfo.type == MaterialInfo::TypeCustom) {
                        matInfo.values.insert(param->name(), param->value());
                    } else {
                        qCDebug(GLTFExporterLog,
                                "Common material had unknown parameter: '%ls'",
                                qUtf16PrintableImpl(param->name()));
                    }
                }
            }
        }
        if (GLTFExporterLog().isDebugEnabled()) {
            qCDebug(GLTFExporterLog, "  Material #%i", materialCount);
            qCDebug(GLTFExporterLog, "    name: '%ls'", qUtf16PrintableImpl(matInfo.name));
            qCDebug(GLTFExporterLog, "    originalName: '%ls'",
                    qUtf16PrintableImpl(matInfo.originalName));
            qCDebug(GLTFExporterLog, "    type: %i", matInfo.type);
            qCDebug(GLTFExporterLog) << "    colors:" << matInfo.colors;
            qCDebug(GLTFExporterLog) << "    values:" << matInfo.values;
            qCDebug(GLTFExporterLog) << "    textures:" << matInfo.textures;
        }

        m_materialInfo.insert(material, matInfo);
        materialCount++;
    }
}

void GLTFExporter::parseMeshes()
{
    qCDebug(GLTFExporterLog, "Parsing meshes...");

    int meshCount = 0;
    for (auto it = m_meshMap.constBegin(); it != m_meshMap.constEnd(); ++it) {
        Node *node = it.key();
        QGeometryRenderer *mesh = it.value();

        MeshInfo meshInfo;
        meshInfo.originalName = mesh->objectName();
        meshInfo.name = newMeshName();
        meshInfo.materialName = m_materialInfo.value(m_materialMap.value(node)).name;

        bool defaultType = qobject_cast<QConeMesh *>(mesh) || qobject_cast<QCuboidMesh *>(mesh)
                || qobject_cast<QCylinderMesh *>(mesh) || qobject_cast<QPlaneMesh *>(mesh)
                || qobject_cast<QSphereMesh *>(mesh) || qobject_cast<QTorusMesh *>(mesh);

        QGeometry *meshGeometry = nullptr;
        QGeometryFactoryPtr geometryFunctorPtr = mesh->geometryFactory();
        if (defaultType || !geometryFunctorPtr.data()) {
            meshGeometry = mesh->geometry();
        } else {
            // Execute the geometry functor to get the geometry, if it is available.
            // Functor gives us the latest data if geometry has changed.
            meshGeometry = geometryFunctorPtr.data()->operator()();
        }

        if (!meshGeometry) {
            qCWarning(GLTFExporterLog, "Ignoring mesh without geometry!");
            continue;
        }

        QAttribute *indexAttrib = nullptr;
        QAttribute *verticesAttrib = nullptr;
        QAttribute *normalsAttrib = nullptr;
        QAttribute *texCoordsAttrib = nullptr;
        QAttribute *colorAttrib = nullptr;
        QAttribute *tangentsAttrib = nullptr;

        const float *vertexPtr = nullptr;
        const float *normalsPtr = nullptr;
        const float *texCoordsPtr = nullptr;
        const float *colorPtr = nullptr;
        const float *tangentsPtr = nullptr;
        const quint16 *indexPtr = nullptr;

        for (auto att : meshGeometry->attributes()) {
            if (att->attributeType() == QAttribute::IndexAttribute) {
                indexAttrib = att;
                indexPtr = reinterpret_cast<const quint16 *>(att->buffer()->data().constData());
            } else if (att->name() == VERTICES_ATTRIBUTE_NAME) {
                verticesAttrib = att;
                vertexPtr = reinterpret_cast<const float *>(att->buffer()->data().constData());
            } else if (att->name() == NORMAL_ATTRIBUTE_NAME) {
                normalsAttrib = att;
                normalsPtr = reinterpret_cast<const float *>(att->buffer()->data().constData());
            } else if (att->name() == TEXTCOORD_ATTRIBUTE_NAME) {
                texCoordsAttrib = att;
                texCoordsPtr = reinterpret_cast<const float *>(att->buffer()->data().constData());
            } else if (att->name() == COLOR_ATTRIBUTE_NAME) {
                colorAttrib = att;
                colorPtr = reinterpret_cast<const float *>(att->buffer()->data().constData());
            } else if (att->name() == TANGENT_ATTRIBUTE_NAME) {
                tangentsAttrib = att;
                tangentsPtr = reinterpret_cast<const float *>(att->buffer()->data().constData());
            }
        }

        Q_ASSERT(verticesAttrib);

        // Default types use single interleaved buffer for all vertex data, but we must regenerate
        // it as it is not available on the frontend by default.
        QByteArray defaultVertexArray;
        QByteArray defaultIndexArray;
        if (defaultType) {
            defaultVertexArray = verticesAttrib->buffer()->dataGenerator().data()->operator()();
            const float *defaultVertexBufferPtr =
                    reinterpret_cast<const float *>(defaultVertexArray.constData());
            vertexPtr = defaultVertexBufferPtr;
            if (normalsPtr)
                normalsPtr = defaultVertexBufferPtr;
            if (texCoordsPtr)
                texCoordsPtr = defaultVertexBufferPtr;
            if (colorPtr)
                colorPtr = defaultVertexBufferPtr;
            if (tangentsPtr)
                tangentsPtr = defaultVertexBufferPtr;

            defaultIndexArray = indexAttrib->buffer()->dataGenerator().data()->operator()();
            indexPtr = reinterpret_cast<const quint16 *>(defaultIndexArray.constData());
        }

        const uint vertexOffset = verticesAttrib->byteOffset() / sizeof(float);
        uint normalOffset = 0;
        uint textCoordOffset = 0;
        uint colorOffset = 0;
        uint tangentOffset = 0;

        const uint vertexStride = verticesAttrib->byteStride() > 0
                ? verticesAttrib->byteStride() / sizeof(float) - 3 : 0;
        uint normalStride = 0;
        uint textCoordStride = 0;
        uint colorStride = 0;
        uint tangentStride = 0;

        uint stride = 3;

        if (normalsAttrib) {
            normalOffset = normalsAttrib->byteOffset() / sizeof(float);
            normalStride = normalsAttrib->byteStride() / sizeof(float);
            if (normalStride > 0)
                normalStride -= 3;
            stride += 3;
        }
        if (texCoordsAttrib) {
            textCoordOffset = texCoordsAttrib->byteOffset() / sizeof(float);
            textCoordStride = texCoordsAttrib->byteStride() / sizeof(float);
            if (textCoordStride > 0)
                textCoordStride -= 2;
            stride += 2;
        }
        if (colorAttrib) {
            colorOffset = colorAttrib->byteOffset() / sizeof(float);
            colorStride = colorAttrib->byteStride() / sizeof(float);
            if (colorStride > 0)
                colorStride -= 4;
            stride += 4;
        }
        if (tangentsAttrib) {
            tangentOffset = tangentsAttrib->byteOffset() / sizeof(float);
            tangentStride = tangentsAttrib->byteStride() / sizeof(float);
            if (tangentStride > 0)
                tangentStride -= 4;
            stride += 4;
        }

        QByteArray vertexBuf;
        const int vertexCount = verticesAttrib->count();
        vertexBuf.resize(stride * vertexCount * sizeof(float));
        float *p = reinterpret_cast<float *>(vertexBuf.data());

        uint vertexIndex = vertexOffset;
        uint normalIndex = normalOffset;
        uint textCoordIndex = textCoordOffset;
        uint colorIndex = colorOffset;
        uint tangentIndex = tangentOffset;

        // Create interleaved buffer
        for (int j = 0; j < vertexCount; ++j) {
            *p++ = vertexPtr[vertexIndex++];
            *p++ = vertexPtr[vertexIndex++];
            *p++ = vertexPtr[vertexIndex++];
            vertexIndex += vertexStride;

            if (normalsPtr) {
                *p++ = normalsPtr[normalIndex++];
                *p++ = normalsPtr[normalIndex++];
                *p++ = normalsPtr[normalIndex++];
                normalIndex += normalStride;
            }

            if (texCoordsPtr) {
                *p++ = texCoordsPtr[textCoordIndex++];
                *p++ = texCoordsPtr[textCoordIndex++];
                textCoordIndex += textCoordStride;
            }

            if (colorPtr) {
                *p++ = colorPtr[colorIndex++];
                *p++ = colorPtr[colorIndex++];
                *p++ = colorPtr[colorIndex++];
                *p++ = colorPtr[colorIndex++];
                colorIndex += colorStride;
            }

            if (tangentsPtr) {
                *p++ = tangentsPtr[tangentIndex++];
                *p++ = tangentsPtr[tangentIndex++];
                *p++ = tangentsPtr[tangentIndex++];
                *p++ = tangentsPtr[tangentIndex++];
                tangentIndex += tangentStride;
            }
        }

        MeshInfo::BufferView vertexBufView;
        vertexBufView.name = newBufferViewName();
        vertexBufView.length = vertexBuf.size();
        vertexBufView.offset = m_buffer.size();
        vertexBufView.componentType = GLT_FLOAT;
        vertexBufView.target = GLT_ARRAY_BUFFER;
        meshInfo.views.append(vertexBufView);

        QByteArray indexBuf;
        MeshInfo::BufferView indexBufView;
        uint indexCount = 0;
        if (indexAttrib) {
            const uint indexSize = indexAttrib->vertexBaseType() == QAttribute::UnsignedShort
                    ? sizeof(quint16) : sizeof(quint32);
            indexCount = indexAttrib->count();
            uint srcIndex = indexAttrib->byteOffset() / indexSize;
            const uint indexStride = indexAttrib->byteStride()
                    ? indexAttrib->byteStride() / indexSize - 1: 0;
            indexBuf.resize(indexCount * indexSize);
            if (indexSize == sizeof(quint32)) {
                quint32 *dst = reinterpret_cast<quint32 *>(indexBuf.data());
                const quint32 *src = reinterpret_cast<const quint32 *>(indexPtr);
                for (uint j = 0; j < indexCount; ++j) {
                    *dst++ = src[srcIndex++];
                    srcIndex += indexStride;
                }
            } else {
                quint16 *dst = reinterpret_cast<quint16 *>(indexBuf.data());
                for (uint j = 0; j < indexCount; ++j) {
                    *dst++ = indexPtr[srcIndex++];
                    srcIndex += indexStride;
                }
            }

            indexBufView.name = newBufferViewName();
            indexBufView.length = indexBuf.size();
            indexBufView.offset = vertexBufView.offset + vertexBufView.length;
            indexBufView.componentType = indexSize == sizeof(quint32)
                    ? GLT_UNSIGNED_INT : GLT_UNSIGNED_SHORT;
            indexBufView.target = GLT_ELEMENT_ARRAY_BUFFER;
            meshInfo.views.append(indexBufView);
        }

        MeshInfo::Accessor acc;
        uint startOffset = 0;
        const float *bufPtr = reinterpret_cast<const float *>(vertexBuf.constData());
        acc.name = newAccessorName();
        acc.usage = QStringLiteral("POSITION");
        acc.bufferView = vertexBufView.name;
        acc.offset = 0;
        acc.stride = stride * sizeof(float);
        acc.count = vertexCount;
        acc.componentType = vertexBufView.componentType;
        acc.type = QStringLiteral("VEC3");
        calcBB(acc.minVal, acc.maxVal, bufPtr, vertexCount, 3, startOffset, stride);
        meshInfo.accessors.append(acc);
        startOffset += 3;

        if (normalsAttrib) {
            acc.name = newAccessorName();
            acc.usage = QStringLiteral("NORMAL");
            acc.offset = startOffset * sizeof(float);
            calcBB(acc.minVal, acc.maxVal, bufPtr, vertexCount, 3, startOffset, stride);
            meshInfo.accessors.append(acc);
            startOffset += 3;
        }
        if (texCoordsAttrib) {
            acc.name = newAccessorName();
            acc.usage = QStringLiteral("TEXCOORD_0");
            acc.offset = startOffset * sizeof(float);
            acc.type = QStringLiteral("VEC2");
            calcBB(acc.minVal, acc.maxVal, bufPtr, vertexCount, 2, startOffset, stride);
            meshInfo.accessors.append(acc);
            startOffset += 2;
        }
        if (colorAttrib) {
            acc.name = newAccessorName();
            acc.usage = QStringLiteral("COLOR");
            acc.offset = startOffset * sizeof(float);
            acc.type = QStringLiteral("VEC4");
            calcBB(acc.minVal, acc.maxVal, bufPtr, vertexCount, 4, startOffset, stride);
            meshInfo.accessors.append(acc);
            startOffset += 4;
        }
        if (tangentsAttrib) {
            acc.name = newAccessorName();
            acc.usage = QStringLiteral("TANGENT");
            acc.offset = startOffset * sizeof(float);
            acc.type = QStringLiteral("VEC4");
            calcBB(acc.minVal, acc.maxVal, bufPtr, vertexCount, 4, startOffset, stride);
            meshInfo.accessors.append(acc);
            startOffset += 4;
        }

        // Index
        if (indexAttrib) {
            acc.name = newAccessorName();
            acc.usage = QStringLiteral("INDEX");
            acc.bufferView = indexBufView.name;
            acc.offset = 0;
            acc.stride = 0;
            acc.count = indexCount;
            acc.componentType = indexBufView.componentType;
            acc.type = QStringLiteral("SCALAR");
            acc.minVal = acc.maxVal = QVector<float>();
            meshInfo.accessors.append(acc);
        }

        if (GLTFExporterLog().isDebugEnabled()) {
            qCDebug(GLTFExporterLog, "  Mesh #%i: (%ls/%ls)", meshCount,
                    qUtf16PrintableImpl(meshInfo.name), qUtf16PrintableImpl(meshInfo.originalName));
            qCDebug(GLTFExporterLog, "    Vertex count: %i", vertexCount);
            qCDebug(GLTFExporterLog, "    Bytes per vertex: %i", stride);
            qCDebug(GLTFExporterLog, "    Vertex buffer size (bytes): %i", vertexBuf.size());
            qCDebug(GLTFExporterLog, "    Index buffer size (bytes): %i", indexBuf.size());

            QStringList sl;
            for (auto bv : meshInfo.views)
                sl << bv.name;
            qCDebug(GLTFExporterLog) << "  buffer views:" << sl;
            sl.clear();
            for (auto acc : meshInfo.accessors)
                sl << acc.name;
            qCDebug(GLTFExporterLog) << "  accessors:" << sl;
            qCDebug(GLTFExporterLog, "  material: '%ls'",
                    qUtf16PrintableImpl(meshInfo.materialName));
        }

        m_buffer.append(vertexBuf);
        m_buffer.append(indexBuf);

        m_meshInfo.insert(mesh, meshInfo);
        meshCount++;
    }

    qCDebug(GLTFExporterLog, "Total buffer size: %i", m_buffer.size());
}

void GLTFExporter::parseCameras()
{
    qCDebug(GLTFExporterLog, "Parsing cameras...");
    int cameraCount = 0;
    for (auto it = m_cameraMap.constBegin(); it != m_cameraMap.constEnd(); ++it) {
        QCameraLens *camera = it.value();
        CameraInfo c;

        if (camera->projectionType() == QCameraLens::PerspectiveProjection) {
            c.perspective = true;
            c.aspectRatio = camera->aspectRatio();
            c.yfov = qDegreesToRadians(camera->fieldOfView());
        } else {
            c.perspective = false;
            // Note that accurate conversion from four properties of QCameraLens to just two
            // properties of gltf orthographic cameras is not feasible. Only centered cases
            // convert properly.
            c.xmag = qAbs(camera->left() - camera->right());
            c.ymag = qAbs(camera->top() - camera->bottom());
        }

        c.originalName = camera->objectName();
        c.name = newCameraName();
        c.znear = camera->nearPlane();
        c.zfar = camera->farPlane();

        // GLTF cameras point in -Z by default, the rest is in the
        // node matrix, so no separate look-at params given here.

        m_cameraInfo.insert(camera, c);
        if (GLTFExporterLog().isDebugEnabled()) {
            qCDebug(GLTFExporterLog, "  Camera: #%i: (%ls/%ls)", cameraCount++,
                    qUtf16PrintableImpl(c.name), qUtf16PrintableImpl(c.originalName));
            qCDebug(GLTFExporterLog, "    Aspect ratio: %i", c.aspectRatio);
            qCDebug(GLTFExporterLog, "    Fov: %i", c.yfov);
            qCDebug(GLTFExporterLog, "    Near: %i", c.znear);
            qCDebug(GLTFExporterLog, "    Far: %i", c.zfar);
        }
    }
}

void GLTFExporter::parseLights()
{
    qCDebug(GLTFExporterLog, "Parsing lights...");
    int lightCount = 0;
    for (auto it = m_lightMap.constBegin(); it != m_lightMap.constEnd(); ++it) {
        QAbstractLight *light = it.value();
        LightInfo lightInfo;
        lightInfo.direction = QVector3D();
        lightInfo.attenuation = QVector3D();
        lightInfo.cutOffAngle = 0.0f;
        lightInfo.type = light->type();
        if (light->type() == QAbstractLight::SpotLight) {
            QSpotLight *spot = qobject_cast<QSpotLight *>(light);
            lightInfo.direction = spot->localDirection();
            lightInfo.attenuation = QVector3D(spot->constantAttenuation(),
                                              spot->linearAttenuation(),
                                              spot->quadraticAttenuation());
            lightInfo.cutOffAngle = spot->cutOffAngle();
        } else if (light->type() == QAbstractLight::PointLight) {
            QPointLight *point = qobject_cast<QPointLight *>(light);
            lightInfo.attenuation = QVector3D(point->constantAttenuation(),
                                              point->linearAttenuation(),
                                              point->quadraticAttenuation());
        } else if (light->type() == QAbstractLight::DirectionalLight) {
            QDirectionalLight *directional = qobject_cast<QDirectionalLight *>(light);
            lightInfo.direction = directional->worldDirection();
        }
        lightInfo.color = light->color();
        lightInfo.intensity = light->intensity();

        lightInfo.originalName = light->objectName();
        lightInfo.name = newLightName();

        m_lightInfo.insert(light, lightInfo);

        if (GLTFExporterLog().isDebugEnabled()) {
            qCDebug(GLTFExporterLog, "  Light #%i: (%ls/%ls)", lightCount++,
                    qUtf16PrintableImpl(lightInfo.name), qUtf16PrintableImpl(lightInfo.originalName));
            qCDebug(GLTFExporterLog, "    Type: %i", lightInfo.type);
            qCDebug(GLTFExporterLog, "    Color: (%i, %i, %i, %i)", lightInfo.color.red(),
                    lightInfo.color.green(), lightInfo.color.blue(), lightInfo.color.alpha());
            qCDebug(GLTFExporterLog, "    Intensity: %i", lightInfo.intensity);
            qCDebug(GLTFExporterLog, "    Direction: (%f, %f, %f)", lightInfo.direction.x(),
                    lightInfo.direction.y(), lightInfo.direction.z());
            qCDebug(GLTFExporterLog, "    Attenuation: (%f, %f, %f)", lightInfo.attenuation.x(),
                    lightInfo.attenuation.y(), lightInfo.attenuation.z());
            qCDebug(GLTFExporterLog, "    CutOffAngle: %i", lightInfo.cutOffAngle);
        }
    }
}

static inline QJsonArray col2jsvec(const QColor &color, bool alpha = false)
{
    QJsonArray arr;
    arr << color.redF() << color.greenF() << color.blueF();
    if (alpha)
        arr << color.alphaF();
    return arr;
}

template <typename T>
static inline QJsonArray vec2jsvec(const QVector<T> &v)
{
    QJsonArray arr;
    for (int i = 0; i < v.count(); ++i)
        arr << v.at(i);
    return arr;
}

static inline QJsonArray vec2jsvec(const QVector2D &v)
{
    QJsonArray arr;
    arr << v.x() << v.y();
    return arr;
}

static inline QJsonArray vec2jsvec(const QVector3D &v)
{
    QJsonArray arr;
    arr << v.x() << v.y() << v.z();
    return arr;
}

static inline QJsonArray vec2jsvec(const QVector4D &v)
{
    QJsonArray arr;
    arr << v.x() << v.y() << v.z() << v.w();
    return arr;
}

static inline QJsonArray matrix2jsvec(const QMatrix4x4 &matrix)
{
    QJsonArray jm;
    const float *mtxp = matrix.constData();
    for (int j = 0; j < 16; ++j)
        jm.append(*mtxp++);
    return jm;
}

static inline void setVarToJSonObject(QJsonObject &jsObj, const QString &key, const QVariant &var)
{
    switch (var.type()) {
    case QVariant::Bool:
        jsObj[key] = var.toBool();
        break;
    case QMetaType::Float:
        jsObj[key] = var.value<float>();
        break;
    case QVariant::Vector2D:
        jsObj[key] = vec2jsvec(var.value<QVector2D>());
        break;
    case QVariant::Vector3D:
        jsObj[key] = vec2jsvec(var.value<QVector3D>());
        break;
    case QVariant::Vector4D:
        jsObj[key] = vec2jsvec(var.value<QVector4D>());
        break;
    case QVariant::Matrix4x4:
        jsObj[key] = matrix2jsvec(var.value<QMatrix4x4>());
        break;
    default:
        qCWarning(GLTFExporterLog, "Unknown value type for '%ls'", qUtf16PrintableImpl(key));
        break;
    }
}

static inline void promoteColorsToRGBA(QJsonObject *obj)
{
    auto it = obj->begin();
    auto itEnd = obj->end();
    while (it != itEnd) {
        QJsonArray arr = it.value().toArray();
        if (arr.count() == 3) {
            const QString key = it.key();
            if (key == QStringLiteral("ambient")
                    || key == QStringLiteral("diffuse")
                    || key == QStringLiteral("specular")
                    || key == QStringLiteral("warm")
                    || key == QStringLiteral("cool")) {
                arr.append(1);
                *it = arr;
            }
        }
        ++it;
    }
}

bool GLTFExporter::saveScene()
{
    qCDebug(GLTFExporterLog, "Saving scene...");

    QVector<MeshInfo::BufferView> bvList;
    QVector<MeshInfo::Accessor> accList;
    for (auto mi : m_meshInfo.values()) {
        for (auto v : mi.views)
            bvList << v;
        for (auto acc : mi.accessors)
            accList << acc;
    }

    m_obj = QJsonObject();

    QJsonObject asset;
    asset["generator"] = QString(QStringLiteral("GLTFExporter %1")).arg(qVersion());
    asset["version"] = QStringLiteral("1.0");
    asset["premultipliedAlpha"] = true;
    m_obj["asset"] = asset;

    QString bufName = m_exportName + QStringLiteral(".bin");
    QString binFileName = m_exportDir + bufName;
    QFile f(binFileName);
    QFileInfo fiBin(binFileName);

    if (f.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        qCDebug(GLTFExporterLog, "  Writing '%ls'", qUtf16PrintableImpl(binFileName));
        m_exportedFiles.insert(fiBin.fileName());
        f.write(m_buffer);
        f.close();
    } else {
        qCWarning(GLTFExporterLog, "  Creating buffers file '%ls' failed!",
                  qUtf16PrintableImpl(binFileName));
        return false;
    }

    QJsonObject buffers;
    QJsonObject buffer;
    buffer["byteLength"] = m_buffer.size();
    buffer["type"] = QStringLiteral("arraybuffer");
    buffer["uri"] = bufName;
    buffers["buf"] = buffer;
    m_obj["buffers"] = buffers;

    QJsonObject bufferViews;
    for (auto bv : bvList) {
        QJsonObject bufferView;
        bufferView["buffer"] = QStringLiteral("buf");
        bufferView["byteLength"] = int(bv.length);
        bufferView["byteOffset"] = int(bv.offset);
        if (bv.target)
            bufferView["target"] = int(bv.target);
        bufferViews[bv.name] = bufferView;
    }
    if (bufferViews.size())
        m_obj["bufferViews"] = bufferViews;

    QJsonObject accessors;
    for (auto acc : accList) {
        QJsonObject accessor;
        accessor["bufferView"] = acc.bufferView;
        accessor["byteOffset"] = int(acc.offset);
        accessor["byteStride"] = int(acc.stride);
        accessor["count"] = int(acc.count);
        accessor["componentType"] = int(acc.componentType);
        accessor["type"] = acc.type;
        if (!acc.minVal.isEmpty() && !acc.maxVal.isEmpty()) {
            accessor["min"] = vec2jsvec(acc.minVal);
            accessor["max"] = vec2jsvec(acc.maxVal);
        }
        accessors[acc.name] = accessor;
    }
    if (accessors.size())
        m_obj["accessors"] = accessors;

    QJsonObject meshes;
    for (auto meshInfo : m_meshInfo.values()) {
        QJsonObject mesh;
        mesh["name"] = meshInfo.originalName;
        QJsonArray prims;
        QJsonObject prim;
        prim["mode"] = 4; // triangles
        QJsonObject attrs;
        for (auto acc : meshInfo.accessors) {
            if (acc.usage != QStringLiteral("INDEX"))
                attrs[acc.usage] = acc.name;
            else
                prim["indices"] = acc.name;
        }
        prim["attributes"] = attrs;
        prim["material"] = meshInfo.materialName;
        prims.append(prim);
        mesh["primitives"] = prims;
        meshes[meshInfo.name] = mesh;
    }
    if (meshes.size())
        m_obj["meshes"] = meshes;

    QJsonObject cameras;
    for (auto camInfo : m_cameraInfo.values()) {
        QJsonObject camera;
        QJsonObject proj;
        proj["znear"] = camInfo.znear;
        proj["zfar"] = camInfo.zfar;
        if (camInfo.perspective) {
            proj["aspect_ratio"] = camInfo.aspectRatio;
            proj["yfov"] = camInfo.yfov;
            camera["type"] = QStringLiteral("perspective");
            camera["perspective"] = proj;
        } else {
            proj["xmag"] = camInfo.xmag;
            proj["ymag"] = camInfo.ymag;
            camera["type"] = QStringLiteral("orthographic");
            camera["orthographic"] = proj;
        }
        camera["name"] = camInfo.originalName;
        cameras[camInfo.name] = camera;
    }
    if (cameras.size())
        m_obj["cameras"] = cameras;

    QJsonArray sceneNodes;
    QJsonObject nodes;
    if (m_rootNodeEmpty) {
        // Don't export the root node if it is there just to group the scene, so we don't get
        // an extra empty node when we import the scene back.
        for (auto c : m_rootNode->children)
            sceneNodes << exportNodes(c, nodes);
    } else {
        sceneNodes << exportNodes(m_rootNode, nodes);
    }
    m_obj["nodes"] = nodes;

    QJsonObject scenes;
    QJsonObject defaultScene;
    defaultScene["nodes"] = sceneNodes;
    scenes["defaultScene"] = defaultScene;
    m_obj["scenes"] = scenes;
    m_obj["scene"] = QStringLiteral("defaultScene");

    QJsonObject materials;
    QHash<QString, QString> textureNameMap;
    exportMaterials(materials, &textureNameMap);
    if (materials.size())
        m_obj["materials"] = materials;

    QJsonObject textures;
    QHash<QString, QString> imageKeyMap; // uri -> key
    for (auto it = textureNameMap.constBegin(); it != textureNameMap.constEnd(); ++it) {
        QJsonObject texture;
        if (!imageKeyMap.contains(it.key()))
            imageKeyMap[it.key()] = newImageName();
        texture["source"] = imageKeyMap[it.key()];
        texture["format"] = 0x1908; // RGBA
        texture["internalFormat"] = 0x1908;
        texture["sampler"] = QStringLiteral("sampler_mip_rep");
        texture["target"] = 3553; // TEXTURE_2D
        texture["type"] = 5121; // UNSIGNED_BYTE
        textures[it.value()] = texture;
    }
    if (textures.size()) {
        m_obj["textures"] = textures;
        QJsonObject samplers;
        QJsonObject sampler;
        sampler["magFilter"] = 9729; // LINEAR
        sampler["minFilter"] = 9987; // LINEAR_MIPMAP_LINEAR
        sampler["wrapS"] = 10497; // REPEAT
        sampler["wrapT"] = 10497;
        samplers["sampler_mip_rep"] = sampler;
        m_obj["samplers"] = samplers;
    }

    QJsonObject images;
    for (auto it = imageKeyMap.constBegin(); it != imageKeyMap.constEnd(); ++it) {
        QJsonObject image;
        image["uri"] = m_imageMap.value(it.key());
        images[it.value()] = image;
    }
    if (images.size())
        m_obj["images"] = images;

    // Lights must be declared as extensions to the top-level glTF object
    QJsonObject lights;
    for (auto lightInfo : m_lightInfo.values()) {
        QJsonObject light;
        QJsonObject lightDetails;
        QString type;
        if (lightInfo.type == QAbstractLight::SpotLight) {
            type = QStringLiteral("spot");
            lightDetails["falloffAngle"] = lightInfo.cutOffAngle;
        } else if (lightInfo.type == QAbstractLight::PointLight) {
            type = QStringLiteral("point");
        } else if (lightInfo.type == QAbstractLight::DirectionalLight) {
            type = QStringLiteral("directional");
        }
        light["type"] = type;
        if (lightInfo.type == QAbstractLight::SpotLight
                || lightInfo.type == QAbstractLight::DirectionalLight) {
            // The GLTF specs are bit unclear whether there is a direction parameter
            // for spot/directional lights, or are they supposed to just use the
            // parent transforms for direction, but we do need it in any case, so we add it.
            lightDetails["direction"] = vec2jsvec(lightInfo.direction);

        }
        if (lightInfo.type == QAbstractLight::SpotLight
                || lightInfo.type == QAbstractLight::PointLight) {
            lightDetails["constantAttenuation"] = lightInfo.attenuation.x();
            lightDetails["linearAttenuation"] = lightInfo.attenuation.y();
            lightDetails["quadraticAttenuation"] = lightInfo.attenuation.z();
        }
        lightDetails["color"] = col2jsvec(lightInfo.color, false);
        lightDetails["intensity"] = lightInfo.intensity; // Not in spec but needed
        light["name"] = lightInfo.originalName; // Not in spec but we want to pass the name anyway
        light[type] = lightDetails;
        lights[lightInfo.name] = light;
    }
    if (lights.size()) {
        QJsonObject extensions;
        QJsonObject common;
        common["lights"] = lights;
        extensions["KHR_materials_common"] = common;
        m_obj["extensions"] = extensions;
    }

    // TODO: Save techniques, programs, and shaders for custom materials

    m_doc.setObject(m_obj);

    QString gltfName = m_exportDir + m_exportName + QStringLiteral(".qgltf");
    f.setFileName(gltfName);
    qCDebug(GLTFExporterLog, "  Writing %sJSON file: '%ls'",
            m_gltfOpts.binaryJson ? "binary " : "", qUtf16PrintableImpl(gltfName));

    if (m_gltfOpts.binaryJson) {
        if (f.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            m_exportedFiles.insert(QFileInfo(f.fileName()).fileName());
            QByteArray json = m_doc.toBinaryData();
            f.write(json);
            f.close();
        } else {
            qCWarning(GLTFExporterLog, "  Writing binary JSON file '%ls' failed!",
                      qUtf16PrintableImpl(gltfName));
            return false;
        }
    } else {
        if (f.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
            m_exportedFiles.insert(QFileInfo(f.fileName()).fileName());
            QByteArray json = m_doc.toJson(m_gltfOpts.compactJson ? QJsonDocument::Compact
                                                                  : QJsonDocument::Indented);
            f.write(json);
            f.close();
        } else {
            qCWarning(GLTFExporterLog, "  Writing JSON file '%ls' failed!",
                      qUtf16PrintableImpl(gltfName));
            return false;
        }
    }

    QString qrcName = m_exportDir + m_exportName + QStringLiteral(".qrc");
    f.setFileName(qrcName);
    qCDebug(GLTFExporterLog, "Writing '%ls'", qUtf16PrintableImpl(qrcName));
    if (f.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        QByteArray pre = "<RCC><qresource prefix=\"/gltf_res\">\n";
        QByteArray post = "</qresource></RCC>\n";
        f.write(pre);
        for (auto file : m_exportedFiles) {
            QString line = QString(QStringLiteral("  <file>%1</file>\n")).arg(file);
            f.write(line.toUtf8());
        }
        f.write(post);
        f.close();
        m_exportedFiles.insert(QFileInfo(f.fileName()).fileName());
    } else {
        qCWarning(GLTFExporterLog, "  Creating qrc file '%ls' failed!",
                  qUtf16PrintableImpl(qrcName));
        return false;
    }

    qCDebug(GLTFExporterLog, "Saving done!");

    return true;
}

void GLTFExporter::delNode(GLTFExporter::Node *n)
{
    if (!n)
        return;
    for (auto *c : n->children)
        delNode(c);
    delete n;
}

QString GLTFExporter::exportNodes(GLTFExporter::Node *n, QJsonObject &nodes)
{
    QJsonObject node;
    node["name"] = n->name;
    QJsonArray children;
    for (auto c : n->children)
        children << exportNodes(c, nodes);
    node["children"] = children;
    if (auto transform = m_transformMap.value(n))
        node["matrix"] = matrix2jsvec(transform->matrix());

    if (auto mesh = m_meshMap.value(n)) {
        QJsonArray meshList;
        meshList.append(m_meshInfo.value(mesh).name);
        node["meshes"] = meshList;
    }

    if (auto camera = m_cameraMap.value(n))
        node["camera"] = m_cameraInfo.value(camera).name;

    if (auto light = m_lightMap.value(n)) {
        QJsonObject extensions;
        QJsonObject lights;
        lights["light"] = m_lightInfo.value(light).name;
        extensions["KHR_materials_common"] = lights;
        node["extensions"] = extensions;
    }

    nodes[n->uniqueName] = node;
    return n->uniqueName;
}

void GLTFExporter::exportMaterials(QJsonObject &materials, QHash<QString, QString> *textureNameMap)
{
    QHash<QString, bool> imageHasAlpha;

    QHashIterator<Qt3DRender::QMaterial *, MaterialInfo> matIt(m_materialInfo);
    for (auto matIt = m_materialInfo.constBegin(); matIt != m_materialInfo.constEnd(); ++matIt) {
        const MaterialInfo &matInfo = matIt.value();

        QJsonObject material;
        material["name"] = matInfo.originalName;

        bool opaque = true;
        QJsonObject vals;
        for (auto it = matInfo.textures.constBegin(); it != matInfo.textures.constEnd(); ++it) {
            if (!textureNameMap->contains(it.value()))
                textureNameMap->insert(it.value(), newTextureName());
            QString key = it.key();
            if (key == QStringLiteral("normal")) // avoid clashing with the vertex normals
                key = QStringLiteral("normalmap");
            // Alpha is supported for diffuse textures, but have to check the image data to decide
            // if blending is needed
            if (key == QStringLiteral("diffuse")) {
                QString imgFn = it.value();
                if (imageHasAlpha.contains(imgFn)) {
                    if (imageHasAlpha[imgFn])
                        opaque = false;
                } else {
                    QImage img(imgFn);
                    if (!img.isNull()) {
                        if (img.hasAlphaChannel()) {
                            for (int y = 0; opaque && y < img.height(); ++y) {
                                for (int x = 0; opaque && x < img.width(); ++x) {
                                    if (qAlpha(img.pixel(x, y)) < 255)
                                        opaque = false;
                                }
                            }
                        }
                        imageHasAlpha[imgFn] = !opaque;
                    } else {
                        qCWarning(GLTFExporterLog, "Cannot determine presence of alpha for '%ls'",
                                  qUtf16PrintableImpl(imgFn));
                    }
                }
            }
            vals[key] = textureNameMap->value(it.value());
        }
        for (auto it = matInfo.values.constBegin(); it != matInfo.values.constEnd(); ++it) {
            if (vals.contains(it.key()))
                continue;
            setVarToJSonObject(vals, it.key(), it.value());
        }
        for (auto it = matInfo.colors.constBegin(); it != matInfo.colors.constEnd(); ++it) {
            if (vals.contains(it.key()))
                continue;
            // Alpha is supported for the diffuse color. < 1 will enable blending.
            const bool alpha = it.key() == QStringLiteral("diffuse");
            if (alpha && it.value().alphaF() < 1.0f)
                opaque = false;
            vals[it.key()] = col2jsvec(it.value(), alpha);
        }
        if (matInfo.type == MaterialInfo::TypeCustom) {
            material["values"] = vals;
            // TODO: custom materials
        } else {
            // Material is a common material, so export it as such.
            QJsonObject commonMat;
            if (matInfo.type == MaterialInfo::TypeGooch)
                commonMat["technique"] = QStringLiteral("GOOCH"); // Qt3D specific extension
            else if (matInfo.type == MaterialInfo::TypePerVertex)
                commonMat["technique"] = QStringLiteral("PERVERTEX"); // Qt3D specific extension
            else
                commonMat["technique"] = QStringLiteral("PHONG");

            // Set the values as-is. "normalmap" is our own extension, not in the spec.
            // However, RGB colors have to be promoted to RGBA since the spec uses
            // vec4, and all types are pre-defined for common material values.
            promoteColorsToRGBA(&vals);
            if (!vals.isEmpty())
                commonMat["values"] = vals;

            // Blend function handling is our own extension used for Phong Alpha material.
            QJsonObject functions;
            if (!matInfo.blendEquations.isEmpty())
                functions["blendEquationSeparate"] = vec2jsvec(matInfo.blendEquations);
            if (!matInfo.blendArguments.isEmpty())
                functions["blendFuncSeparate"] = vec2jsvec(matInfo.blendArguments);
            if (!functions.isEmpty())
                commonMat["functions"] = functions;
            QJsonObject extensions;
            extensions["KHR_materials_common"] = commonMat;
            material["extensions"] = extensions;
        }

        materials[matInfo.name] = material;
    }
}

void GLTFExporter::clearOldExport(const QString &dir)
{
    // Look for .qrc file with same name
    QRegularExpression re(QStringLiteral("<file>(.*)</file>"));
    QFile qrcFile(dir + m_exportName + QStringLiteral(".qrc"));
    if (qrcFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        while (!qrcFile.atEnd()) {
            QByteArray line = qrcFile.readLine();
            QRegularExpressionMatch match = re.match(line);
            if (match.hasMatch()) {
                QString fileName = match.captured(1);
                QString filePathName = dir + fileName;
                QFile::remove(filePathName);
                qCDebug(GLTFExporterLog, "Removed old file: '%ls'",
                        qUtf16PrintableImpl(filePathName));
            }
        }
        qrcFile.close();
        qrcFile.remove();
        qCDebug(GLTFExporterLog, "Removed old file: '%ls'",
                qUtf16PrintableImpl(qrcFile.fileName()));
    }
}

QString GLTFExporter::newBufferViewName()
{
    return QString(QStringLiteral("bufferView_%1")).arg(++m_bufferViewCount);
}

QString GLTFExporter::newAccessorName()
{
    return QString(QStringLiteral("accessor_%1")).arg(++m_accessorCount);
}

QString GLTFExporter::newMeshName()
{
    return QString(QStringLiteral("mesh_%1")).arg(++m_meshCount);
}

QString GLTFExporter::newMaterialName()
{
    return QString(QStringLiteral("material_%1")).arg(++m_materialCount);
}

QString GLTFExporter::newTechniqueName()
{
    return QString(QStringLiteral("technique_%1")).arg(++m_techniqueCount);
}

QString GLTFExporter::newTextureName()
{
    return QString(QStringLiteral("texture_%1")).arg(++m_textureCount);
}

QString GLTFExporter::newImageName()
{
    return QString(QStringLiteral("image_%1")).arg(++m_imageCount);
}

QString GLTFExporter::newShaderName()
{
    return QString(QStringLiteral("shader_%1")).arg(++m_shaderCount);
}

QString GLTFExporter::newProgramName()
{
    return QString(QStringLiteral("program_%1")).arg(++m_programCount);
}

QString GLTFExporter::newNodeName()
{
    return QString(QStringLiteral("node_%1")).arg(++m_nodeCount);
}

QString GLTFExporter::newCameraName()
{
    return QString(QStringLiteral("camera_%1")).arg(++m_cameraCount);
}

QString GLTFExporter::newLightName()
{
    return QString(QStringLiteral("light_%1")).arg(++m_lightCount);
}

} // namespace Qt3DRender

QT_END_NAMESPACE

#include "moc_gltfexporter.cpp"
