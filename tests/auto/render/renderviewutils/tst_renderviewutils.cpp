/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
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

#include <QtTest/QTest>

#include <Qt3DRenderer/private/renderviewjobutils_p.h>
#include <Qt3DRenderer/private/rendershaderdata_p.h>
#include <Qt3DRenderer/private/managers_p.h>
#include <Qt3DRenderer/qshaderdata.h>


class tst_RenderViewUtils : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void topLevelScalarValueNoUniforms();
    void topLevelScalarValue();
    void topLevelArrayValue();
    void topLevelStructValue_data();
    void topLevelStructValue();
};

class ScalarShaderData : public Qt3D::QShaderData
{
    Q_OBJECT
    Q_PROPERTY(float scalar READ scalar WRITE setScalar NOTIFY scalarChanged)

public:
    ScalarShaderData()
        : Qt3D::QShaderData()
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

    QHash<QString, Qt3D::Render::ShaderUniform> buildUniformMap(const QString &blockName)
    {
        QHash<QString, Qt3D::Render::ShaderUniform> uniforms;

        uniforms.insert(blockName + QStringLiteral(".scalar"), Qt3D::Render::ShaderUniform());

        return uniforms;
    }

Q_SIGNALS:
    void scalarChanged();

private:
    float m_scalar;
};


class ArrayShaderData : public Qt3D::QShaderData
{
    Q_OBJECT
    Q_PROPERTY(QVariantList array READ array WRITE setArray NOTIFY arrayChanged)

public:
    ArrayShaderData()
        : Qt3D::QShaderData()
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

    QHash<QString, Qt3D::Render::ShaderUniform> buildUniformMap(const QString &blockName)
    {
        QHash<QString, Qt3D::Render::ShaderUniform> uniforms;

        uniforms.insert(blockName + QStringLiteral(".array[0]"), Qt3D::Render::ShaderUniform());

        return uniforms;
    }

Q_SIGNALS:
    void arrayChanged();

private:
    QVariantList m_array;
};

class StructShaderData : public Qt3D::QShaderData
{
    Q_OBJECT
    Q_PROPERTY(float scalar READ scalar WRITE setScalar NOTIFY scalarChanged)
    Q_PROPERTY(QVariantList array READ array WRITE setArray NOTIFY arrayChanged)

public:
    StructShaderData()
        : Qt3D::QShaderData()
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

    virtual QHash<QString, Qt3D::Render::ShaderUniform> buildUniformMap(const QString &blockName)
    {
        QHash<QString, Qt3D::Render::ShaderUniform> uniforms;

        uniforms.insert(blockName + QStringLiteral(".scalar"), Qt3D::Render::ShaderUniform());
        uniforms.insert(blockName + QStringLiteral(".array[0]"), Qt3D::Render::ShaderUniform());

        return uniforms;
    }

    virtual QHash<QString, QVariant> buildUniformMapValues(const QString &blockName)
    {
        QHash<QString, QVariant> uniforms;

        uniforms.insert(blockName + QStringLiteral(".scalar"), QVariant(scalar()));
        uniforms.insert(blockName + QStringLiteral(".array[0]"), QVariant(array()));

        return uniforms;
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
    Q_PROPERTY(Qt3D::QShaderData *inner READ inner WRITE setInner NOTIFY innerChanged)

public:
    MultiLevelStructShaderData()
        : StructShaderData()
        , m_inner(Q_NULLPTR)
    {
    }

    void setInner(Qt3D::QShaderData *inner)
    {
        if (inner != m_inner) {
            m_inner = inner;
            emit innerChanged();
        }
    }

    Qt3D::QShaderData *inner() const
    {
        return m_inner;
    }

    QHash<QString, Qt3D::Render::ShaderUniform> buildUniformMap(const QString &blockName) Q_DECL_OVERRIDE
    {
        QHash<QString, Qt3D::Render::ShaderUniform> innerUniforms;

        StructShaderData *innerData = Q_NULLPTR;
        if ((innerData = qobject_cast<StructShaderData *>(m_inner)) != Q_NULLPTR)
            innerUniforms = innerData->buildUniformMap(QStringLiteral(".inner"));

        QHash<QString, Qt3D::Render::ShaderUniform> uniforms = StructShaderData::buildUniformMap(blockName);
        QHash<QString, Qt3D::Render::ShaderUniform>::const_iterator it = innerUniforms.begin();
        const QHash<QString, Qt3D::Render::ShaderUniform>::const_iterator end = innerUniforms.end();

        while (it != end) {
            uniforms.insert(blockName + it.key(), it.value());
            ++it;
        }
        return uniforms;
    }

    QHash<QString, QVariant> buildUniformMapValues(const QString &blockName) Q_DECL_OVERRIDE
    {
        QHash<QString, QVariant> innerUniformsValues;

        StructShaderData *innerData = Q_NULLPTR;
        if ((innerData = qobject_cast<StructShaderData *>(m_inner)) != Q_NULLPTR)
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

Q_SIGNALS:
    void innerChanged();

private:
    Qt3D::QShaderData *m_inner;
};

namespace {

void initBackendShaderData(Qt3D::QShaderData *frontend,
                           Qt3D::Render::ShaderDataManager *manager)
{
    // Create children first
    Q_FOREACH (QObject *c, frontend->children()) {
        Qt3D::QShaderData *cShaderData = qobject_cast<Qt3D::QShaderData *>(c);
        if (cShaderData)
            initBackendShaderData(cShaderData, manager);
    }

    // Create backend element for frontend one
    Qt3D::Render::RenderShaderData *backend = manager->getOrCreateResource(frontend->id());
    // Init the backend element
    backend->updateFromPeer(frontend);
}

} // anonymous


void tst_RenderViewUtils::topLevelScalarValueNoUniforms()
{
    QScopedPointer<ScalarShaderData> shaderData(new ScalarShaderData());
    QScopedPointer<Qt3D::Render::ShaderDataManager> manager(new Qt3D::Render::ShaderDataManager());

    shaderData->setScalar(883.0f);
    initBackendShaderData(shaderData.data(), manager.data());

    Qt3D::Render::RenderShaderData *backendShaderData = manager->lookupResource(shaderData->id());
    QVERIFY(backendShaderData != Q_NULLPTR);

    Qt3D::Render::UniformBlockValueBuilder blockBuilder;
    blockBuilder.shaderDataManager = manager.data();
    blockBuilder.updatedPropertiesOnly = false;
    // build name-value map
    blockBuilder.buildActiveUniformNameValueMapStructHelper(backendShaderData, QStringLiteral(""));
    // activeUniformNamesToValue should be empty as blockBuilder.uniforms is
    QVERIFY(blockBuilder.activeUniformNamesToValue.isEmpty());
}

void tst_RenderViewUtils::topLevelScalarValue()
{
    QScopedPointer<ScalarShaderData> shaderData(new ScalarShaderData());
    QScopedPointer<Qt3D::Render::ShaderDataManager> manager(new Qt3D::Render::ShaderDataManager());

    shaderData->setScalar(883.0f);
    initBackendShaderData(shaderData.data(), manager.data());

    Qt3D::Render::RenderShaderData *backendShaderData = manager->lookupResource(shaderData->id());
    QVERIFY(backendShaderData != Q_NULLPTR);

    Qt3D::Render::UniformBlockValueBuilder blockBuilder;
    blockBuilder.shaderDataManager = manager.data();
    blockBuilder.updatedPropertiesOnly = false;
    blockBuilder.uniforms = shaderData->buildUniformMap(QStringLiteral("MyBlock"));
    // build name-value map
    blockBuilder.buildActiveUniformNameValueMapStructHelper(backendShaderData, QStringLiteral("MyBlock"));

    QVERIFY(blockBuilder.uniforms.count() == 1);
    QCOMPARE(blockBuilder.activeUniformNamesToValue.count(), 1);

    QHash<QString, QVariant>::const_iterator it = blockBuilder.activeUniformNamesToValue.begin();
    const QHash<QString, QVariant>::const_iterator end = blockBuilder.activeUniformNamesToValue.end();

    while (it != end) {
        QVERIFY(blockBuilder.uniforms.contains(it.key()));
        QCOMPARE(it.value(), QVariant(shaderData->scalar()));
        ++it;
    }
}

void tst_RenderViewUtils::topLevelArrayValue()
{
    QScopedPointer<ArrayShaderData> shaderData(new ArrayShaderData());
    QScopedPointer<Qt3D::Render::ShaderDataManager> manager(new Qt3D::Render::ShaderDataManager());

    QVariantList arrayValues = QVariantList() << 454 << 350 << 383 << 427 << 552;
    shaderData->setArray(arrayValues);
    initBackendShaderData(shaderData.data(), manager.data());

    Qt3D::Render::RenderShaderData *backendShaderData = manager->lookupResource(shaderData->id());
    QVERIFY(backendShaderData != Q_NULLPTR);

    Qt3D::Render::UniformBlockValueBuilder blockBuilder;
    blockBuilder.shaderDataManager = manager.data();
    blockBuilder.updatedPropertiesOnly = false;
    blockBuilder.uniforms = shaderData->buildUniformMap(QStringLiteral("MyBlock"));
    // build name-value map
    blockBuilder.buildActiveUniformNameValueMapStructHelper(backendShaderData, QStringLiteral("MyBlock"));

    QVERIFY(blockBuilder.uniforms.count() == 1);
    QCOMPARE(blockBuilder.activeUniformNamesToValue.count(), 1);

    QHash<QString, QVariant>::const_iterator it = blockBuilder.activeUniformNamesToValue.begin();
    const QHash<QString, QVariant>::const_iterator end = blockBuilder.activeUniformNamesToValue.end();

    while (it != end) {
        QVERIFY(blockBuilder.uniforms.contains(it.key()));
        QCOMPARE(it.value(), QVariant(arrayValues));
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
    QFETCH(StructShaderData *, shaderData);
    QFETCH(QString, blockName);
    QScopedPointer<Qt3D::Render::ShaderDataManager> manager(new Qt3D::Render::ShaderDataManager());

    initBackendShaderData(shaderData, manager.data());

    Qt3D::Render::RenderShaderData *backendShaderData = manager->lookupResource(shaderData->id());
    QVERIFY(backendShaderData != Q_NULLPTR);

    Qt3D::Render::UniformBlockValueBuilder blockBuilder;
    blockBuilder.shaderDataManager = manager.data();
    blockBuilder.updatedPropertiesOnly = false;
    blockBuilder.uniforms = shaderData->buildUniformMap(blockName);
    const QHash<QString, QVariant> expectedValues = shaderData->buildUniformMapValues(blockName);
    // build name-value map
    blockBuilder.buildActiveUniformNameValueMapStructHelper(backendShaderData, blockName);

    QCOMPARE(blockBuilder.activeUniformNamesToValue.count(), blockBuilder.uniforms.count());

    QHash<QString, QVariant>::const_iterator it = blockBuilder.activeUniformNamesToValue.begin();
    const QHash<QString, QVariant>::const_iterator end = blockBuilder.activeUniformNamesToValue.end();

    while (it != end) {
        QVERIFY(blockBuilder.uniforms.contains(it.key()));
        QVERIFY(expectedValues.contains(it.key()));
        QCOMPARE(it.value(), expectedValues.value(it.key()));
        ++it;
    }
}

QTEST_APPLESS_MAIN(tst_RenderViewUtils)

#include "tst_renderviewutils.moc"
