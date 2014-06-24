/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "assimpparser.h"

#include <Qt3DCore/transform.h>
#include <parameter.h>
#include <Qt3DCore/matrixtransform.h>
#include <Qt3DCore/cameralens.h>
#include <mesh.h>
#include <QFileInfo>
#include <QColor>
#include <qmath.h>
#include <Qt3DCore/entity.h>
#include <material.h>
#include <mesh.h>
#include <texture.h>
#include "renderlogging.h"

QT_BEGIN_NAMESPACE

namespace Qt3D {

/*!
 * \class AssimpParser
 * \namespace Qt3D
 * \since 5.3
 *
 * \brief Provides a generic way of loading various 3D assets
 * format into a Qt3D scene.
 *
 * It should be noted that Assimp aiString is explicitly defined to be UTF-8.
 *
 * \sa GLTFParser
*/

namespace {

const QString ASSIMP_MATERIAL_DIFFUSE_COLOR = QStringLiteral("diffuse");
const QString ASSIMP_MATERIAL_SPECULAR_COLOR = QStringLiteral("specular");
const QString ASSIMP_MATERIAL_AMBIENT_COLOR = QStringLiteral("ambient");
const QString ASSIMP_MATERIAL_EMISSIVE_COLOR = QStringLiteral("emissive");
const QString ASSIMP_MATERIAL_TRANSPARENT_COLOR = QStringLiteral("transparent");
const QString ASSIMP_MATERIAL_REFLECTIVE_COLOR = QStringLiteral("reflective");

const QString ASSIMP_MATERIAL_IS_TWOSIDED = QStringLiteral("twosided");
const QString ASSIMP_MATERIAL_IS_WIREFRAME = QStringLiteral("wireframe");

const QString ASSIMP_MATERIAL_OPACITY = QStringLiteral("opacity");
const QString ASSIMP_MATERIAL_SHININESS = QStringLiteral("shininess");
const QString ASSIMP_MATERIAL_SHININESS_STRENGTH = QStringLiteral("shininess_strength");
const QString ASSIMP_MATERIAL_REFRACTI = QStringLiteral("refracti");
const QString ASSIMP_MATERIAL_REFLECTIVITY = QStringLiteral("reflectivity");

const QString ASSIMP_MATERIAL_NAME = QStringLiteral("name");

const QString VERTICES_ATTRIBUTE_NAME = QStringLiteral("position");
const QString NORMAL_ATTRIBUTE_NAME = QStringLiteral("normal");
const QString TANGENT_ATTRIBUTE_NAME = QStringLiteral("tangent");
const QString TEXTCOOD_ATTRIBUTE_NAME = QStringLiteral("textcoord");

}

QStringList AssimpParser::assimpSupportedFormatsList = AssimpParser::assimpSupportedFormats();

/*!
 * Returns a QStringlist with the suffixes of the various supported asset formats.
 */
QStringList AssimpParser::assimpSupportedFormats()
{
    QStringList formats;

    formats.reserve(60);
    formats.append(QStringLiteral("3d"));
    formats.append(QStringLiteral("3ds"));
    formats.append(QStringLiteral("ac"));
    formats.append(QStringLiteral("ac3d"));
    formats.append(QStringLiteral("acc"));
    formats.append(QStringLiteral("ase"));
    formats.append(QStringLiteral("ask"));
    formats.append(QStringLiteral("b3d"));
    formats.append(QStringLiteral("blend"));
    formats.append(QStringLiteral("bvh"));
    formats.append(QStringLiteral("cob"));
    formats.append(QStringLiteral("csm"));
    formats.append(QStringLiteral("dae"));
    formats.append(QStringLiteral("dxf"));
    formats.append(QStringLiteral("enff"));
    formats.append(QStringLiteral("hmp"));
    formats.append(QStringLiteral("irr"));
    formats.append(QStringLiteral("irrmesh"));
    formats.append(QStringLiteral("lwo"));
    formats.append(QStringLiteral("lws"));
    formats.append(QStringLiteral("lxo"));
    formats.append(QStringLiteral("md2"));
    formats.append(QStringLiteral("md3"));
    formats.append(QStringLiteral("md5anim"));
    formats.append(QStringLiteral("md5camera"));
    formats.append(QStringLiteral("md5mesh"));
    formats.append(QStringLiteral("mdc"));
    formats.append(QStringLiteral("mdl"));
    formats.append(QStringLiteral("mesh.xml"));
    formats.append(QStringLiteral("mot"));
    formats.append(QStringLiteral("ms3d"));
    formats.append(QStringLiteral("ndo"));
    formats.append(QStringLiteral("nff"));
    formats.append(QStringLiteral("obj"));
    formats.append(QStringLiteral("off"));
    formats.append(QStringLiteral("pk3"));
    formats.append(QStringLiteral("ply"));
    formats.append(QStringLiteral("prj"));
    formats.append(QStringLiteral("q3o"));
    formats.append(QStringLiteral("q3s"));
    formats.append(QStringLiteral("raw"));
    formats.append(QStringLiteral("scn"));
    formats.append(QStringLiteral("smd"));
    formats.append(QStringLiteral("stl"));
    formats.append(QStringLiteral("ter"));
    formats.append(QStringLiteral("uc"));
    formats.append(QStringLiteral("vta"));
    formats.append(QStringLiteral("x"));
    formats.append(QStringLiteral("xml"));

    return formats;
}

/*!
 *  Initialized a new instance of AssimpParser.
 */
AssimpParser::AssimpParser() : AbstractSceneParser(),
    m_aiScene(0),
    m_sceneParsed(false)
{
}

/*!
 * Cleans the parser properly before destruction.
 */
AssimpParser::~AssimpParser()
{
    cleanup();
}

/*!
 *  Return true if the provided \a path has a suffix supported
 *  by the Assimp Assets importer.
 */
bool AssimpParser::isAssimpPath(const QString& path)
{
    QFileInfo fileInfo(path);

    if (!fileInfo.exists() ||
            !AssimpParser::assimpSupportedFormats().contains(fileInfo.suffix().toLower()))
        return false;
    return true;
}

/*!
 * Returns a QMatrix4x4 from \a matrix;
 */
QMatrix4x4 AssimpParser::aiMatrix4x4ToQMatrix4x4(const aiMatrix4x4 &matrix)
{
    return QMatrix4x4(matrix.a1, matrix.a2, matrix.a3, matrix.a4,
                      matrix.b1, matrix.b2, matrix.b3, matrix.b4,
                      matrix.c1, matrix.c2, matrix.c3, matrix.c4,
                      matrix.d1, matrix.d2, matrix.d3, matrix.d4);
}

/*!
 * Sets the \a path used by the parser to load the asset file.
 * If the file is valid, this will trigger the parsing of the file.
 */
void AssimpParser::setFilePath(const QString& path)
{
    QFileInfo file(path);
    m_sceneDir = file.absoluteDir();
    if (!file.exists()) {
        qCWarning(Render::Io) << "File missing " << path;
        return ;
    }
    readSceneFile(path);
}

/*!
 * Returns true if the extension of \a path is supported by
 * the assimp parser.
 */
bool AssimpParser::isPathExtensionSupported(const QString &path)
{
    return AssimpParser::isAssimpPath(path);
}

/*!
 * Returns a Entity node which is the root node of the scene
 * node specified by \a id. If \a id is empty, the scene is assumed to be
 * the root node of the scene.
 *
 * Returns Q_NULLPTR if \a id was specified but not node matching it can be found.
 */
Entity *AssimpParser::scene(QString id)
{
    // m_aiScene shouldn't be null.
    // If it is either, the file failed to be imported or
    // setFilePath was not called
    if (m_aiScene.isNull())
        return Q_NULLPTR;

    aiNode *rootNode = m_aiScene->mRootNode;
    // if id specified, tries to find node
    if (!id.isEmpty() &&
            !(rootNode = rootNode->FindNode(id.toUtf8().constData()))) {
        qCDebug(Render::Io) << Q_FUNC_INFO << " Couldn't find requested scene node";
        return Q_NULLPTR;
    }

    // Builds the Qt3D scene using the Assimp aiScene
    // and the various dicts filled previously by parse
    return node(rootNode);
}

/*!
 *  Returns a Node from the scene identified by \a id.
 *  Returns Q_NULLPTR if no node can be found.
 */
Entity *AssimpParser::node(QString id)
{
    if (m_aiScene.isNull())
        return Q_NULLPTR;
    parse();
    aiNode *n = m_aiScene->mRootNode->FindNode(id.toUtf8().constData());
    return node(n);
}

/*!
 * Returns a Node from an Assimp aiNode \a node.
 */
Entity *AssimpParser::node(aiNode *node)
{
    if (node == Q_NULLPTR)
        return Q_NULLPTR;
    Entity *entityNode = new Entity();

    // Add Meshes to the node
    for (uint i = 0; i < node->mNumMeshes; i++) {
        uint meshIdx = node->mMeshes[i];
        Mesh * mesh = m_meshes[meshIdx];
        // mesh material
        if (m_materials.contains(meshIdx))
            entityNode->addComponent(m_materials[meshIdx]);
        // mesh
        entityNode->addComponent(mesh);
    }

    // Add Children to Node
    for (uint i = 0; i < node->mNumChildren; i++) {
        // this-> is necessary here otherwise
        // it conflicts with the variable node
        Entity *child = this->node(node->mChildren[i]);
        if (child != Q_NULLPTR)
            entityNode->addChild(child);
    }

    // Add Transformations
    Transform *transform = new Transform();
    QMatrix4x4 qTransformMatrix = AssimpParser::aiMatrix4x4ToQMatrix4x4(node->mTransformation);
    transform->setMatrix(qTransformMatrix);
    entityNode->addComponent(transform);

    // Add Camera
    if (m_cameras.contains(node))
        entityNode->addChild(m_cameras[node]);

    // TO DO : Add lights ....

    return entityNode;
}

/*!
 * Reads the scene file pointed by \a path and launches the parsing of
 * the scene using Assimp after having cleaned up previously saved values
 * from eventual previous parsings.
 */
void AssimpParser::readSceneFile(const QString &path)
{
    Assimp::Importer importer;
    Assimp::DefaultLogger::create("AssimpLog.txt", Assimp::Logger::VERBOSE);

    // SET THIS TO REMOVE POINTS AND LINES -> HAVE ONLY TRIANGLES
    importer.SetPropertyInteger(AI_CONFIG_PP_SBP_REMOVE, aiPrimitiveType_LINE|aiPrimitiveType_POINT);
    // SET CUSTOM FILE HANDLER TO HANDLE FILE READING THROUGH QT (RESOURCES, SOCKET ...)
    importer.SetIOHandler(new AssimpHelper::AssimpIOSystem());

    // type and aiProcess_Triangulate discompose polygons with more than 3 points in triangles
    // aiProcess_SortByPType makes sur that meshes data are triangles
    importer.ReadFile(path.toUtf8().constData(),
                      aiProcess_SortByPType|
                      aiProcess_Triangulate|
                      aiProcess_JoinIdenticalVertices|
                      aiProcess_GenSmoothNormals);
    Assimp::DefaultLogger::kill();
    cleanup();
    aiScene *scene = Q_NULLPTR;
    if (!(scene = importer.GetOrphanedScene())) {
        qCWarning(Render::Io) << "Assimp scene import failed";
        return ;
    }
    m_aiScene.reset(scene);
    parse();
}

/*!
 * Cleans the various dictionnaries holding the scenes infos.
 */
void AssimpParser::cleanup()
{
    m_meshes.clear();
    m_effects.clear();
    m_embeddedTextures.clear();
    m_materialTextures.clear();
    m_cameras.clear();
    m_sceneParsed = false;
    if (!m_aiScene.isNull())
        m_aiScene.reset(0);
}

/*!
 * Parses the the aiScene provided py Assimp and convert Assimp
 * values to Qt3D values.
 */
void AssimpParser::parse()
{
    if (!m_sceneParsed) {
        // Set parsed flags
        m_sceneParsed = !m_sceneParsed;

        for (uint i = 0; i < m_aiScene->mNumTextures; i++)
            loadEmbeddedTexture(i);
        for (uint i = 0; i < m_aiScene->mNumMaterials; i++)
            loadMaterial(i);
        for (uint i = 0; i < m_aiScene->mNumMeshes; i++)
            loadMesh(i);
        for (uint i = 0; i < m_aiScene->mNumCameras; i++)
            loadCamera(i);
        for (uint i = 0; i < m_aiScene->mNumLights; i++)
            loadLight(i);
        for (uint i = 0; i < m_aiScene->mNumAnimations; i++)
            loadAnimation(i);
    }
}

/*!
 * Converts the provided Assimp aiMaterial \a material to a Qt3D material.
 * \sa Material and adds its to a dictionary of materials.
 */
void AssimpParser::loadMaterial(uint materialIndex)
{
    Material *material = new Material();
    aiMaterial *assimpMaterial = m_aiScene->mMaterials[materialIndex];
    // Material Name
    copyMaterialName(material, assimpMaterial);
    copyMaterialColorProperties(material, assimpMaterial);
    copyMaterialBoolProperties(material, assimpMaterial);
    copyMaterialFloatProperties(material, assimpMaterial);

    // Add textures to materials dict
    copyMaterialTextures(material, assimpMaterial);

    m_materials[materialIndex] = material;
}

/*!
 * Converts the Assimp aiMesh mesh identified by \a meshIndex to a MeshData
 * \sa MeshData and adds it to a dictionary of meshes.
 */
void AssimpParser::loadMesh(uint meshIndex)
{
    aiMesh *mesh = m_aiScene->mMeshes[meshIndex];

    // Primitive are always triangles with the current Assimp's configuration
    MeshDataPtr meshData(new MeshData(GL_TRIANGLES));

    // Mesh Name
    QString meshName = QString::fromUtf8(mesh->mName.data);

    // Vertices and Normals always present with the current Assimp's configuration
    aiVector3D *vertices = mesh->mVertices;
    aiVector3D *normals = mesh->mNormals;
    // Tangents and TextureCoord not always present
    bool hasTangent = mesh->HasTangentsAndBitangents();
    bool hasTexture = mesh->HasTextureCoords(0);
    aiVector3D *tangents = hasTangent ? mesh->mTangents : Q_NULLPTR;
    aiVector3D *textureCoord = hasTexture ? mesh->mTextureCoords[0] : Q_NULLPTR;

    // Add values in raw float array
    ushort chunkSize = 6 + (hasTangent ? 3 : 0) + (hasTexture ? 2 : 0);
    QByteArray bufferArray;
    bufferArray.resize(chunkSize * mesh->mNumVertices * sizeof(float));
    float *vbufferContent = reinterpret_cast<float*>(bufferArray.data());
    for (uint i = 0; i < mesh->mNumVertices; i++) {
        uint idx = i * chunkSize;
        // position
        vbufferContent[idx] = vertices[i].x;
        vbufferContent[idx + 1] = vertices[i].y;
        vbufferContent[idx + 2] = vertices[i].z;
        // normals
        vbufferContent[idx + 3] = normals[i].x;
        vbufferContent[idx + 4] = normals[i].y;
        vbufferContent[idx + 5] = normals[i].z;

        if (hasTangent) {
            vbufferContent[idx + 6] = tangents[i].x;
            vbufferContent[idx + 7] = tangents[i].y;
            vbufferContent[idx + 8] = tangents[i].z;
        }
        if (hasTexture) {
            char offset =  (hasTangent ? 9 : 6);
            vbufferContent[idx + offset] = textureCoord[i].x;
            vbufferContent[idx + offset + 1] = textureCoord[i].y;
        }
    }
    // Create a Buffer from the raw array
    BufferPtr vbuffer(new Buffer(QOpenGLBuffer::VertexBuffer));
    vbuffer->setUsage(QOpenGLBuffer::StaticDraw);
    vbuffer->setData(bufferArray);

    // Add vertex attributes to the mesh with the right array
    meshData->addAttribute(VERTICES_ATTRIBUTE_NAME,
                           new Attribute(vbuffer,
                                         GL_FLOAT_VEC3,
                                         mesh->mNumVertices,
                                         0,
                                         chunkSize * sizeof(float)));
    meshData->addAttribute(NORMAL_ATTRIBUTE_NAME,
                           new Attribute(vbuffer,
                                         GL_FLOAT_VEC3,
                                         mesh->mNumVertices,
                                         3 * sizeof(float),
                                         chunkSize * sizeof(float)));
    if (hasTangent)
        meshData->addAttribute(TANGENT_ATTRIBUTE_NAME,
                               new Attribute(vbuffer,
                                             GL_FLOAT_VEC3,
                                             mesh->mNumVertices,
                                             6 * sizeof(float),
                                             chunkSize * sizeof(float)));
    if (hasTexture)
        meshData->addAttribute(TEXTCOOD_ATTRIBUTE_NAME,
                               new Attribute(vbuffer,
                                             GL_FLOAT_VEC2,
                                             mesh->mNumVertices,
                                             (hasTangent ? 9 : 6) * sizeof(float),
                                             chunkSize * sizeof(float)));
    GLuint indiceType;
    QByteArray ibufferContent;
    uint indices = mesh->mNumFaces * 3;
    // If there are less than 65535 indices, indices can then fit in ushort
    // which saves video memory
    if (indices >= USHRT_MAX) {
        indiceType = GL_UNSIGNED_INT;
        ibufferContent.resize(indices * sizeof(quint32));
        for (uint i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];
            Q_ASSERT(face.mNumIndices == 3);
            memcpy(&reinterpret_cast<quint32*>(ibufferContent.data())[i * 3], face.mIndices, 3 * sizeof(uint));
        }
    }
    else {
        indiceType = GL_UNSIGNED_SHORT;
        ibufferContent.resize(indices * sizeof(quint16));
        for (uint i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];
            Q_ASSERT(face.mNumIndices == 3);
            for (ushort j = 0; j < face.mNumIndices; j++)
                reinterpret_cast<quint16*>(ibufferContent.data())[i * 3 + j] = face.mIndices[j];
        }
    }

    // Create Indices buffer
    BufferPtr ibuffer(new Buffer(QOpenGLBuffer::IndexBuffer));
    ibuffer->setUsage(QOpenGLBuffer::StaticDraw);
    ibuffer->setData(ibufferContent);

    // Add indices attributes
    meshData->setIndexAttr(AttributePtr(new Attribute(ibuffer, indiceType, indices, 0, 0)));

    meshData->computeBoundsFromAttribute(VERTICES_ATTRIBUTE_NAME);

    Mesh *storedMesh = new Mesh();
    storedMesh->setData(meshData);
    m_meshes[meshIndex] = storedMesh;

    qCDebug(Render::Io) << Q_FUNC_INFO << " Mesh " << meshName << " Vertices " << mesh->mNumVertices << " Faces " << mesh->mNumFaces << " Indices " << indices;
}

/*!
 * Converts the provided Assimp aiTexture at \a textureIndex to a Texture and
 * adds it ot a dictionary of textures.
 * \sa Texture
 */
void AssimpParser::loadEmbeddedTexture(uint textureIndex)
{
    aiTexture *assimpTexture = m_aiScene->mTextures[textureIndex];
    Texture *texture = new Texture();
    TexImageDataPtr textureData(new TexImageData(0, 0));

    bool isCompressed = assimpTexture->mHeight == 0;
    uint textureSize = assimpTexture->mWidth *
            (isCompressed ? assimpTexture->mHeight : 1);
    // Set texture to RGBA8888
    char *textureContent = new char[textureSize * 4];
    for (uint i = 0; i < textureSize; i++) {
        uint idx = i * 4;
        aiTexel texel = assimpTexture->pcData[i];
        textureContent[idx] = texel.r;
        textureContent[idx + 1] = texel.g;
        textureContent[idx + 2] = texel.b;
        textureContent[idx + 3] = texel.a;
    }
    textureData->setData(QByteArray(textureContent, textureSize * 4),
                         QOpenGLTexture::RGBA,
                         QOpenGLTexture::UInt8);
    texture->addImageData(textureData);
    m_embeddedTextures[textureIndex] = texture;
}

/*!
 * Loads the light in the current scene located at \a lightIndex.
 */
void AssimpParser::loadLight(uint lightIndex)
{
    aiLight *light = m_aiScene->mLights[lightIndex];
    // TODO: Implement me!
    Q_UNUSED(light);
}

/*!
 * Parses the camera at cameraIndex and saves it to a dictionary of cameras.
 */
void AssimpParser::loadCamera(uint cameraIndex)
{
    aiCamera *assimpCamera = m_aiScene->mCameras[cameraIndex];
    aiNode *cameraNode = m_aiScene->mRootNode->FindNode(assimpCamera->mName);

    // If no node is associated to the camera in the scene, camera not saved
    if (cameraNode == Q_NULLPTR)
        return ;

    Entity  *camera = new Entity();
    CameraLens *lens = new CameraLens();
    aiMatrix4x4 cm;

    assimpCamera->GetCameraMatrix(cm);
    lens->setObjectName(QString::fromUtf8(assimpCamera->mName.data));
    lens->setPerspectiveProjection(qRadiansToDegrees(assimpCamera->mHorizontalFOV),
                                   qMax(assimpCamera->mAspect, 1.0f),
                                   assimpCamera->mClipPlaneNear,
                                   assimpCamera->mClipPlaneFar);
    camera->addComponent(lens);
    // View Matrix defines camera position & up vector relative to the associated
    // node in the scene. This is computed in AssimpParser::node
    //    camera->lookAt()->setViewMatrix();
    Transform *transform = new Transform();
    QMatrix4x4 viewMatrix = AssimpParser::aiMatrix4x4ToQMatrix4x4(cm);
    // CHECK THAT THIS WORKS
    qCDebug(Render::Io) << Q_FUNC_INFO << "IF CAMERA NOT BEHAVING CORRECTLY LOOK HERE";
    viewMatrix.lookAt(QVector3D(0, 0, 0),
                      QVector3D(assimpCamera->mLookAt.x, assimpCamera->mLookAt.y, assimpCamera->mLookAt.z),
                      QVector3D(0, 0, 0));
    transform->setMatrix(viewMatrix);
    camera->addComponent(transform);
    m_cameras[cameraNode] = camera;
}

// OPTIONAL
void AssimpParser::loadAnimation(uint animationIndex)
{
    Q_UNUSED(animationIndex)
}

/*!
 *  Sets the object name of \a material to the name of \a assimpMaterial.
 */
void AssimpParser::copyMaterialName(Material *material, aiMaterial *assimpMaterial)
{
    aiString name;
    if (assimpMaterial->Get(AI_MATKEY_NAME, name) == aiReturn_SUCCESS) {
        // May not be necessary
        // Kept for debug purposes at the moment
        material->setObjectName(QString::fromUtf8(name.data));
        qCDebug(Render::Io) << Q_FUNC_INFO << "Assimp Material " << material->objectName();
    }
}

/*!
 *  Fills \a material color properties with \a assimpMaterial color properties;
 */
void AssimpParser::copyMaterialColorProperties(Material *material, aiMaterial *assimpMaterial)
{
    aiColor3D color;
    if (assimpMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, color) == aiReturn_SUCCESS)
        material->addParameter(new Parameter(material, ASSIMP_MATERIAL_DIFFUSE_COLOR, QColor::fromRgbF(color.r, color.g, color.b)));
    if (assimpMaterial->Get(AI_MATKEY_COLOR_SPECULAR, color) == aiReturn_SUCCESS)
        material->addParameter(new Parameter(material, ASSIMP_MATERIAL_SPECULAR_COLOR, QColor::fromRgbF(color.r, color.g, color.b)));
    if (assimpMaterial->Get(AI_MATKEY_COLOR_AMBIENT, color) == aiReturn_SUCCESS)
        material->addParameter(new Parameter(material, ASSIMP_MATERIAL_AMBIENT_COLOR, QColor::fromRgbF(color.r, color.g, color.b)));
    if (assimpMaterial->Get(AI_MATKEY_COLOR_EMISSIVE, color) == aiReturn_SUCCESS)
        material->addParameter(new Parameter(material, ASSIMP_MATERIAL_EMISSIVE_COLOR, QColor::fromRgbF(color.r, color.g, color.b)));
    if (assimpMaterial->Get(AI_MATKEY_COLOR_TRANSPARENT, color) == aiReturn_SUCCESS)
        material->addParameter(new Parameter(material, ASSIMP_MATERIAL_TRANSPARENT_COLOR, QColor::fromRgbF(color.r, color.g, color.b)));
    if (assimpMaterial->Get(AI_MATKEY_COLOR_REFLECTIVE, color) == aiReturn_SUCCESS)
        material->addParameter(new Parameter(material, ASSIMP_MATERIAL_REFLECTIVE_COLOR, QColor::fromRgbF(color.r, color.g, color.b)));
}

/*!
 * Retrieves a\a material bool property;
 */
void AssimpParser::copyMaterialBoolProperties(Material *material, aiMaterial *assimpMaterial)
{
    int value;
    if (assimpMaterial->Get(AI_MATKEY_TWOSIDED, value) == aiReturn_SUCCESS)
        material->addParameter(new Parameter(material, ASSIMP_MATERIAL_IS_TWOSIDED, (value == 0) ? false : true));
    if (assimpMaterial->Get(AI_MATKEY_ENABLE_WIREFRAME, value) == aiReturn_SUCCESS)
        material->addParameter(new Parameter(material, ASSIMP_MATERIAL_IS_WIREFRAME, (value == 0) ? false : true));
}

void AssimpParser::copyMaterialShadingModel(Material *material, aiMaterial *assimpMaterial)
{
    Q_UNUSED(material)
    Q_UNUSED(assimpMaterial)
    // TODO
    // Match each shading function with a default shader

    //    AssimpParser::assimpMaterialAttributesMap[AI_MATKEY_SHADING_MODEL] = &AssimpParser::getMaterialShadingModel;
    //    AssimpParser::assimpMaterialAttributesMap[AI_MATKEY_BLEND_FUNC] = &AssimpParser::getMaterialBlendingFunction;
}

void AssimpParser::copyMaterialBlendingFunction(Material *material, aiMaterial *assimpMaterial)
{
    Q_UNUSED(material)
    Q_UNUSED(assimpMaterial)
    // TO DO
}

/*!
 *
 */
void AssimpParser::copyMaterialTextures(Material *material, aiMaterial *assimpMaterial)
{
    static const aiTextureType textureType[] = {aiTextureType_AMBIENT,
                                                aiTextureType_DIFFUSE,
                                                aiTextureType_DISPLACEMENT,
                                                aiTextureType_EMISSIVE,
                                                aiTextureType_HEIGHT,
                                                aiTextureType_LIGHTMAP,
                                                aiTextureType_NORMALS,
                                                aiTextureType_OPACITY,
                                                aiTextureType_REFLECTION,
                                                aiTextureType_SHININESS,
                                                aiTextureType_SPECULAR,
                                                aiTextureType_UNKNOWN};
    for (uint i = 0; i < sizeof(textureType); i++) {
        aiString path;
        if (assimpMaterial->GetTexture(textureType[i], 0, &path) == AI_SUCCESS) {
            QString fullPath = m_sceneDir.absoluteFilePath(QString::fromUtf8(path.data));
            // Load texture if not already loaded
            bool textureLoaded = true;
            if (!m_materialTextures.contains(fullPath)) {
                Texture *tex = new Texture();
                QImage textureImage(fullPath);
                if (!textureImage.isNull()) {
                    tex->setFromQImage(textureImage);
                    m_materialTextures[fullPath] = tex;
                    qWarning() << Q_FUNC_INFO << " Loaded Texture " << fullPath;
                }
                else {
                    qWarning() << Q_FUNC_INFO << " Invalid Texture " << fullPath;
                    textureLoaded = false;
                }
            }
            if (textureLoaded)
                material->setTextureParameter(fullPath, m_materialTextures[fullPath]);
        }
    }
}

/*!
 * Retrieves a\a material float property.
 */
void AssimpParser::copyMaterialFloatProperties(Material *material, aiMaterial *assimpMaterial)
{
    float value = 0;
    if (assimpMaterial->Get(AI_MATKEY_OPACITY, value) == aiReturn_SUCCESS)
        material->addParameter(new Parameter(material, ASSIMP_MATERIAL_OPACITY, value));
    if (assimpMaterial->Get(AI_MATKEY_SHININESS, value) == aiReturn_SUCCESS)
        material->addParameter(new Parameter(material, ASSIMP_MATERIAL_SHININESS,value));
    if (assimpMaterial->Get(AI_MATKEY_SHININESS_STRENGTH, value) == aiReturn_SUCCESS)
        material->addParameter(new Parameter(material, ASSIMP_MATERIAL_SHININESS_STRENGTH, value));
    if (assimpMaterial->Get(AI_MATKEY_REFRACTI, value) == aiReturn_SUCCESS)
        material->addParameter(new Parameter(material, ASSIMP_MATERIAL_REFRACTI, value));
    if (assimpMaterial->Get(AI_MATKEY_REFLECTIVITY, value) == aiReturn_SUCCESS)
        material->addParameter(new Parameter(material, ASSIMP_MATERIAL_REFLECTIVITY, value));
}

} // Qt3D

QT_END_NAMESPACE
