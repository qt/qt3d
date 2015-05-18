/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Copyright (C) 2015 The Qt Company Ltd and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "gltfparser_p.h"

#include "texturedata.h"
#include <Qt3DRenderer/private/renderlogging_p.h>

#include <Qt3DCore/qentity.h>
#include <qmesh.h>
#include <qmaterial.h>
#include <qtechnique.h>
#include <qshaderprogram.h>
#include <qeffect.h>
#include <Qt3DCore/qcamera.h>
#include <Qt3DCore/qtransform.h>
#include <Qt3DCore/qmatrixtransform.h>
#include <Qt3DCore/qcameralens.h>
#include <Qt3DCore/private/qurlhelper_p.h>
#include <qtexture.h>
#include <qparameter.h>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QQuaternion>
#include <QMatrix4x4>
#include <QColor>
#include <QVector2D>
#include <QVector3D>
#include <QUrl>

// need to move these to somewhere common?
#include <Qt3DRenderer/private/renderstate_p.h>
#include <Qt3DRenderer/private/blendstate_p.h>
#include <private/qabstractmesh_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {
namespace {

const QString KEY_CAMERA     = QStringLiteral("camera");
const QString KEY_CAMERAS    = QStringLiteral("cameras");
const QString KEY_SCENES     = QStringLiteral("scenes");
const QString KEY_NODES      = QStringLiteral("nodes");
const QString KEY_MESHES     = QStringLiteral("meshes");
const QString KEY_CHILDREN   = QStringLiteral("children");
const QString KEY_MATRIX     = QStringLiteral("matrix");
const QString KEY_TYPE       = QStringLiteral("type");
const QString KEY_PERSPECTIVE =QStringLiteral("perspective");
const QString KEY_NAME       = QStringLiteral("name");
const QString KEY_COUNT      = QStringLiteral("count");
const QString KEY_YFOV       = QStringLiteral("yfov");
const QString KEY_ZNEAR      = QStringLiteral("znear");
const QString KEY_ZFAR       = QStringLiteral("zfar");
const QString KEY_MATERIALS  = QStringLiteral("materials");
const QString KEY_TECHNIQUE  = QStringLiteral("technique");
const QString KEY_VALUES     = QStringLiteral("values");
const QString KEY_BUFFERS    = QStringLiteral("buffers");
const QString KEY_SHADERS    = QStringLiteral("shaders");
const QString KEY_PROGRAMS   = QStringLiteral("programs");
const QString KEY_PROGRAM    = QStringLiteral("program");
const QString KEY_TECHNIQUES = QStringLiteral("techniques");
const QString KEY_ACCESSORS  = QStringLiteral("accessors");
const QString KEY_IMAGES     = QStringLiteral("images");
const QString KEY_TEXTURES   = QStringLiteral("textures");
const QString KEY_SCENE      = QStringLiteral("scene");
const QString KEY_BUFFER     = QStringLiteral("buffer");
const QString KEY_TARGET     = QStringLiteral("target");
const QString KEY_BYTE_OFFSET = QStringLiteral("byteOffset");
const QString KEY_BYTE_LENGTH = QStringLiteral("byteLength");
const QString KEY_BYTE_STRIDE = QStringLiteral("byteStride");
const QString KEY_PRIMITIVES = QStringLiteral("primitives");
const QString KEY_PRIMITIVE  = QStringLiteral("primitive");
const QString KEY_MATERIAL   = QStringLiteral("material");
const QString KEY_ATTRIBUTES = QStringLiteral("attributes");
const QString KEY_INDICES    = QStringLiteral("indices");
const QString KEY_PATH       = QStringLiteral("path");
const QString KEY_FORMAT     = QStringLiteral("format");
const QString KEY_PASSES     = QStringLiteral("passes");
const QString KEY_SOURCE     = QStringLiteral("source");
const QString KEY_SAMPLER    = QStringLiteral("sampler");
const QString KEY_SAMPLERS   = QStringLiteral("samplers");
const QString KEY_LENGTH     = QStringLiteral("length");
const QString KEY_SEMANTIC   = QStringLiteral("semantic");
const QString KEY_STATES     = QStringLiteral("states");
const QString KEY_UNIFORMS   = QStringLiteral("uniforms");
const QString KEY_PARAMETERS = QStringLiteral("parameters");
const QString KEY_WRAP_S     = QStringLiteral("wrapS");
const QString KEY_MIN_FILTER = QStringLiteral("minFilter");
const QString KEY_MAG_FILTER = QStringLiteral("magFilter");

const QString KEY_INSTANCE_TECHNIQUE = QStringLiteral("instanceTechnique");
const QString KEY_INSTANCE_PROGRAM = QStringLiteral("instanceProgram");
const QString KEY_BUFFER_VIEWS    = QStringLiteral("bufferViews");
const QString KEY_BUFFER_VIEW     = QStringLiteral("bufferView");
const QString KEY_VERTEX_SHADER   = QStringLiteral("vertexShader");
const QString KEY_FRAGMENT_SHADER = QStringLiteral("fragmentShader");
const QString KEY_INTERNAL_FORMAT = QStringLiteral("internalFormat");

//Parameter::StandardUniform parseSemanticName(const QByteArray& s)
//{
//    if (s == "MODEL")           return Parameter::ModelMatrix;
//    if (s == "VIEW")            return Parameter::ViewMatrix;
//    if (s == "PROJECTION")      return Parameter::ProjectionMatrix;
//    if (s == "MODELVIEW")       return Parameter::ModelView;
//    if (s == "MODELVIEWPROJECTION")
//        return Parameter::ModelViewProjection;

//    if (s == "MODELINVERSE")    return Parameter::ModelInverse;
//    if (s == "VIEWINVERSE")     return Parameter::ViewInverse;
//    if (s == "PROJECTIONINVERSE")
//        return Parameter::ProjectionInverse;
//    if (s == "MODELVIEWINVERSE")
//        return Parameter::ModelViewInverse;
//    if (s == "MODELVIEWPROJECTIONINVERSE")
//        return Parameter::ModelViewProjectionInverse;

//    if (s == "MODELINVERSETRANSPOSE")
//        return Parameter::ModelNormal;
//    if (s == "MODELVIEWINVERSETRANSPOSE")
//        return Parameter::ModelViewNormal;

//    return Parameter::None;
//}

//QParameter::OpenGLTypes parseType(const QByteArray &s)
//{
//    if (s == "BYTE")                  return QParameter::Undefined;
//    if (s == "BYTE_VEC2")             return QParameter::Undefined;
//    if (s == "BYTE_VEC3")             return QParameter::Undefined;
//    if (s == "BYTE_VEC4")             return QParameter::Undefined;
//    if (s == "UNSIGNED_BYTE")         return QParameter::Undefined;
//    if (s == "UNSIGNED_BYTE_VEC2")    return QParameter::Undefined;
//    if (s == "UNSIGNED_BYTE_VEC3")    return QParameter::Undefined;
//    if (s == "UNSIGNED_BYTE_VEC4")    return QParameter::Undefined;
//    if (s == "SHORT")                 return QParameter::Undefined;
//    if (s == "SHORT_VEC2")            return QParameter::Undefined;
//    if (s == "SHORT_VEC3")            return QParameter::Undefined;
//    if (s == "SHORT_VEC4")            return QParameter::Undefined;
//    if (s == "UNSIGNED_SHORT")        return QParameter::Undefined;
//    if (s == "UNSIGNED_SHORT_VEC2")   return QParameter::Undefined;
//    if (s == "UNSIGNED_SHORT_VEC3")   return QParameter::Undefined;
//    if (s == "UNSIGNED_SHORT_VEC4")   return QParameter::Undefined;
//    if (s == "FLOAT")                 return QParameter::Float;
//    if (s == "FLOAT_VEC2")            return QParameter::FloatVec2;
//    if (s == "FLOAT_VEC3")            return QParameter::FloatVec3;
//    if (s == "FLOAT_VEC4")            return QParameter::FloatVec4;
//    if (s == "FLOAT_MAT2")            return QParameter::FloatMat2;
//    if (s == "FLOAT_MAT2")            return QParameter::FloatMat3;
//    if (s == "FLOAT_MAT2")            return QParameter::FloatMat4;
//    if (s == "INT")                   return QParameter::Int;
//    if (s == "INT_VEC2")              return QParameter::IntVec2;
//    if (s == "INT_VEC3")              return QParameter::IntVec3;
//    if (s == "INT_VEC4")              return QParameter::IntVec4;
//    if (s == "BOOL")                  return QParameter::Bool;
//    if (s == "BOOL_VEC2")             return QParameter::BoolVec2;
//    if (s == "BOOL_VEC3")             return QParameter::BoolVec3;
//    if (s == "BOOL_VEC4")             return QParameter::BoolVec4;

//    return QParameter::Undefined;
//}

} // of anonymous namespace

class GLTFParserMeshPrivate;

class GLTFParserMesh : public QAbstractMesh
{
    Q_OBJECT
private:
    class GLTFParserMeshFunctor : public QAbstractMeshFunctor
    {
    public:
        explicit GLTFParserMeshFunctor(QMeshDataPtr meshData);
        QMeshDataPtr operator ()() Q_DECL_OVERRIDE;
        bool operator ==(const QAbstractMeshFunctor &other) const Q_DECL_OVERRIDE;

    private:
        QMeshDataPtr m_meshData;
    };

public:
    explicit GLTFParserMesh(QNode *parent = 0);

    void setData(QMeshDataPtr data);
    QAbstractMeshFunctorPtr meshFunctor() const Q_DECL_OVERRIDE;

protected:
    void copy(const QNode *ref) Q_DECL_OVERRIDE;

private:
    QT3D_CLONEABLE(GLTFParserMesh)
    Q_DECLARE_PRIVATE(GLTFParserMesh)
};

class GLTFParserMeshPrivate : public QAbstractMeshPrivate
{
public:
    GLTFParserMeshPrivate()
        : QAbstractMeshPrivate()
    {}

    Q_DECLARE_PUBLIC(GLTFParserMesh)
    QMeshDataPtr m_meshData;
};

GLTFParser::GLTFParser() : AbstractSceneParser(),
    m_parseDone(false)
{
}

bool GLTFParser::isGLTFPath(const QString& path)
{
    QFileInfo finfo(path);
    if (!finfo.exists())
        return false;

    // might need to detect other things in the future, but would
    // prefer to avoid doing a full parse.
    return (finfo.suffix().toLower() == QStringLiteral("json"));
}

void GLTFParser::setBasePath(const QString& path)
{
    m_basePath = path;
}

bool GLTFParser::setJSON( QJsonDocument json )
{
    if ( !json.isObject() ) {
        return false;
    }

    m_json = json;
    m_parseDone = false;

    m_meshDict.clear();
    m_attributeDict.clear();
    m_defaultScene.clear();

    return true;
}

/*!
 * Sets the \a path used by the parser to load the scene file.
 * If the file is valid, parsing is automatically triggered.
 */
void GLTFParser::setSource(const QUrl &source)
{
    const QString path = QUrlHelper::urlToLocalFileOrQrc(source);
    QFileInfo finfo(path);
    if (!finfo.exists()) {
        qWarning() << "missing file:" << path;
        return;
    }
    QFile f(path);
    f.open(QIODevice::ReadOnly);

    if (!setJSON(QJsonDocument::fromJson(f.readAll()))) {
        qWarning() << "not a JSON document";
        return;
    }

    setBasePath(finfo.dir().absolutePath());
}

/*!
 * Returns true if the extension of \a path is supported by the
 * GLTF parser.
 */
bool GLTFParser::isExtensionSupported(const QUrl &source) const
{
    const QString path = QUrlHelper::urlToLocalFileOrQrc(source);
    return GLTFParser::isGLTFPath(path);
}

QMeshDataPtr GLTFParser::mesh(const QString &id)
{
    parse();
    if (m_meshDict.contains(id))
        return m_meshDict.value(id);

    qWarning() << "Unknown mesh" << id << "in GLTF file" << m_basePath;
    return QMeshDataPtr();
}

QEntity* GLTFParser::defaultScene()
{
    parse();
    if (m_defaultScene.isEmpty()) {
        qWarning() << Q_FUNC_INFO << "no default scene";
        return NULL;
    }

    return scene(m_defaultScene);
}

QEntity* GLTFParser::scene(const QString &id)
{
    parse();

    QJsonObject scenes = m_json.object().value(KEY_SCENES).toObject();
    if (!scenes.contains(id)) {
        qWarning() << "GLTF: no such scene" << id << "in file" << m_basePath;
        return defaultScene();
    }

    QJsonObject sceneObj = scenes.value(id).toObject();
    QEntity* sceneEntity = new QEntity;
    foreach (QJsonValue nnv, sceneObj.value(KEY_NODES).toArray()) {
        QString nodeName = nnv.toString();
        QEntity* child = node(nodeName);
        if (!child)
            continue;
        child->setParent(sceneEntity);
    }

    return sceneEntity;
}

QEntity* GLTFParser::node(const QString &id)
{
    QJsonObject nodes = m_json.object().value(KEY_NODES).toObject();
    if (!nodes.contains(id)) {
        qWarning() << "unknown node" << id << "in GLTF file" << m_basePath;
        return NULL;
    }

    QJsonObject jsonObj = nodes.value(id).toObject();
    QEntity* result( new QEntity );
    parse();

    if ( jsonObj.contains(KEY_CHILDREN) )
    {
        foreach (QJsonValue c, jsonObj.value(KEY_CHILDREN).toArray()) {
            QEntity* child = node(c.toString());
            if (!child)
                continue;
            child->setParent(result);
        }
    }

    if ( jsonObj.contains(KEY_MESHES) )
    {
        typedef QList<GLTFParserMesh *> MeshList;
        QMap<QString, MeshList> materialDict;

        foreach (QJsonValue m, jsonObj.value(KEY_MESHES).toArray())
        {
            if (!m_meshDict.contains(m.toString())) {
                qWarning() << "node" << id << "references unknown mesh" << m.toString();
                continue;
            }

            foreach (QMeshDataPtr md, m_meshDict.values(m.toString())) {
                QString matId = m_meshMaterialDict[md.data()];
                GLTFParserMesh* meshComp = new GLTFParserMesh;
                meshComp->setData(md);
                materialDict[matId].append(meshComp);
            }
        }

        if (materialDict.size() == 1) {
            // common case
            result->addComponent(material(materialDict.firstKey()));
            foreach (GLTFParserMesh* m, materialDict.first())
                result->addComponent(m);
        } else {
            // need to make a child entity per material
            foreach (QString matId, materialDict.keys()) {
                QEntity* subEntity(new QEntity);
                subEntity->setParent(result);

                subEntity->addComponent(material(matId));
                foreach (GLTFParserMesh* m, materialDict.value(matId))
                    subEntity->addComponent(m);
            } // of distinct material iteration
        } // of multiple materials case
    }

    processName(jsonObj, result);

    if ( jsonObj.contains(KEY_MATRIX) )
    {
        QMatrix4x4 m(Qt::Uninitialized);

        QJsonArray matrixValues = jsonObj.value(KEY_MATRIX).toArray();
        for (int i=0; i<16; ++i) {
            double v = matrixValues.at( i ).toDouble();
            m(i % 4, i >> 2) = v;
        }

        // ADD MATRIX TRANSFORM COMPONENT TO ENTITY
        QTransform *trans = new QTransform();
        trans->addTransform(new QMatrixTransform(m));
        result->addComponent(trans);
    }

    if ( jsonObj.contains(KEY_CAMERA) )
    {
        QCameraLens* cam = camera( jsonObj.value(KEY_CAMERA).toString() );
        if (!cam) {
            qWarning() << "failed to build camera:" << jsonObj.value(KEY_CAMERA)
                       << "on node" << id;
        } else {
            result->addComponent(cam);
        }
    } // of have camera attribute

    return result;
}

// Get rid of miniwindef.h clobbering legal variable names
#if defined(near)
#undef near
#endif

#if defined(far)
#undef far
#endif

QCameraLens* GLTFParser::camera(const QString &id)
{
    parse();
    QJsonObject cams = m_json.object().value(KEY_CAMERAS).toObject();
    if (!cams.contains(id)) {
        qWarning() << "unknown camera" << id << "in GLTF file" << m_basePath;
        return Q_NULLPTR;
    }

    QJsonObject jsonObj = cams.value(id).toObject();
    QByteArray camTy = jsonObj.value(KEY_TYPE).toString().toUtf8();

    if (camTy == "perspective") {
        if (!jsonObj.contains(KEY_PERSPECTIVE)) {
            qWarning() << "camera:" << id << "missing 'perspective' object";
            return Q_NULLPTR;
        }

        QJsonObject pObj = jsonObj.value(KEY_PERSPECTIVE).toObject();
        double yfov = pObj.value(KEY_YFOV).toDouble();
        double near = pObj.value(KEY_ZNEAR).toDouble();
        double far = pObj.value(KEY_ZFAR).toDouble();

        QCameraLens* result = new QCameraLens;
        result->setPerspectiveProjection(yfov, 1.0, near, far);
        return result;
    } else if (camTy == "orthographic") {
        qWarning() << Q_FUNC_INFO << "implement me";

        return Q_NULLPTR;
    } else {
        qWarning() << "camera:" << id << "has unsupported type:" << camTy;
        return Q_NULLPTR;
    }
}

QMaterial* GLTFParser::material(const QString &id)
{
    parse();

    if (m_materialCache.contains(id))
        return m_materialCache.value(id);

    QJsonObject mats = m_json.object().value(KEY_MATERIALS).toObject();
    if (!mats.contains(id)) {
        qWarning() << "unknown material" << id << "in GLTF file" << m_basePath;
        return NULL;
    }

    QJsonObject jsonObj = mats.value(id).toObject();

    QJsonObject tech = jsonObj.value(KEY_INSTANCE_TECHNIQUE).toObject();
    QString tname = tech.value(KEY_TECHNIQUE).toString();
    if (!m_techniques.contains(tname)) {
        qWarning() << "unknown technique" << tname <<
                      "for material" << id << "in GLTF file" << m_basePath;
        return NULL;
    }

    QTechnique *technique = m_techniques.value(tname);
    if (!m_effectProxies.contains(tname)) {
        QEffect* eff = new QEffect;
        eff->setObjectName(tname);
        eff->addTechnique(technique);
        m_effectProxies[tname] = eff;
    }

    QMaterial* mat = new QMaterial;
    mat->setEffect(m_effectProxies[tname]);

    processName(jsonObj, mat);

//    QJsonObject values = tech.value(KEY_VALUES).toObject();
//    foreach (QString vName, values.keys()) {
//        QParameter* param = technique->parameterForName(vName);
//        if (!param) {
//            qWarning() << "unknown parameter:" << vName << "in technique" << tname
//                       << "processing material" << id;
//            continue;
//        }

//        if (param->isTextureType()) {
//            QString textureId = values.value(vName).toString();
//            if (!m_textures.contains(textureId)) {
//                qWarning() << "unknown texture" << textureId << "for parameter" << vName
//                           << "of material" << id;
//            } else {
//                mat->setTextureParameter(vName, m_textures.value(textureId));
//            }
//        } else {
//            //            QVariant var = parameterValueFromJSON(param, values.value(vName));
//            mat->addParameter(param);
//        }
//    } // of material technique-instance values iteration

    m_materialCache[id] = mat;
    return mat;
}

void GLTFParser::parse()
{
    if (m_parseDone)
        return;

    QJsonObject buffers = m_json.object().value(KEY_BUFFERS).toObject();
    foreach (QString nm, buffers.keys()) {
        processJSONBuffer( nm, buffers.value(nm).toObject() );
    }

    QJsonObject views = m_json.object().value(KEY_BUFFER_VIEWS).toObject();
    foreach (QString nm, views.keys()) {
        processJSONBufferView( nm, views.value(nm).toObject() );
    }

    QJsonObject shaders = m_json.object().value(KEY_SHADERS).toObject();
    foreach (QString nm, shaders.keys()) {
        processJSONShader( nm, shaders.value(nm).toObject() );
    }

    QJsonObject programs = m_json.object().value(KEY_PROGRAMS).toObject();
    foreach (QString nm, programs.keys()) {
        processJSONProgram( nm, programs.value(nm).toObject() );
    }

    QJsonObject techniques = m_json.object().value(KEY_TECHNIQUES).toObject();
    foreach (QString nm, techniques.keys()) {
        processJSONTechnique( nm, techniques.value(nm).toObject() );
    }

    QJsonObject attrs = m_json.object().value(KEY_ACCESSORS).toObject();
    foreach (QString nm, attrs.keys()) {
        processJSONAccessor( nm, attrs.value(nm).toObject() );
    }

    QJsonObject meshes = m_json.object().value(KEY_MESHES).toObject();
    foreach (QString nm, meshes.keys()) {
        processJSONMesh( nm, meshes.value(nm).toObject() );
    }

    QJsonObject images = m_json.object().value(KEY_IMAGES).toObject();
    foreach (QString nm, images.keys()) {
        processJSONImage( nm, images.value(nm).toObject() );
    }

    QJsonObject textures = m_json.object().value(KEY_TEXTURES).toObject();
    foreach (QString nm, textures.keys()) {
        processJSONTexture(nm, textures.value(nm).toObject() );
    }

    m_defaultScene = m_json.object().value(KEY_SCENE).toString();
    m_parseDone = true;
}

void GLTFParser::processJSONBuffer( QString id, const QJsonObject& json )
{
    // simply cache buffers for lookup by buffer-views
    m_bufferDatas[id] = BufferData(json);
}

void GLTFParser::processJSONBufferView( QString id, const QJsonObject& json )
{
    QString bufName = json.value(KEY_BUFFER).toString();
    if (!m_bufferDatas.contains(bufName)) {
        qWarning() << "unknown buffer:" << bufName << "processing view:" << id;
        return;
    }

    int target = json.value(KEY_TARGET).toInt();
    QOpenGLBuffer::Type ty(QOpenGLBuffer::VertexBuffer);

    switch (target) {
    case GL_ARRAY_BUFFER:           ty = QOpenGLBuffer::VertexBuffer; break;
    case GL_ELEMENT_ARRAY_BUFFER:   ty = QOpenGLBuffer::IndexBuffer; break;
    default:
        qWarning() << Q_FUNC_INFO << "buffer" << id << "unsupported target:" << target;
        return;
    }

    quint64 offset = 0;
    if (json.contains(KEY_BYTE_OFFSET)) {
        offset = json.value(KEY_BYTE_OFFSET).toInt();
        qCDebug(Render::Io) << "bv:" << id << "has offset:" << offset;
    }

    quint64 len = json.value(KEY_BYTE_LENGTH).toInt();
    QFile* f = resolveLocalData(m_bufferDatas[bufName].path);
    if (!f->seek(offset)) {
        qWarning() << "failed to seek to offset in file for bufferView:" << id << offset;
    }

    QByteArray bytes = f->read(len);
    if (bytes.count() != (int) len) {
        qWarning() << "failed to read sufficient bytes from:" << m_bufferDatas[bufName].path
                   << "for view" << id;
    }
    delete f;

    BufferPtr b(new Buffer(ty));
    b->setData(bytes);
    m_buffers[id] = b;
}

void GLTFParser::processJSONAccessor( QString id, const QJsonObject& json )
{
    QString bvName = json.value(KEY_BUFFER_VIEW).toString();
    if (!m_buffers.contains(bvName)) {
        qWarning() << "unknown buffer-view:" << bvName << "processing accessor:" << id;
        return;
    }

    BufferPtr buf = m_buffers.value(bvName);
    int offset = 0, stride = 0;
    int type = json.value(KEY_TYPE).toInt();
    int count = json.value(KEY_COUNT).toInt();

    if ( json.contains(KEY_BYTE_OFFSET))
        offset = json.value(KEY_BYTE_OFFSET).toInt();
    if ( json.contains(KEY_BYTE_STRIDE))
        stride = json.value(KEY_BYTE_STRIDE).toInt();

    AttributePtr attr( new Attribute( buf, type, count, offset, stride ) );
    m_attributeDict[id] = attr;
}

void GLTFParser::processJSONMesh( QString id, QJsonObject jsonObj )
{
    QJsonArray primsArray = jsonObj.value(KEY_PRIMITIVES).toArray();
    Q_FOREACH (QJsonValue primVal, primsArray) {
        QJsonObject primObj = primVal.toObject();
        int type = primObj.value(KEY_PRIMITIVE).toInt();
        QString material = primObj.value(KEY_MATERIAL).toString();

        if ( material.isEmpty()) {
            qCWarning(Render::Io) << "malformed primitive on " << id << ", missing material value"
                                  << material;
            continue;
        }

        QMeshDataPtr md(new QMeshData(static_cast<QMeshData::PrimitiveType>(type)));
        m_meshMaterialDict[md.data()] = material;

        QJsonObject attrs = primObj.value(KEY_ATTRIBUTES).toObject();
        Q_FOREACH (QString attrName, attrs.keys()) {
            QString k = attrs.value(attrName).toString();
            if (!m_attributeDict.contains(k)) {
                qCWarning(Render::Io) << "unknown attribute accessor:" << k << "on mesh" << id;
                continue;
            }

            md->addAttribute(attrName, m_attributeDict[k]);

            //  qCDebug(Render::rIo) << "DUMP of:" << attrName;
            //   m_attributeDict[k]->dump(20);
        }

        if ( primObj.contains(KEY_INDICES)) {
            QString k = primObj.value(KEY_INDICES).toString();
            if (!m_attributeDict.contains(k)) {
                qCWarning(Render::Io) << "unknown index accessor:" << k << "on mesh" << id;
            } else {
                md->setIndexAttribute(m_attributeDict[k]);
                //    m_attributeDict[k]->dump(100);
            }
        } // of has indices

        m_meshDict.insert( id, md );
    } // of primitives iteration
}

void GLTFParser::processName(const QJsonObject &json, QObject *ins)
{
    if ( json.contains(KEY_NAME) )
    {
        ins->setObjectName( json.value(KEY_NAME).toString() );
    }
}

void GLTFParser::processJSONProgram( QString id, QJsonObject jsonObj)
{
    QShaderProgram* prog = new QShaderProgram;
    prog->setObjectName(id);

    QString fragName = jsonObj.value(KEY_FRAGMENT_SHADER).toString(),
            vertName = jsonObj.value(KEY_VERTEX_SHADER).toString();
    if (!m_shaderPaths.contains(fragName) || !m_shaderPaths.contains(vertName)) {
        qCWarning(Render::Io) << Q_FUNC_INFO << "program:" << id << "missing shader:" <<
                                 fragName << vertName;
        return;
    }

    prog->setFragmentShaderCode(Qt3D::QShaderProgram::loadSource(QUrl(m_shaderPaths[fragName])));
    prog->setVertexShaderCode(Qt3D::QShaderProgram::loadSource(QUrl(m_shaderPaths[vertName])));
    m_programs[id] = prog;
}

void GLTFParser::processJSONShader( QString id, QJsonObject jsonObj)
{
    // shaders are trivial for the moment, defer the real work
    // to the program section
    QString path = jsonObj.value(KEY_PATH).toString();

    QFileInfo info(m_basePath, path);
    if (!info.exists()) {
        qCWarning(Render::Io) << "can't find shader" << id << "from path" << path;
        return;
    }

    m_shaderPaths[id] = info.absoluteFilePath();
}

void GLTFParser::processJSONImage( QString id, QJsonObject jsonObj)
{
    QString path = jsonObj.value(KEY_PATH).toString();
    QFileInfo info(m_basePath, path);
    if (!info.exists()) {
        qCWarning(Render::Io)<< "can't find image" << id << "from path" << path;
        return;
    }

    QImage img(info.absoluteFilePath());
    if (img.isNull()) {
        qCWarning(Render::Io) << "failed to load image:" << info.absoluteFilePath();
        return;
    }

    m_images[id] = img;
}

void GLTFParser::processJSONTexture( QString id, QJsonObject jsonObj)
{
    Q_UNUSED(id)
    Q_UNUSED(jsonObj)
//    int target = jsonObj.value(KEY_TARGET).toInt();
//    QAbstractTextureProvider* tex = new QAbstractTextureProvider(static_cast<QAbstractTextureProvider::Target>(target));


//    // TODO: Choose suitable internal format - may vary on OpenGL context type
//    //int pixelFormat = jsonObj.value(KEY_FORMAT).toInt();
//    //int internalFormat = jsonObj.value(KEY_INTERNAL_FORMAT).toInt();

//    tex->setFormat(QAbstractTextureProvider::RGBA8_UNorm /* static_cast<QOpenGLTexture::TextureFormat>(internalFormat)*/);

//    QString samplerId = jsonObj.value(KEY_SAMPLER).toString();
//    QString source = jsonObj.value(KEY_SOURCE).toString();
//    if (!m_images.contains(source)) {
//        qCWarning(Render::Io) << "texture" << id << "references missing image" << source;
//        return;
//    }

//    tex->setFromQImage(m_images[source]);

//    QJsonObject samplersDict(m_json.object().value(KEY_SAMPLERS).toObject());
//    if (!samplersDict.contains(samplerId)) {
//        qCWarning(Render::Io) << "texture" << id << "references unknown sampler" << samplerId;
//        return;
//    }

//    QJsonObject sampler = samplersDict.value(samplerId).toObject();

//    tex->setWrapMode(QTextureWrapMode(static_cast<QTextureWrapMode::WrapMode>(sampler.value(KEY_WRAP_S).toInt())));
//    tex->setMinificationFilter(static_cast<QAbstractTextureProvider::Filter>(sampler.value(KEY_MIN_FILTER).toInt()));
//    tex->setMagnificationFilter(static_cast<QAbstractTextureProvider::Filter>(sampler.value(KEY_MAG_FILTER).toInt()));

//    m_textures[id] = tex;
}

void GLTFParser::processJSONTechnique( QString id, QJsonObject jsonObj )
{
    QTechnique *t = new QTechnique;
    t->setObjectName(id);

    QHash<QString, QParameter*> paramDict;
    QJsonObject params = jsonObj.value(KEY_PARAMETERS).toObject();
    Q_FOREACH (QString pname, params.keys()) {
        QJsonObject po = params.value(pname).toObject();

        //        int dataType = po.value(KEY_TYPE).toInt();
        QString semantic = po.value(KEY_SEMANTIC).toString();
        // The Standard has changed, it doesn't return the raw int value for a type
        // But a string
//        QParameter* p = new QParameter(pname, parseType(po.value(KEY_TYPE).toString().toUtf8()), t);
//        Parameter::StandardUniform su = parseSemanticName(semantic.toUtf8());
//        if (su != Parameter::None) {
//            p->setStandardUniform(su);
//        } else {
//            // should really verify it's an attribute parameter?
//            // but what would be the way to do that?
//            // check the accessor dict?
//            p->setMeshAttributeName(semantic);
//        }

//        t->addParameter(p);

//        paramDict[pname] = p;
    } // of parameters iteration

    QJsonObject passes = jsonObj.value(KEY_PASSES).toObject();
    Q_FOREACH (QString pname, passes.keys()) {
        QJsonObject po = passes.value(pname).toObject();
        QJsonObject ip = po.value(KEY_INSTANCE_PROGRAM).toObject();

        QString programName = ip.value(KEY_PROGRAM).toString();
        if (!m_programs.contains(programName)) {
            qWarning() << Q_FUNC_INFO << "technique" << id << "pass" << pname
                       << ": missing program" << programName;
            continue;
        }

        QRenderPass* pass = new QRenderPass;
        pass->setShaderProgram(m_programs[programName]);

        QJsonObject attrs = ip.value(KEY_ATTRIBUTES).toObject();
        Q_FOREACH ( QString attrName, attrs.keys() ) {
            QString pname = attrs.value(attrName).toString();
            // TO DO : Correct that
            //            pass->addAttributeBinding(paramDict[pname], attrName);
        } // of program-instance attributes

        QJsonObject uniforms = ip.value(KEY_UNIFORMS).toObject();
        Q_FOREACH (QString uniformName, uniforms.keys()) {
            QString pname = uniforms.value(uniformName).toString();
            // TO DO : Correct that
            //            pass->addUniformBinding(paramDict[pname], uniformName);
        } // of program-instance attributes

        // TODO: Use public state api only here
//        QJsonObject states = po.value(KEY_STATES).toObject();
//        Render::RenderStateSet* ss = new Render::RenderStateSet;

//        Q_FOREACH (QString stateName, states.keys()) {
//            Render::RenderState* s= buildState(stateName.toUtf8(), states.value(stateName));
//            if (!s)
//                continue;

//            ss->addState(s);
//        } // of program-instance attributes

//        pass->setStateSet(ss);
        t->addPass(pass);
    } // of passes iteration

    m_techniques[id] = t;
}

//Render::RenderState* GLTFParser::buildState(const QByteArray& nm, QJsonValue v)
//{
//    if (nm == "blendEnable") {
//        return NULL; // will see a blendEquation spec too
//    }

//    if (nm == "blendFunc") {
//        QJsonObject obj =  v.toObject();
//        GLenum srcF = static_cast<GLenum>(obj.value(QStringLiteral("sfactor")).toInt());
//        GLenum dstF = static_cast<GLenum>(obj.value(QStringLiteral("dfactor")).toInt());
//        return Render::BlendState::getOrCreate(srcF, dstF);
//    }

//    if (nm == "blendEquation") {
//        return Render::BlendEquation::getOrCreate(static_cast<GLenum>(v.toInt()));
//    }

//    if (nm == "cullFaceEnable" && v.toInt()) {
//        return Render::CullFace::getOrCreate(GL_BACK);
//    }

//    if (nm == "depthTestEnable" && v.toInt()) {
//        return Render::DepthTest::getOrCreate(GL_LESS);
//    }

//    if (nm == "depthMask") {
//        return Render::DepthMask::getOrCreate(v.toInt() ? GL_TRUE : GL_FALSE);
//    }

//    qCWarning(Render::Io) << Q_FUNC_INFO << "unsupported gltf state:" << nm;
//    return NULL;
//}

QFile *GLTFParser::resolveLocalData(QString path)
{
    QDir d(m_basePath);
    Q_ASSERT(d.exists());

    QString absPath = d.absoluteFilePath(path);
    QFile* f = new QFile(absPath);
    f->open(QIODevice::ReadOnly);
    return f;
}

GLTFParser::BufferData::BufferData() :
    length(0)
{
}

GLTFParser::BufferData::BufferData(QJsonObject json)
{
    path = json.value(KEY_PATH).toString();
    length = json.value(KEY_LENGTH).toInt();
    // url
}

QVariant GLTFParser::parameterValueFromJSON(QParameter* p, QJsonValue val)
{
    Q_UNUSED(p);
    Q_UNUSED(val);
//    switch (p->datatype()) {
//    case QParameter::Bool:
//        return val.toBool();

//    case QParameter::Float:
//        return val.toDouble();

//    case QParameter::FloatVec2: {
//        QJsonArray a = val.toArray();
//        return QVector2D(a[0].toDouble(), a[1].toDouble());
//    }

//    case QParameter::FloatVec3: {
//        QJsonArray a = val.toArray();
//        return QVector3D(a[0].toDouble(), a[1].toDouble(), a[3].toDouble());
//    }

//    case QParameter::FloatVec4: {
//        QJsonArray a = val.toArray();
//        return QVector4D(a[0].toDouble(),
//                a[1].toDouble(),
//                a[2].toDouble(),
//                a[3].toDouble());
//    }

//    case QParameter::FloatMat4: {
//        QJsonArray a = val.toArray();

//        QMatrix4x4 m(Qt::Uninitialized);
//        for (int i=0; i<16; ++i) {
//            m(i % 4, i / 4) = a[i].toDouble();
//        }
//        return m;
//    }

//    default:
//        qCWarning(Render::Io) << Q_FUNC_INFO << "unhandled type:" << QString::number(p->datatype(), 16);
//    }

    return QVariant();
}

void GLTFParserMesh::copy(const QNode *ref)
{
    QAbstractMesh::copy(ref);
    const GLTFParserMesh *gltfMesh = static_cast<const GLTFParserMesh*>(ref);
    d_func()->m_meshData = gltfMesh->d_func()->m_meshData;
}

GLTFParserMesh::GLTFParserMesh(QNode *parent)
    : QAbstractMesh(*new GLTFParserMeshPrivate, parent)
{
}


void GLTFParserMesh::setData(QMeshDataPtr data)
{
   Q_D(GLTFParserMesh);
   d->m_meshData = data;
   QAbstractMesh::update();
}

QAbstractMeshFunctorPtr GLTFParserMesh::meshFunctor() const
{
    Q_D(const GLTFParserMesh);
    return QAbstractMeshFunctorPtr(new GLTFParserMeshFunctor(d->m_meshData));
}

GLTFParserMesh::GLTFParserMeshFunctor::GLTFParserMeshFunctor(QMeshDataPtr meshData)
    : QAbstractMeshFunctor()
    , m_meshData(meshData)
{
}

QMeshDataPtr GLTFParserMesh::GLTFParserMeshFunctor::operator ()()
{
    return m_meshData;
}

bool GLTFParserMesh::GLTFParserMeshFunctor::operator ==(const QAbstractMeshFunctor &) const
{
    return false;
}

} // namespace Qt3D

QT_END_NAMESPACE

#include "gltfparser.moc"
