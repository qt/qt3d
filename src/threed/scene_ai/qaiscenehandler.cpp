/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
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

#include "qaiscenehandler_p.h"
#include "qaiscene_p.h"
#include "qdownloadmanager.h"
#include "ailoaderiosystem_p.h"

#include "aiScene.h"
#include "aiPostProcess.h"
#include "DefaultLogger.h"

#include <QtCore/qdir.h>
#include <QtCore/qdebug.h>
#include <QObject>
#include <QBuffer>


#define qAiPostProcessPreset ( \
    aiProcess_CalcTangentSpace           | \
    aiProcess_GenSmoothNormals           | \
    aiProcess_JoinIdenticalVertices      | \
    aiProcess_ImproveCacheLocality       | \
    aiProcess_LimitBoneWeights           | \
    aiProcess_RemoveRedundantMaterials   | \
    aiProcess_SplitLargeMeshes           | \
    aiProcess_Triangulate                | \
    aiProcess_GenUVCoords                | \
    aiProcess_SortByPType                | \
    aiProcess_FindDegenerates            | \
    aiProcess_FindInvalidData )

QT_BEGIN_NAMESPACE

void DumpScene(const aiScene* pScene);

QAiSceneHandler::QAiSceneHandler()
    : m_options(qAiPostProcessPreset)
    , m_showWarnings(false)
    , m_mayHaveLinesPoints(false)
    , m_meshSplitVertexLimit(2000)
    , m_meshSplitTriangleLimit(2000)
    , m_removeComponentFlags(0)
    , m_removeSortFlags(0)
{
    // by default remove per vertex colors from the data - no-one uses that in
    // models - if they need it it can be turned on with UseVertexColors
    m_removeComponentFlags |= aiComponent_COLORS;

    // by default remove points and lines from the model, since these are usually
    // degenerate structures from bad modelling or bad import/export.  if they
    // are needed it can be turned on with IncludeLinesPoints
    m_removeSortFlags |= aiPrimitiveType_POINT | aiPrimitiveType_LINE;
}

QAiSceneHandler::~QAiSceneHandler()
{
    // nothing to do
}

void QAiSceneHandler::decodeOptions(const QString &options)
{
    if (options.isEmpty())
        return;

    // See aiPostProcess.h for aiProcessPreset_TargetRealtime_Quality
    // - a useful default set of values - its exactly what we want but
    // wont compile with flags, so redefined with the above macro.
    // Also, allow the user to override some settings
    m_options = qAiPostProcessPreset;

    // Has to match the enum
    static const char *validOptions[] = {
        "NoOptions",
        "ShowWarnings",
        "CalculateNormals",
        "ForceFaceted",
        "ForceSmooth",
        "IncludeAllMaterials",
        "IncludeLinesPoints",
        "FixNormals",
        "DeDupMeshes",
        "Optimize",
        "FlipUVs",
        "FlipWinding",
        "UseVertexColors",
        "VertexSplitLimitx2",
        "TriangleSplitLimitx2",
        0
    };

    QStringList opList = options.simplified().split(QLatin1Char(' '), QString::SkipEmptyParts);

    for (int i = 0; i < opList.count(); ++i)
    {
        QString op = opList.at(i);
        op = op.trimmed();
        int k = 0;
        for ( ; validOptions[k]; ++k)
            if (op == QString::fromLatin1(validOptions[k]))
                break;
        if (validOptions[k]) // found
        {
            Options o = static_cast<Options>(k);
            switch (o)
            {
            case NoOptions:
                break;
            case ShowWarnings:
                if (m_showWarnings)
                {
                    m_importer.SetExtraVerbose(true);
                }
                else
                {
                    m_showWarnings = true;
                    m_options |= aiProcess_ValidateDataStructure;
                }
                break;
            case CalculateNormals:
                m_removeComponentFlags |= aiComponent_NORMALS;
                m_options |= aiProcess_GenSmoothNormals;
                m_options &= ~aiProcess_GenNormals;
                break;
            case ForceFaceted:
                m_removeComponentFlags |= aiComponent_NORMALS;
                m_options |= aiProcess_GenNormals;
                m_options &= ~aiProcess_GenSmoothNormals;
                m_options &= ~aiProcess_JoinIdenticalVertices;
                break;
            case ForceSmooth:
                Assimp::DefaultLogger::get()->warn("ForceSmooth is deprecated - ignoring (meshes now smooth by default)");
                break;
            case IncludeAllMaterials:
                m_options &= ~aiProcess_RemoveRedundantMaterials;
                break;
            case IncludeLinesPoints:
                m_removeSortFlags &= ~(aiPrimitiveType_LINE | aiPrimitiveType_POINT);
                m_mayHaveLinesPoints = true;
                // leave it with the FindDegenerates option turned on - we want zero
                // area triangles to display as proper GL lines or points
                break;
            case FixNormals:
                m_options |= aiProcess_FixInfacingNormals;
                break;
            case DeDupMeshes:
                m_options |= aiProcess_FindInstances;
                break;
            case Optimize:
                m_options |= aiProcess_OptimizeGraph | aiProcess_OptimizeMeshes;
                break;
            case FlipUVs:
                m_options |= aiProcess_FlipUVs;
                break;
            case FlipWinding:
                m_options |= aiProcess_FlipWindingOrder;
                break;
            case UseVertexColors:
                m_removeComponentFlags &= ~aiComponent_COLORS;
                break;
            case VertexSplitLimitx2:
                m_meshSplitVertexLimit <<= 1;
                // repeating this in the option string more than once works...
                break;
            case TriangleSplitLimitx2:
                // ....and we're OK with that, just don't overdo it
                m_meshSplitTriangleLimit <<= 1;
                break;
            }
        }
        else
        {
            qWarning("Bad option: \"%s\" in %s", qPrintable(op),
                     qPrintable(options));
        }
    }
}

QStringList QAiSceneHandler::supportedFormats() {
    QStringList result;
    result.reserve(60);
    result.append(QStringLiteral("3d"));
    result.append(QStringLiteral("3ds"));
    result.append(QStringLiteral("ac"));
    result.append(QStringLiteral("ac3d"));
    result.append(QStringLiteral("acc"));
    result.append(QStringLiteral("ase"));
    result.append(QStringLiteral("ask"));
    result.append(QStringLiteral("b3d"));
    result.append(QStringLiteral("blend"));
    result.append(QStringLiteral("bvh"));
    result.append(QStringLiteral("cob"));
    result.append(QStringLiteral("csm"));
    result.append(QStringLiteral("dae"));
    result.append(QStringLiteral("dxf"));
    result.append(QStringLiteral("enff"));
    result.append(QStringLiteral("hmp"));
    result.append(QStringLiteral("irr"));
    result.append(QStringLiteral("irrmesh"));
    result.append(QStringLiteral("lwo"));
    result.append(QStringLiteral("lws"));
    result.append(QStringLiteral("lxo"));
    result.append(QStringLiteral("md2"));
    result.append(QStringLiteral("md3"));
    result.append(QStringLiteral("md5anim"));
    result.append(QStringLiteral("md5camera"));
    result.append(QStringLiteral("md5mesh"));
    result.append(QStringLiteral("mdc"));
    result.append(QStringLiteral("mdl"));
    result.append(QStringLiteral("mesh.xml"));
    result.append(QStringLiteral("mot"));
    result.append(QStringLiteral("ms3d"));
    result.append(QStringLiteral("ndo"));
    result.append(QStringLiteral("nff"));
    result.append(QStringLiteral("obj"));
    result.append(QStringLiteral("off"));
    result.append(QStringLiteral("pk3"));
    result.append(QStringLiteral("ply"));
    result.append(QStringLiteral("prj"));
    result.append(QStringLiteral("q3o"));
    result.append(QStringLiteral("q3s"));
    result.append(QStringLiteral("raw"));
    result.append(QStringLiteral("scn"));
    result.append(QStringLiteral("smd"));
    result.append(QStringLiteral("stl"));
    result.append(QStringLiteral("ter"));
    result.append(QStringLiteral("uc"));
    result.append(QStringLiteral("vta"));
    result.append(QStringLiteral("x"));
    result.append(QStringLiteral("xml"));
    return result;
}

QGLAbstractScene *QAiSceneHandler::read()
{
    AiLoaderIOSystem *ios = new AiLoaderIOSystem(device(), url());
    m_importer.SetIOHandler(ios);

    Assimp::Logger *log = 0;
    Assimp::Logger::LogSeverity severity = Assimp::Logger::NORMAL;
    if (m_showWarnings)
    {
        severity = Assimp::Logger::VERBOSE;
        int streams = aiDefaultLogStream_FILE |
#ifdef Q_CC_MSVC
                aiDefaultLogStream_DEBUGGER
#else
                aiDefaultLogStream_STDERR
#endif
                ;
        log = Assimp::DefaultLogger::create("AssimpLog.txt", severity, streams);
    }

    QString path;
    QUrl u = url();
    if (u.scheme() != QLatin1String("file"))
    {
        path = QLatin1String(u.toEncoded());
    } else {
        path = u.toLocalFile();
    }

    if (m_removeComponentFlags)
        m_options |= aiProcess_RemoveComponent;
    else
        m_options &= ~aiProcess_RemoveComponent;

    m_importer.SetPropertyInteger(AI_CONFIG_PP_RVC_FLAGS, m_removeComponentFlags);
    m_importer.SetPropertyInteger(AI_CONFIG_PP_SBP_REMOVE, m_removeSortFlags);
    //m_importer.SetPropertyInteger(AI_CONFIG_PP_SLM_VERTEX_LIMIT, m_meshSplitVertexLimit);
    //m_importer.SetPropertyInteger(AI_CONFIG_PP_SLM_TRIANGLE_LIMIT, m_meshSplitTriangleLimit);

    // force this on, and provide no way to turn it off.  Its set by the
    // aiProcessPreset_TargetRealtime_Quality option in the constructor.
    // Guarantees that all meshes only have one primitive type
    Q_ASSERT(m_options & aiProcess_SortByPType);

    // the importer owns the scene, so when the it goes out of scope on exiting
    // this function the scene will get destroyed
    const aiScene* scene = m_importer.ReadFile(path.toLatin1().constData(), m_options);

    if (!scene)
    {
        // Notes on import success flags - according to assimp doco if validation
        // is requested the flags AI_SCENE_FLAGS_VALIDATION_WARNING will be set
        // if there's a warning, and AI_SCENE_FLAGS_VALIDATED is set on success.
        // This does not happen.  Also AI_SCENE_FLAGS_INCOMPLETE can be set on a
        // valid model, so checking for that is no use either.  Best way to proceed
        // is that if ShowWarnings is turned on above, then any pertinent warnings
        // will be shown; and if a NULL result is returned here, then its a fatal
        // error and a message is shown here.  If a non-NULL result is returned
        // just go ahead and try to load it.
        QString c = QDir::current().absolutePath();
        qWarning("Asset importer error: %s\n", m_importer.GetErrorString());
        if (log)
            qWarning("For details check log: %s/AssimpLog.txt\n", qPrintable(c));
        return 0;
    }
    else {
        if (!qgetenv("QT3D_DEBUG_MODEL").isEmpty())
            DumpScene(scene);
    }

    QAiScene *qscene = new QAiScene(scene, this);

    Assimp::DefaultLogger::kill();

    finalize();
    return qscene;
}

void LogPrint(const char* pFormat, ...)
{
    static char buff[1024];
    buff[0] = '\0';
    va_list args;
    va_start(args, pFormat);
    vsnprintf( buff, sizeof(buff) - 1, pFormat, args);
#ifndef QT_NO_DEBUG_STREAM
    qDebug() << buff;
#endif
    va_end(args);
}

void DumpAnimation(int i, const aiAnimation* pAnimation, const aiScene* pScene)
{
    Q_UNUSED(pScene);
    LogPrint("  Animation %d",i);
    LogPrint("    name = '%s'",pAnimation->mName.data);
    LogPrint("    ticks =         %f",pAnimation->mDuration);
    LogPrint("    ticks per sec = %f",pAnimation->mTicksPerSecond);
    for (unsigned int i=0; i<pAnimation->mNumChannels; ++i) {
        const aiNodeAnim* pNodeAnim = pAnimation->mChannels[i];
        LogPrint("    channel %d affects: '%s'", i, pNodeAnim->mNodeName.data);
    }
}

void DumpMesh(int i, const aiMesh* pMesh, const aiScene* pScene)
{
    Q_UNUSED(pScene);
    LogPrint("  Mesh %d",i);
    LogPrint("    name = '%s'",pMesh->mName.data);
    LogPrint("    has bones: %s (%d)",pMesh->HasBones()?"YES":"no ",pMesh->mNumBones);
    for (unsigned int i=0; i<pMesh->mNumBones; ++i) {
        LogPrint("      bone(%02d): '%s'", i, pMesh->mBones[i]->mName.data);
    }
}
void DumpNodeRecursive(int level, const aiNode* pNode, const aiScene* pScene)
{
    if (level > 100)
        level = 100;
    char Prefix[256];
    memset(Prefix,' ',level*2);
    Prefix[level*2] = '\0';
    LogPrint("  %sNode, name='%s', nMeshes=%d",Prefix,pNode->mName.data,pNode->mNumMeshes);
    for (unsigned int i=0; i<pNode->mNumChildren; ++i) {
        DumpNodeRecursive(level+1, pNode->mChildren[i], pScene);
    }
}

void DumpScene(const aiScene* pScene)
{
    LogPrint("DumpScene(%p)",pScene);
    if (pScene) {
        LogPrint("  has animations: %s (%d)",pScene->HasAnimations()?"YES":"no ",pScene->mNumAnimations);
        LogPrint("  has meshes:     %s (%d)",pScene->HasMeshes()?"YES":"no ",pScene->mNumMeshes);
        for (unsigned int i=0; i<pScene->mNumAnimations; ++i) {
            DumpAnimation(i,pScene->mAnimations[i],pScene);
        }
        for (unsigned int i=0; i<pScene->mNumMeshes; ++i) {
            DumpMesh(i,pScene->mMeshes[i],pScene);
        }
        DumpNodeRecursive(0,pScene->mRootNode,pScene);
    }
}

QGLAbstractScene * QAiSceneHandler::download()
{
    QUrl u = url();

    if (u.scheme() == QLatin1String("file")) {
        qWarning() << "Cannot download urls with FILE scheme.  Use the read() method.";
    } else {
        if (m_removeComponentFlags)
            m_options |= aiProcess_RemoveComponent;
        else
            m_options &= ~aiProcess_RemoveComponent;

        m_importer.SetPropertyInteger(AI_CONFIG_PP_RVC_FLAGS, m_removeComponentFlags);
        m_importer.SetPropertyInteger(AI_CONFIG_PP_SBP_REMOVE, m_removeSortFlags);
        m_importer.SetPropertyInteger(AI_CONFIG_PP_SLM_VERTEX_LIMIT, m_meshSplitVertexLimit);
        m_importer.SetPropertyInteger(AI_CONFIG_PP_SLM_TRIANGLE_LIMIT, m_meshSplitTriangleLimit);

        // force this on, and provide no way to turn it off.  Its set by the
        // aiProcessPreset_TargetRealtime_Quality option in the constructor.
        // Guarantees that all meshes only have one primitive type
        Q_ASSERT(m_options & aiProcess_SortByPType);

        setScene(new QAiScene(this));
        //m_scene = new QAiScene(this);

        //m_scene->begindownLoad(url());

        downloadScene();
    }
    return getScene();
}

void QAiSceneHandler::downloadComplete(QByteArray *sceneData)
{
    //Create i/o device to use for file reading.
    QBuffer sceneBuffer(sceneData);
    sceneBuffer.open(QIODevice::ReadOnly);
    setDevice(&sceneBuffer);

    //Set i/o system to use non-file based device.
    AiLoaderIOSystem *ios = new AiLoaderIOSystem(device(), url());
    m_importer.SetIOHandler(ios);

    QString path;
    path = QLatin1String(url().toEncoded());

    Assimp::Logger *log = 0;
    Assimp::Logger::LogSeverity severity = Assimp::Logger::NORMAL;
    if (m_showWarnings)
    {
        severity = Assimp::Logger::VERBOSE;
        int streams = aiDefaultLogStream_FILE |
#ifdef Q_CC_MSVC
                aiDefaultLogStream_DEBUGGER
#else
                aiDefaultLogStream_STDERR
#endif
                ;
        log = Assimp::DefaultLogger::create("AssimpLog.txt", severity, streams);
    }

    const aiScene* scene = m_importer.ReadFile(path.toStdString(), m_options);
    if (!scene)
    {
        // Notes on import success flags - according to assimp doco if validation
        // is requested the flags AI_SCENE_FLAGS_VALIDATION_WARNING will be set
        // if there's a warning, and AI_SCENE_FLAGS_VALIDATED is set on success.
        // This does not happen.  Also AI_SCENE_FLAGS_INCOMPLETE can be set on a
        // valid model, so checking for that is no use either.  Best way to proceed
        // is that if ShowWarnings is turned on above, then any pertinent warnings
        // will be shown; and if a NULL result is returned here, then its a fatal
        // error and a message is shown here.  If a non-NULL result is returned
        // just go ahead and try to load it.
        QString c = QDir::current().absolutePath();
        qWarning("Asset importer error: %s\n", m_importer.GetErrorString());
        if (log)
            qWarning("For details check log: %s/AssimpLog.txt\n", qPrintable(c));
    } else {
        //If we have reached this point everything has proceeded correctly,
        //load the scene.
        QAiScene *theScene = qobject_cast<QAiScene*>(getScene());
        theScene->loadScene(scene);
    }

    Assimp::DefaultLogger::kill();

    delete sceneData;
    finalize();
}

QT_END_NAMESPACE
