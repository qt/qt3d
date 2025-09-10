// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qskyboxentity.h"
#include "qskyboxentity_p.h"

#include <QtCore/qtimer.h>
#include <Qt3DRender/qfilterkey.h>
#include <Qt3DRender/qeffect.h>
#include <Qt3DRender/qtexture.h>
#include <Qt3DRender/qmaterial.h>
#include <Qt3DRender/qcullface.h>
#include <Qt3DRender/qdepthtest.h>
#include <Qt3DRender/qparameter.h>
#include <Qt3DRender/qtechnique.h>
#include <Qt3DExtras/qcuboidmesh.h>
#include <Qt3DRender/qrenderpass.h>
#include <Qt3DRender/qgraphicsapifilter.h>
#include <Qt3DRender/qseamlesscubemap.h>
#include <Qt3DRender/qshaderprogram.h>
#include <Qt3DRender/qgeometryrenderer.h>

QT_BEGIN_NAMESPACE


namespace Qt3DExtras {

using namespace Qt3DCore;
using namespace Qt3DRender;

QSkyboxEntityPrivate::QSkyboxEntityPrivate()
    : QEntityPrivate()
    , m_effect(new QEffect())
    , m_material(new QMaterial())
    , m_skyboxTexture(new QTextureCubeMap())
    , m_loadedTexture(new QTextureLoader())
    , m_gl3Shader(new QShaderProgram())
    , m_gl2es2Shader(new QShaderProgram())
    , m_rhiShader(new QShaderProgram())
    , m_gl2Technique(new QTechnique())
    , m_es2Technique(new QTechnique())
    , m_gl3Technique(new QTechnique())
    , m_rhiTechnique(new QTechnique())
    , m_filterKey(new QFilterKey)
    , m_gl2RenderPass(new QRenderPass())
    , m_es2RenderPass(new QRenderPass())
    , m_gl3RenderPass(new QRenderPass())
    , m_rhiRenderPass(new QRenderPass())
    , m_mesh(new QCuboidMesh())
    , m_gammaStrengthParameter(new QParameter(QStringLiteral("gammaStrength"), 0.0f))
    , m_textureParameter(new QParameter(QStringLiteral("skyboxTexture"), m_skyboxTexture))
    , m_posXImage(new QTextureImage())
    , m_posYImage(new QTextureImage())
    , m_posZImage(new QTextureImage())
    , m_negXImage(new QTextureImage())
    , m_negYImage(new QTextureImage())
    , m_negZImage(new QTextureImage())
    , m_extension(QStringLiteral(".png"))
    , m_hasPendingReloadTextureCall(false)
{
    m_loadedTexture->setGenerateMipMaps(false);
}

/*!
 * \internal
 */
void QSkyboxEntityPrivate::init()
{
    m_gl3Shader->setVertexShaderCode(QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/shaders/gl3/skybox.vert"))));
    m_gl3Shader->setFragmentShaderCode(QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/shaders/gl3/skybox.frag"))));
    m_gl2es2Shader->setVertexShaderCode(QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/shaders/es2/skybox.vert"))));
    m_gl2es2Shader->setFragmentShaderCode(QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/shaders/es2/skybox.frag"))));
    m_rhiShader->setVertexShaderCode(QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/shaders/rhi/skybox.vert"))));
    m_rhiShader->setFragmentShaderCode(QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/shaders/rhi/skybox.frag"))));

    m_gl3Technique->graphicsApiFilter()->setApi(QGraphicsApiFilter::OpenGL);
    m_gl3Technique->graphicsApiFilter()->setMajorVersion(3);
    m_gl3Technique->graphicsApiFilter()->setMinorVersion(3);
    m_gl3Technique->graphicsApiFilter()->setProfile(QGraphicsApiFilter::CoreProfile);

    m_gl2Technique->graphicsApiFilter()->setApi(QGraphicsApiFilter::OpenGL);
    m_gl2Technique->graphicsApiFilter()->setMajorVersion(2);
    m_gl2Technique->graphicsApiFilter()->setMinorVersion(0);
    m_gl2Technique->graphicsApiFilter()->setProfile(QGraphicsApiFilter::NoProfile);

    m_es2Technique->graphicsApiFilter()->setApi(QGraphicsApiFilter::OpenGLES);
    m_es2Technique->graphicsApiFilter()->setMajorVersion(2);
    m_es2Technique->graphicsApiFilter()->setMinorVersion(0);
    m_es2Technique->graphicsApiFilter()->setProfile(QGraphicsApiFilter::NoProfile);

    m_rhiTechnique->graphicsApiFilter()->setApi(QGraphicsApiFilter::RHI);
    m_rhiTechnique->graphicsApiFilter()->setMajorVersion(1);
    m_rhiTechnique->graphicsApiFilter()->setMinorVersion(0);

    m_filterKey->setParent(m_effect);
    m_filterKey->setName(QStringLiteral("renderingStyle"));
    m_filterKey->setValue(QStringLiteral("forward"));

    m_gl3Technique->addFilterKey(m_filterKey);
    m_gl2Technique->addFilterKey(m_filterKey);
    m_es2Technique->addFilterKey(m_filterKey);
    m_rhiTechnique->addFilterKey(m_filterKey);

    m_gl3RenderPass->setShaderProgram(m_gl3Shader);
    m_gl2RenderPass->setShaderProgram(m_gl2es2Shader);
    m_es2RenderPass->setShaderProgram(m_gl2es2Shader);
    m_rhiRenderPass->setShaderProgram(m_rhiShader);

    QCullFace *cullFront = new QCullFace();
    cullFront->setMode(QCullFace::Front);
    QDepthTest *depthTest = new QDepthTest();
    depthTest->setDepthFunction(QDepthTest::LessOrEqual);
    QSeamlessCubemap *seamlessCubemap = new QSeamlessCubemap();

    m_gl3RenderPass->addRenderState(cullFront);
    m_gl3RenderPass->addRenderState(depthTest);
    m_gl3RenderPass->addRenderState(seamlessCubemap);
    m_gl2RenderPass->addRenderState(cullFront);
    m_gl2RenderPass->addRenderState(depthTest);
    m_es2RenderPass->addRenderState(cullFront);
    m_es2RenderPass->addRenderState(depthTest);
    m_rhiRenderPass->addRenderState(cullFront);
    m_rhiRenderPass->addRenderState(depthTest);

    m_gl3Technique->addRenderPass(m_gl3RenderPass);
    m_gl2Technique->addRenderPass(m_gl2RenderPass);
    m_es2Technique->addRenderPass(m_es2RenderPass);
    m_rhiTechnique->addRenderPass(m_rhiRenderPass);

    m_effect->addTechnique(m_gl3Technique);
    m_effect->addTechnique(m_gl2Technique);
    m_effect->addTechnique(m_es2Technique);
    m_effect->addTechnique(m_rhiTechnique);

    m_material->setEffect(m_effect);
    m_material->addParameter(m_gammaStrengthParameter);
    m_material->addParameter(m_textureParameter);

    m_mesh->setXYMeshResolution(QSize(2, 2));
    m_mesh->setXZMeshResolution(QSize(2, 2));
    m_mesh->setYZMeshResolution(QSize(2, 2));

    m_posXImage->setFace(QTextureCubeMap::CubeMapPositiveX);
    m_posXImage->setMirrored(false);
    m_posYImage->setFace(QTextureCubeMap::CubeMapPositiveY);
    m_posYImage->setMirrored(false);
    m_posZImage->setFace(QTextureCubeMap::CubeMapPositiveZ);
    m_posZImage->setMirrored(false);
    m_negXImage->setFace(QTextureCubeMap::CubeMapNegativeX);
    m_negXImage->setMirrored(false);
    m_negYImage->setFace(QTextureCubeMap::CubeMapNegativeY);
    m_negYImage->setMirrored(false);
    m_negZImage->setFace(QTextureCubeMap::CubeMapNegativeZ);
    m_negZImage->setMirrored(false);

    m_skyboxTexture->setMagnificationFilter(QTextureCubeMap::Linear);
    m_skyboxTexture->setMinificationFilter(QTextureCubeMap::Linear);
    m_skyboxTexture->setGenerateMipMaps(false);
    m_skyboxTexture->setWrapMode(QTextureWrapMode(QTextureWrapMode::ClampToEdge));

    m_skyboxTexture->addTextureImage(m_posXImage);
    m_skyboxTexture->addTextureImage(m_posYImage);
    m_skyboxTexture->addTextureImage(m_posZImage);
    m_skyboxTexture->addTextureImage(m_negXImage);
    m_skyboxTexture->addTextureImage(m_negYImage);
    m_skyboxTexture->addTextureImage(m_negZImage);

    q_func()->addComponent(m_mesh);
    q_func()->addComponent(m_material);
}

/*!
 * \internal
 */
void QSkyboxEntityPrivate::reloadTexture()
{
    if (!m_hasPendingReloadTextureCall) {
        m_hasPendingReloadTextureCall = true;
        QTimer::singleShot(0, [this] {
            if (m_extension == QStringLiteral(".dds")) {
                m_loadedTexture->setSource(QUrl(m_baseName + m_extension));
                m_textureParameter->setValue(QVariant::fromValue(m_loadedTexture));
            } else {
                m_posXImage->setSource(QUrl(m_baseName + QStringLiteral("_posx") + m_extension));
                m_posYImage->setSource(QUrl(m_baseName + QStringLiteral("_posy") + m_extension));
                m_posZImage->setSource(QUrl(m_baseName + QStringLiteral("_posz") + m_extension));
                m_negXImage->setSource(QUrl(m_baseName + QStringLiteral("_negx") + m_extension));
                m_negYImage->setSource(QUrl(m_baseName + QStringLiteral("_negy") + m_extension));
                m_negZImage->setSource(QUrl(m_baseName + QStringLiteral("_negz") + m_extension));
                m_textureParameter->setValue(QVariant::fromValue(m_skyboxTexture));
            }
            m_hasPendingReloadTextureCall = false;
        });
    }
}

/*!
 * \class Qt3DExtras::QSkyboxEntity
 * \inheaderfile Qt3DExtras/QSkyboxEntity
 * \inmodule Qt3DExtras
 *
 * \brief Qt3DExtras::QSkyboxEntity is a convenience Qt3DCore::QEntity subclass that can
 * be used to insert a skybox in a 3D scene.
 *
 * By specifying a base name and an extension, Qt3DExtras::QSkyboxEntity
 * will take care of building a TextureCubeMap to be rendered at runtime. The
 * images in the source directory should match the pattern:
 * \b base name + * "_posx|_posy|_posz|_negx|_negy|_negz" + extension
 *
 * By default the extension defaults to .png.
 *
 * Be sure to disable frustum culling in the FrameGraph through which the
 * skybox rendering happens.
 *
 * \note Please note that you shouldn't try to render a skybox with an
 * orthographic projection.
 *
 * \since 5.5
 */

/*!
 * \qmltype SkyboxEntity
 * \nativetype Qt3DExtras::QSkyboxEntity
 * \inqmlmodule Qt3D.Extras
 *
 * \brief SkyboxEntity is a convenience Entity subclass that can be used to
 * insert a skybox in a 3D scene.
 *
 * By specifying a base name and an extension, SkyboxEntity will take care of
 * building a TextureCubeMap to be rendered at runtime. The images in the
 * source directory should match the pattern: \b base name + *
 * "_posx|_posy|_posz|_negx|_negy|_negz" + extension
 *
 * By default the extension defaults to .png.
 *
 * Be sure to disable frustum culling in the FrameGraph through which the
 * skybox rendering happens.
 *
 * \note Please note that you shouldn't try to render a skybox with an
 * orthographic projection.
 *
 * \since 5.5
 */

/*!
 * Constructs a new Qt3DExtras::QSkyboxEntity object with \a parent as parent.
 */
QSkyboxEntity::QSkyboxEntity(QNode *parent)
    : QEntity(*new QSkyboxEntityPrivate, parent)
{
    d_func()->init();
}

/*! \internal */
QSkyboxEntity::~QSkyboxEntity()
{
}

/*!
 * Sets the base name to \a baseName.
 */
void QSkyboxEntity::setBaseName(const QString &baseName)
{
    Q_D(QSkyboxEntity);
    if (baseName != d->m_baseName) {
        d->m_baseName = baseName;
        emit baseNameChanged(baseName);
        d->reloadTexture();
    }
}
/*!
    \property Qt3DExtras::QSkyboxEntity::baseName

    Contains the base name of the Skybox.
*/
/*!
    \qmlproperty string QSkyboxEntity::baseName

    Contains the base name of the Skybox.
*/
/*!
 * Returns the base name of the Skybox.
 */
QString QSkyboxEntity::baseName() const
{
    Q_D(const QSkyboxEntity);
    return d->m_baseName;
}

/*!
 * Sets the extension to \a extension.
 */
void QSkyboxEntity::setExtension(const QString &extension)
{
    Q_D(QSkyboxEntity);
    if (extension != d->m_extension) {
        d->m_extension = extension;
        emit extensionChanged(extension);
        d->reloadTexture();
    }
}

/*!
    \property Qt3DExtras::QSkyboxEntity::extension

    Contains the extension of the filename for the skybox image, including the
    leading '.'.

    The default value is: .png
*/

/*!
    \qmlproperty string QSkyboxEntity::extension

    Contains the extension of the filename for the skybox image, including the
    leading '.'.

    The default value is: .png
*/
/*!
 * Returns the extension
 */
QString QSkyboxEntity::extension() const
{
    Q_D(const QSkyboxEntity);
    return d->m_extension;
}

/*!
 * Sets the gamma correction enable state to \a enabled.
 * \since 5.9
 */
void QSkyboxEntity::setGammaCorrectEnabled(bool enabled)
{
    Q_D(QSkyboxEntity);
    if (enabled != isGammaCorrectEnabled()) {
        d->m_gammaStrengthParameter->setValue(enabled ? 1.0f : 0.0f);
        emit gammaCorrectEnabledChanged(enabled);
    }
}

/*!
 * Returns true if gamma correction is enabled for this skybox.
 * \since 5.9
 */
bool QSkyboxEntity::isGammaCorrectEnabled() const
{
    Q_D(const QSkyboxEntity);
    return !qFuzzyIsNull(d->m_gammaStrengthParameter->value().toFloat());
}

} // namespace Qt3DExtras

/*!
    \property Qt3DExtras::QSkyboxEntity::gammaCorrect

    A boolean indicating whether gamma correction is enabled.
    \since 5.9
*/
/*!
    \qmlproperty bool QSkyboxEntity::gammaCorrect

    A boolean indicating whether gamma correction is enabled.
    \since 5.9
*/
QT_END_NAMESPACE

#include "moc_qskyboxentity.cpp"
