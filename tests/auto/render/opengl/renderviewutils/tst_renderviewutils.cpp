// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include <QtTest/QTest>
#include <qbackendnodetester.h>
#include <shadervariables_p.h>
#include <Qt3DRender/private/renderviewjobutils_p.h>
#include <Qt3DRender/private/shaderdata_p.h>
#include <Qt3DRender/private/managers_p.h>
#include <Qt3DRender/private/stringtoint_p.h>
#include <Qt3DRender/private/uniformblockbuilder_p.h>
#include <Qt3DRender/qshaderdata.h>
#include <QVariant>
#include "testrenderer.h"
#include "testarbiter.h"

class tst_RenderViewUtils : public Qt3DCore::QBackendNodeTester
{
    Q_OBJECT
private Q_SLOTS:
    void topLevelScalarValueNoUniforms();
    void topLevelScalarValue();
    void topLevelTextureValueNoUniforms();
    void topLevelTextureValue();
    void topLevelArrayValue();
    void nestedShaderDataValue();
    void topLevelStructValue_data();
    void topLevelStructValue();
    void topLevelDynamicProperties();
    void transformedProperties();
    void shouldNotifyDynamicPropertyChanges();

private:
    void initBackendShaderData(Qt3DRender::Render::AbstractRenderer *renderer,
                               Qt3DRender::QShaderData *frontend,
                               Qt3DRender::Render::ShaderDataManager *manager)
    {
        // Create children first
        for (QObject *c : frontend->children()) {
            Qt3DRender::QShaderData *cShaderData = qobject_cast<Qt3DRender::QShaderData *>(c);
            if (cShaderData)
                initBackendShaderData(renderer, cShaderData, manager);
        }

        // Create backend element for frontend one
        Qt3DRender::Render::ShaderData *backend = manager->getOrCreateResource(frontend->id());
        // Init the backend element
        backend->setRenderer(renderer);
        simulateInitializationSync(frontend, backend);
    }

    void initBackendTexture(Qt3DRender::QAbstractTexture *frontend,
                            Qt3DRender::Render::TextureManager *manager)
    {
        // Create backend element for frontend one
        Qt3DRender::Render::Texture *backend = manager->getOrCreateResource(frontend->id());
        // Init the backend element
        simulateInitializationSync(frontend, backend);
    }
};

class ScalarShaderData : public Qt3DRender::QShaderData
{
    Q_OBJECT
    Q_PROPERTY(float scalar READ scalar WRITE setScalar NOTIFY scalarChanged)

public:
    ScalarShaderData(Qt3DCore::QNode *parent = nullptr)
        : Qt3DRender::QShaderData(parent)
        , m_scalar(0.0f)
    {
    }

    void setScalar(float scalar)
    {
        if (scalar != m_scalar) {
            m_scalar = scalar;
            emit scalarChanged();
        }
    }

    float scalar() const
    {
        return m_scalar;
    }

    QHash<QString, Qt3DRender::Render::OpenGL::ShaderUniform> buildUniformMap(const QString &blockName)
    {
        QHash<QString, Qt3DRender::Render::OpenGL::ShaderUniform> uniforms;

        uniforms.insert(blockName + QStringLiteral(".scalar"), Qt3DRender::Render::OpenGL::ShaderUniform());

        return uniforms;
    }

    std::vector<int> buildUniformMapNameIds(const QString &blockName) const
    {
        std::vector<int> ids;

        ids.push_back(Qt3DRender::Render::StringToInt::lookupId(blockName + QStringLiteral(".scalar")));

        return ids;
    }


Q_SIGNALS:
    void scalarChanged();

private:
    float m_scalar;
};

class TextureShaderData : public Qt3DRender::QShaderData
{
    Q_OBJECT
    Q_PROPERTY(Qt3DRender::QAbstractTexture* texture READ texture WRITE setTexture NOTIFY textureChanged)

public:
    TextureShaderData()
        : Qt3DRender::QShaderData()
        , m_texture(nullptr)
    {
    }

    void setTexture(Qt3DRender::QAbstractTexture *texture)
    {
        if (texture != m_texture) {
            m_texture = texture;
            emit textureChanged();
        }
    }

    Qt3DRender::QAbstractTexture *texture() const
    {
        return m_texture;
    }

    QHash<QString, Qt3DRender::Render::OpenGL::ShaderUniform> buildUniformMap(const QString &blockName)
    {
        QHash<QString, Qt3DRender::Render::OpenGL::ShaderUniform> uniforms;

        uniforms.insert(blockName + QStringLiteral(".texture"), Qt3DRender::Render::OpenGL::ShaderUniform());

        return uniforms;
    }

    std::vector<int> buildUniformMapNameIds(const QString &blockName) const
    {
        std::vector<int> ids;

        ids.push_back(Qt3DRender::Render::StringToInt::lookupId(blockName + QStringLiteral(".texture")));

        return ids;
    }

Q_SIGNALS:
    void textureChanged();

private:
    Qt3DRender::QAbstractTexture *m_texture;
};


class ArrayShaderData : public Qt3DRender::QShaderData
{
    Q_OBJECT
    Q_PROPERTY(QVariantList array READ array WRITE setArray NOTIFY arrayChanged)

public:
    ArrayShaderData()
        : Qt3DRender::QShaderData()
    {
    }

    void setArray(const QVariantList &array)
    {
        if (array != m_array) {
            m_array = array;
            emit arrayChanged();
        }
    }

    QVariantList array() const
    {
        return m_array;
    }

    QHash<QString, Qt3DRender::Render::OpenGL::ShaderUniform> buildUniformMap(const QString &blockName)
    {
        QHash<QString, Qt3DRender::Render::OpenGL::ShaderUniform> uniforms;

        uniforms.insert(blockName + QStringLiteral(".array[0]"), Qt3DRender::Render::OpenGL::ShaderUniform());

        return uniforms;
    }

    std::vector<int> buildUniformMapNameIds(const QString &blockName) const
    {
        std::vector<int> ids;

        ids.push_back(Qt3DRender::Render::StringToInt::lookupId(blockName + QStringLiteral(".array[0]")));

        return ids;
    }

Q_SIGNALS:
    void arrayChanged();

private:
    QVariantList m_array;
};

class StructShaderData : public Qt3DRender::QShaderData
{
    Q_OBJECT
    Q_PROPERTY(float scalar READ scalar WRITE setScalar NOTIFY scalarChanged)
    Q_PROPERTY(QVariantList array READ array WRITE setArray NOTIFY arrayChanged)

public:
    StructShaderData()
        : Qt3DRender::QShaderData()
        , m_scalar(0.0f)
    {
    }

    void setScalar(float scalar)
    {
        if (scalar != m_scalar) {
            m_scalar = scalar;
            emit scalarChanged();
        }
    }

    float scalar() const
    {
        return m_scalar;
    }

    void setArray(const QVariantList &array)
    {
        if (array != m_array) {
            m_array = array;
            emit arrayChanged();
        }
    }

    QVariantList array() const
    {
        return m_array;
    }

    virtual QHash<QString, Qt3DRender::Render::OpenGL::ShaderUniform> buildUniformMap(const QString &blockName)
    {
        QHash<QString, Qt3DRender::Render::OpenGL::ShaderUniform> uniforms;

        uniforms.insert(blockName + QStringLiteral(".scalar"), Qt3DRender::Render::OpenGL::ShaderUniform());
        uniforms.insert(blockName + QStringLiteral(".array[0]"), Qt3DRender::Render::OpenGL::ShaderUniform());

        return uniforms;
    }

    virtual QHash<QString, QVariant> buildUniformMapValues(const QString &blockName)
    {
        QHash<QString, QVariant> uniforms;

        uniforms.insert(blockName + QStringLiteral(".scalar"), QVariant(scalar()));
        uniforms.insert(blockName + QStringLiteral(".array[0]"), QVariant(array()));

        return uniforms;
    }

    std::vector<int> buildUniformMapNameIds(const QString &blockName) const
    {
        std::vector<int> ids;

        ids.push_back(Qt3DRender::Render::StringToInt::lookupId(blockName + QStringLiteral(".scalar")));
        ids.push_back(Qt3DRender::Render::StringToInt::lookupId(blockName + QStringLiteral(".array[0]")));

        return ids;
    }


Q_SIGNALS:
    void scalarChanged();
    void arrayChanged();

private:
    float m_scalar;
    QVariantList m_array;
};

class MultiLevelStructShaderData : public StructShaderData
{
    Q_OBJECT
    Q_PROPERTY(Qt3DRender::QShaderData *inner READ inner WRITE setInner NOTIFY innerChanged)

public:
    MultiLevelStructShaderData()
        : StructShaderData()
        , m_inner(nullptr)
    {
    }

    void setInner(Qt3DRender::QShaderData *inner)
    {
        if (inner != m_inner) {
            m_inner = inner;
            emit innerChanged();
        }
    }

    Qt3DRender::QShaderData *inner() const
    {
        return m_inner;
    }

    QHash<QString, Qt3DRender::Render::OpenGL::ShaderUniform> buildUniformMap(const QString &blockName) override
    {
        QHash<QString, Qt3DRender::Render::OpenGL::ShaderUniform> innerUniforms;

        StructShaderData *innerData = nullptr;
        if ((innerData = qobject_cast<StructShaderData *>(m_inner)) != nullptr)
            innerUniforms = innerData->buildUniformMap(QStringLiteral(".inner"));

        QHash<QString, Qt3DRender::Render::OpenGL::ShaderUniform> uniforms = StructShaderData::buildUniformMap(blockName);
        QHash<QString, Qt3DRender::Render::OpenGL::ShaderUniform>::const_iterator it = innerUniforms.begin();
        const QHash<QString, Qt3DRender::Render::OpenGL::ShaderUniform>::const_iterator end = innerUniforms.end();

        while (it != end) {
            uniforms.insert(blockName + it.key(), it.value());
            ++it;
        }
        return uniforms;
    }

    QHash<QString, QVariant> buildUniformMapValues(const QString &blockName) override
    {
        QHash<QString, QVariant> innerUniformsValues;

        StructShaderData *innerData = nullptr;
        if ((innerData = qobject_cast<StructShaderData *>(m_inner)) != nullptr)
            innerUniformsValues = innerData->buildUniformMapValues(QStringLiteral(".inner"));

        QHash<QString, QVariant> uniformsValues = StructShaderData::buildUniformMapValues(blockName);
        QHash<QString, QVariant>::const_iterator it = innerUniformsValues.begin();
        const QHash<QString, QVariant>::const_iterator end = innerUniformsValues.end();

        while (it != end) {
            uniformsValues.insert(blockName + it.key(), it.value());
            ++it;
        }

        return uniformsValues;
    }

    std::vector<int> buildUniformMapNameIds(const QString &blockName) const
    {
        std::vector<int> ids;
        QHash<QString, Qt3DRender::Render::OpenGL::ShaderUniform> innerUniforms;

        StructShaderData *innerData = nullptr;
        if ((innerData = qobject_cast<StructShaderData *>(m_inner)) != nullptr)
            innerUniforms = innerData->buildUniformMap(QStringLiteral(".inner"));

        QHash<QString, Qt3DRender::Render::OpenGL::ShaderUniform>::const_iterator it = innerUniforms.begin();
        const QHash<QString, Qt3DRender::Render::OpenGL::ShaderUniform>::const_iterator end = innerUniforms.end();

        while (it != end) {
            ids.push_back(Qt3DRender::Render::StringToInt::lookupId(blockName + it.key()));
            ++it;
        }

        return ids;
    }

Q_SIGNALS:
    void innerChanged();

private:
    Qt3DRender::QShaderData *m_inner;
};

void tst_RenderViewUtils::topLevelScalarValueNoUniforms()
{
    // GIVEN
    TestRenderer renderer;
    QScopedPointer<ScalarShaderData> shaderData(new ScalarShaderData());
    QScopedPointer<Qt3DRender::Render::ShaderDataManager> manager(new Qt3DRender::Render::ShaderDataManager());
    QScopedPointer<Qt3DRender::Render::TextureManager> textureManager(new Qt3DRender::Render::TextureManager());

    // WHEN
    shaderData->setScalar(883.0f);
    initBackendShaderData(&renderer, shaderData.data(), manager.data());

    // THEN
    Qt3DRender::Render::ShaderData *backendShaderData = manager->lookupResource(shaderData->id());
    QVERIFY(backendShaderData != nullptr);

    // WHEB
    const Matrix4x4 v;
    Qt3DRender::Render::UniformBlockValueBuilder blockBuilder({},
                                                              manager.data(),
                                                              textureManager.data(),
                                                              v);
    // build name-value map
    blockBuilder.buildActiveUniformNameValueMapStructHelper(backendShaderData, QStringLiteral(""));

    // THEN
    // activeUniformNamesToValue should be empty as blockBuilder.uniforms is
    QVERIFY(blockBuilder.activeUniformNamesToValue.isEmpty());
}

void tst_RenderViewUtils::topLevelScalarValue()
{
    // GIVEN
    TestRenderer renderer;
    QScopedPointer<ScalarShaderData> shaderData(new ScalarShaderData());
    QScopedPointer<Qt3DRender::Render::ShaderDataManager> manager(new Qt3DRender::Render::ShaderDataManager());
    QScopedPointer<Qt3DRender::Render::TextureManager> textureManager(new Qt3DRender::Render::TextureManager());

    // WHEN
    shaderData->setScalar(883.0f);
    initBackendShaderData(&renderer, shaderData.data(), manager.data());

    // THEN
    Qt3DRender::Render::ShaderData *backendShaderData = manager->lookupResource(shaderData->id());
    QVERIFY(backendShaderData != nullptr);

    // WHEN
    const Matrix4x4 v;
    const auto nameIds = shaderData->buildUniformMapNameIds(QStringLiteral("MyBlock"));
    Qt3DRender::Render::UniformBlockValueBuilder blockBuilder(nameIds,
                                                              manager.data(),
                                                              textureManager.data(),
                                                              v);
    // build name-value map
    blockBuilder.buildActiveUniformNameValueMapStructHelper(backendShaderData, QStringLiteral("MyBlock"));

    // THEN
    QCOMPARE(blockBuilder.activeUniformNamesToValue.size(), 1);

    // WHEN
    Qt3DRender::Render::UniformBlockValueBuilderHash::const_iterator it = blockBuilder.activeUniformNamesToValue.begin();
    const Qt3DRender::Render::UniformBlockValueBuilderHash::const_iterator end = blockBuilder.activeUniformNamesToValue.end();

    while (it != end) {
        // THEN
        QCOMPARE(it.value(), QVariant(shaderData->scalar()));
        ++it;
    }
}

void tst_RenderViewUtils::topLevelTextureValueNoUniforms()
{
    // GIVEN
    TestRenderer renderer;
    QScopedPointer<TextureShaderData> shaderData(new TextureShaderData);
    QScopedPointer<Qt3DRender::Render::ShaderDataManager> manager(new Qt3DRender::Render::ShaderDataManager);
    QScopedPointer<Qt3DRender::QAbstractTexture> texture(new Qt3DRender::QTexture2D);
    QScopedPointer<Qt3DRender::Render::TextureManager> textureManager(new Qt3DRender::Render::TextureManager());

    // WHEN
    shaderData->setTexture(texture.data());
    initBackendShaderData(&renderer, shaderData.data(), manager.data());

    // THEN
    Qt3DRender::Render::ShaderData *backendShaderData = manager->lookupResource(shaderData->id());
    QVERIFY(backendShaderData != nullptr);

    // WHEN
    const Matrix4x4 v;
    Qt3DRender::Render::UniformBlockValueBuilder blockBuilder({},
                                                              manager.data(),
                                                              textureManager.data(),
                                                              v);
    // build name-value map
    blockBuilder.buildActiveUniformNameValueMapStructHelper(backendShaderData, QStringLiteral(""));

    // THEN
    // activeUniformNamesToValue should be empty as blockBuilder.uniforms is
    QVERIFY(blockBuilder.activeUniformNamesToValue.isEmpty());
}

void tst_RenderViewUtils::topLevelTextureValue()
{
    // GIVEN
    TestRenderer renderer;
    QScopedPointer<TextureShaderData> shaderData(new TextureShaderData);
    QScopedPointer<Qt3DRender::Render::ShaderDataManager> manager(new Qt3DRender::Render::ShaderDataManager);
    QScopedPointer<Qt3DRender::QAbstractTexture> texture(new Qt3DRender::QTexture2D);
    QScopedPointer<Qt3DRender::Render::TextureManager> textureManager(new Qt3DRender::Render::TextureManager());

    // WHEN
    initBackendTexture(texture.data(), textureManager.data());
    shaderData->setTexture(texture.data());
    initBackendShaderData(&renderer, shaderData.data(), manager.data());

    // THEN
    Qt3DRender::Render::ShaderData *backendShaderData = manager->lookupResource(shaderData->id());
    QVERIFY(backendShaderData != nullptr);

    // WHEN
    const Matrix4x4 v;
    Qt3DRender::Render::UniformBlockValueBuilder blockBuilder(shaderData->buildUniformMapNameIds(QStringLiteral("MyBlock")),
                                                              manager.data(),
                                                              textureManager.data(),
                                                              v);


    // build name-value map
    blockBuilder.buildActiveUniformNameValueMapStructHelper(backendShaderData, QStringLiteral("MyBlock"));

    // THEN
    QCOMPARE(blockBuilder.activeUniformNamesToValue.size(), 1);

    // WHEN
    Qt3DRender::Render::UniformBlockValueBuilderHash::const_iterator it = blockBuilder.activeUniformNamesToValue.begin();
    const Qt3DRender::Render::UniformBlockValueBuilderHash::const_iterator end = blockBuilder.activeUniformNamesToValue.end();

    while (it != end) {
        // THEN
        QCOMPARE(it.value(), QVariant::fromValue(shaderData->texture()->id()));
        ++it;
    }
}

void tst_RenderViewUtils::topLevelArrayValue()
{
    // GIVEN
    TestRenderer renderer;
    QScopedPointer<ArrayShaderData> shaderData(new ArrayShaderData());
    QScopedPointer<Qt3DRender::Render::ShaderDataManager> manager(new Qt3DRender::Render::ShaderDataManager());
    QScopedPointer<Qt3DRender::Render::TextureManager> textureManager(new Qt3DRender::Render::TextureManager());

    // WHEN
    QVariantList arrayValues = QVariantList() << 454 << 350 << 383 << 427 << 552;
    shaderData->setArray(arrayValues);
    initBackendShaderData(&renderer, shaderData.data(), manager.data());

    // THEN
    Qt3DRender::Render::ShaderData *backendShaderData = manager->lookupResource(shaderData->id());
    QVERIFY(backendShaderData != nullptr);

    // WHEN
    const Matrix4x4 v;
    const auto nameIds = shaderData->buildUniformMapNameIds(QStringLiteral("MyBlock"));
    Qt3DRender::Render::UniformBlockValueBuilder blockBuilder(nameIds,
                                                                      manager.data(),
                                                                      textureManager.data(),
                                                                      v);
    // build name-value map
    blockBuilder.buildActiveUniformNameValueMapStructHelper(backendShaderData, QStringLiteral("MyBlock"));

    // THEN
    QCOMPARE(blockBuilder.activeUniformNamesToValue.size(), 1);

    // WHEN
    Qt3DRender::Render::UniformBlockValueBuilderHash::const_iterator it = blockBuilder.activeUniformNamesToValue.begin();
    const Qt3DRender::Render::UniformBlockValueBuilderHash::const_iterator end = blockBuilder.activeUniformNamesToValue.end();

    while (it != end) {
        // THEN
        QCOMPARE(it.value(), QVariant(arrayValues));
        ++it;
    }
}

void tst_RenderViewUtils::nestedShaderDataValue()
{
    // GIVEN
    TestRenderer renderer;
    QScopedPointer<ArrayShaderData> arrayShaderData(new ArrayShaderData());
    QScopedPointer<Qt3DRender::Render::ShaderDataManager> manager(new Qt3DRender::Render::ShaderDataManager());
    QScopedPointer<Qt3DRender::Render::TextureManager> textureManager(new Qt3DRender::Render::TextureManager());

    QScopedPointer<ScalarShaderData> shaderData1(new ScalarShaderData(arrayShaderData.data()));
    QScopedPointer<ScalarShaderData> shaderData2(new ScalarShaderData(arrayShaderData.data()));
    QScopedPointer<ScalarShaderData> shaderData3(new ScalarShaderData(arrayShaderData.data()));

    shaderData1->setScalar(883.0f);
    shaderData2->setScalar(1200.0f);
    shaderData3->setScalar(1340.0f);
    QHash<QString, QVariant> scalarValues;
    scalarValues[QStringLiteral("MyBlock.array[0].scalar")] = shaderData1->scalar();
    scalarValues[QStringLiteral("MyBlock.array[1].scalar")] = shaderData2->scalar();
    scalarValues[QStringLiteral("MyBlock.array[2].scalar")] = shaderData3->scalar();


    const Qt3DCore::QNodeId id1 = shaderData1->id();
    const Qt3DCore::QNodeId id2 = shaderData2->id();
    const Qt3DCore::QNodeId id3 = shaderData3->id();

    // WHEN
    const QVariantList arrayValues = QVariantList() << QVariant::fromValue(id1) << QVariant::fromValue(id2) << QVariant::fromValue(id3);
    arrayShaderData->setArray(arrayValues);
    initBackendShaderData(&renderer, arrayShaderData.data(), manager.data());

    // THEN
    Qt3DRender::Render::ShaderData *backendArrayShaderData = manager->lookupResource(arrayShaderData->id());
    Qt3DRender::Render::ShaderData *backendShaderData1 = manager->lookupResource(id1);
    Qt3DRender::Render::ShaderData *backendShaderData2 = manager->lookupResource(id2);
    Qt3DRender::Render::ShaderData *backendShaderData3 = manager->lookupResource(id3);
    QVERIFY(backendArrayShaderData != nullptr);
    QVERIFY(backendShaderData1 != nullptr);
    QVERIFY(backendShaderData2 != nullptr);
    QVERIFY(backendShaderData3 != nullptr);

    // WHEN
    const Matrix4x4 v;
    const std::vector<int> nameIds = {
        Qt3DRender::Render::StringToInt::lookupId(QStringLiteral("MyBlock.array[0].scalar")),
        Qt3DRender::Render::StringToInt::lookupId(QStringLiteral("MyBlock.array[1].scalar")),
        Qt3DRender::Render::StringToInt::lookupId(QStringLiteral("MyBlock.array[2].scalar")),
    };

    Qt3DRender::Render::UniformBlockValueBuilder blockBuilder(nameIds,
                                                              manager.data(),
                                                              textureManager.data(),
                                                              v);

    // build name-value map
    blockBuilder.buildActiveUniformNameValueMapStructHelper(backendArrayShaderData, QStringLiteral("MyBlock"));

    // THEN
    QCOMPARE(blockBuilder.activeUniformNamesToValue.size(), 3);

    // WHEN
    auto it = blockBuilder.activeUniformNamesToValue.cbegin();
    const auto end = blockBuilder.activeUniformNamesToValue.cend();

    while (it != end) {
        // THEN
        QCOMPARE(it.value(), scalarValues.value(Qt3DRender::Render::StringToInt::lookupString(it.key())));
        ++it;
    }
}

void tst_RenderViewUtils::topLevelStructValue_data()
{
    QTest::addColumn<StructShaderData*>("shaderData");
    QTest::addColumn<QString>("blockName");

    QVariantList arrayValues2 = QVariantList() << 180 << 220 << 250 << 270 << 300 << 350 << 550;
    QVariantList arrayValues = QVariantList() << 454 << 350 << 383 << 427 << 552;

    MultiLevelStructShaderData *twoLevelsNestedShaderData = new MultiLevelStructShaderData();
    MultiLevelStructShaderData *singleLevelShaderData = new MultiLevelStructShaderData();
    StructShaderData *shaderData = new StructShaderData();

    // Don't forget to set the parent so that initBackendShaderData
    // properly initializes nested members
    shaderData->setParent(singleLevelShaderData);
    shaderData->setArray(arrayValues);
    shaderData->setScalar(1584.0f);

    singleLevelShaderData->setParent(twoLevelsNestedShaderData);
    singleLevelShaderData->setInner(shaderData);
    singleLevelShaderData->setScalar(1200.0f);
    singleLevelShaderData->setArray(arrayValues2);

    twoLevelsNestedShaderData->setInner(singleLevelShaderData);
    twoLevelsNestedShaderData->setArray(arrayValues + arrayValues2);
    twoLevelsNestedShaderData->setScalar(1340.0f);

    QTest::newRow("simple struct") << shaderData << QStringLiteral("Block");
    QTest::newRow("single level inner struct") << (StructShaderData *)singleLevelShaderData << QStringLiteral("Block");
    QTest::newRow("tow level inner struct") << (StructShaderData *)twoLevelsNestedShaderData << QStringLiteral("Block");
}

void tst_RenderViewUtils::topLevelStructValue()
{
    // GIVEN
    TestRenderer renderer;
    QFETCH(StructShaderData *, shaderData);
    QFETCH(QString, blockName);
    QScopedPointer<Qt3DRender::Render::ShaderDataManager> manager(new Qt3DRender::Render::ShaderDataManager());
    QScopedPointer<Qt3DRender::Render::TextureManager> textureManager(new Qt3DRender::Render::TextureManager());

    // WHEN
    initBackendShaderData(&renderer, shaderData, manager.data());

    // THEN
    Qt3DRender::Render::ShaderData *backendShaderData = manager->lookupResource(shaderData->id());
    QVERIFY(backendShaderData != nullptr);

    // WHEN
    const Matrix4x4 v;
    const auto nameIds = shaderData->buildUniformMapNameIds(blockName);
    Qt3DRender::Render::UniformBlockValueBuilder blockBuilder(nameIds,
                                                              manager.data(),
                                                              textureManager.data(),
                                                              v);

    const QHash<QString, QVariant> expectedValues = shaderData->buildUniformMapValues(blockName);
    // build name-value map
    blockBuilder.buildActiveUniformNameValueMapStructHelper(backendShaderData, blockName);

    // THEN
    QCOMPARE(size_t(blockBuilder.activeUniformNamesToValue.size()),
             shaderData->buildUniformMapNameIds(blockName).size());

    // WHEN
    Qt3DRender::Render::UniformBlockValueBuilderHash::const_iterator it = blockBuilder.activeUniformNamesToValue.begin();
    const Qt3DRender::Render::UniformBlockValueBuilderHash::const_iterator end = blockBuilder.activeUniformNamesToValue.end();

    while (it != end) {
        // THEN
        QVERIFY(expectedValues.contains(Qt3DRender::Render::StringToInt::lookupString(it.key())));
        QCOMPARE(it.value(), expectedValues.value(Qt3DRender::Render::StringToInt::lookupString(it.key())));
        ++it;
    }
}

void tst_RenderViewUtils::topLevelDynamicProperties()
{
    // GIVEN
    TestRenderer renderer;
    QScopedPointer<Qt3DRender::QShaderData> shaderData(new Qt3DRender::QShaderData());
    QScopedPointer<Qt3DRender::Render::ShaderDataManager> manager(new Qt3DRender::Render::ShaderDataManager());
    QScopedPointer<Qt3DRender::QAbstractTexture> texture(new Qt3DRender::QTexture2D);
    QScopedPointer<Qt3DRender::Render::TextureManager> textureManager(new Qt3DRender::Render::TextureManager());

    // WHEN
    initBackendTexture(texture.data(), textureManager.data());
    shaderData->setProperty("scalar", 883.0f);
    shaderData->setProperty("array", QVariantList() << 454 << 350 << 383 << 427 << 552);
    shaderData->setProperty("texture", QVariant::fromValue(texture.data()));
    initBackendShaderData(&renderer, shaderData.data(), manager.data());

    // THEN
    Qt3DRender::Render::ShaderData *backendShaderData = manager->lookupResource(shaderData->id());
    QVERIFY(backendShaderData != nullptr);

    // WHEN
    const Matrix4x4 v;
    const std::vector<int> nameIds = {
        Qt3DRender::Render::StringToInt::lookupId(QStringLiteral("MyBlock.scalar")),
        Qt3DRender::Render::StringToInt::lookupId(QStringLiteral("MyBlock.array[0]")),
        Qt3DRender::Render::StringToInt::lookupId(QStringLiteral("MyBlock.texture")),
    };

    Qt3DRender::Render::UniformBlockValueBuilder blockBuilder(nameIds,
                                                              manager.data(),
                                                              textureManager.data(),
                                                              v);
    // build name-value map
    blockBuilder.buildActiveUniformNameValueMapStructHelper(backendShaderData, QStringLiteral("MyBlock"));
    // THEN
    QCOMPARE(blockBuilder.activeUniformNamesToValue.size(), 3);

    QCOMPARE(blockBuilder.activeUniformNamesToValue.value(Qt3DRender::Render::StringToInt::lookupId("MyBlock.scalar")),
             shaderData->property("scalar"));
    QCOMPARE(blockBuilder.activeUniformNamesToValue.value(Qt3DRender::Render::StringToInt::lookupId("MyBlock.array[0]")),
             shaderData->property("array"));
    QCOMPARE(blockBuilder.activeUniformNamesToValue.value(Qt3DRender::Render::StringToInt::lookupId("MyBlock.texture")),
             QVariant::fromValue(texture->id()));
}

void tst_RenderViewUtils::transformedProperties()
{
    // GIVEN
    QScopedPointer<Qt3DRender::QShaderData> shaderData(new Qt3DRender::QShaderData());
    QScopedPointer<Qt3DRender::Render::ShaderDataManager> manager(new Qt3DRender::Render::ShaderDataManager());
    TestRenderer renderer;

    // WHEN
    const Vector3D position = Vector3D(15.0f, -5.0f, 10.0f);
    const QVector3D positionQt = convertToQVector3D(position);
    Matrix4x4 worldMatrix;
    {
        QMatrix4x4 m;
        m.translate(-3.0f, 2.0f, 7.5f);
        worldMatrix = Matrix4x4(m);
    }
    Matrix4x4 viewMatrix;
    {
        QMatrix4x4 m;
        m.translate(9.0f, 6.0f, 12.0f);
        viewMatrix = Matrix4x4(m);
    }

    shaderData->setProperty("position0", positionQt);
    shaderData->setProperty("position1", positionQt);
    shaderData->setProperty("position2", positionQt);
    shaderData->setProperty("position3", positionQt);
    shaderData->setProperty("position1Transformed", Qt3DRender::Render::ShaderData::ModelToEye);
    shaderData->setProperty("position2Transformed", Qt3DRender::Render::ShaderData::ModelToWorld);
    shaderData->setProperty("position3Transformed", Qt3DRender::Render::ShaderData::ModelToWorldDirection);
    initBackendShaderData(&renderer, shaderData.data(), manager.data());

    // THEN
    Qt3DRender::Render::ShaderData *backendShaderData = manager->lookupResource(shaderData->id());
    QVERIFY(backendShaderData != nullptr);
    QCOMPARE(backendShaderData->propertyTransformType(QStringLiteral("position0")), Qt3DRender::Render::ShaderData::NoTransform);
    QCOMPARE(backendShaderData->propertyTransformType(QStringLiteral("position1")), Qt3DRender::Render::ShaderData::ModelToEye);
    QCOMPARE(backendShaderData->propertyTransformType(QStringLiteral("position2")), Qt3DRender::Render::ShaderData::ModelToWorld);
    QCOMPARE(backendShaderData->propertyTransformType(QStringLiteral("position3")), Qt3DRender::Render::ShaderData::ModelToWorldDirection);

    // WHEN
    backendShaderData->updateWorldTransform(worldMatrix);
    const QHash<QString, Qt3DRender::Render::ShaderData::PropertyValue> &props = backendShaderData->properties();
    const Qt3DRender::Render::ShaderData::PropertyValue propPos1 = props[QStringLiteral("position1")];
    const Qt3DRender::Render::ShaderData::PropertyValue propPos2 = props[QStringLiteral("position2")];
    const Qt3DRender::Render::ShaderData::PropertyValue propPos3 = props[QStringLiteral("position3")];
    const Qt3DRender::Render::ShaderData::PropertyValue propPos0 = props[QStringLiteral("position0")];

    const Vector3D position1Value = backendShaderData->getTransformedProperty(&propPos1, viewMatrix).value<Vector3D>();
    const Vector3D position2Value = backendShaderData->getTransformedProperty(&propPos2, viewMatrix).value<Vector3D>();
    const Vector3D position3Value = backendShaderData->getTransformedProperty(&propPos3, viewMatrix).value<Vector3D>();
    const QVariant position0Value = backendShaderData->getTransformedProperty(&propPos0, viewMatrix);

    // THEN
    QCOMPARE(position0Value, positionQt);
    QCOMPARE(position1Value, (viewMatrix * worldMatrix).map(position));
    QCOMPARE(position2Value, worldMatrix.map(position));
    QCOMPARE(position3Value, Vector3D((worldMatrix * Vector4D(position, 0.0f))));
}

void tst_RenderViewUtils::shouldNotifyDynamicPropertyChanges()
{
    // GIVEN
    TestArbiter arbiter;
    QScopedPointer<Qt3DRender::QShaderData> shaderData(new Qt3DRender::QShaderData());
    arbiter.setArbiterOnNode(shaderData.data());

    // WHEN
    shaderData->setProperty("scalar", 883.0f);

    // THEN
    QCOMPARE(arbiter.dirtyNodes().size(), 1);
    QCOMPARE(arbiter.dirtyNodes().front(), shaderData.data());

    arbiter.clear();

    // WHEN
    QScopedPointer<Qt3DRender::QAbstractTexture> texture(new Qt3DRender::QTexture2D);
    shaderData->setProperty("texture", QVariant::fromValue(texture.data()));

    // THEN
    QCOMPARE(arbiter.dirtyNodes().size(), 1);
    QCOMPARE(arbiter.dirtyNodes().front(), shaderData.data());
}

QTEST_MAIN(tst_RenderViewUtils)

#include "tst_renderviewutils.moc"
