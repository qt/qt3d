// Copyright (C) 2024 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include <QtTest/QTest>
#include <Qt3DQuickScene3D/private/scene3dsgmaterial_p.h>
#include <Qt3DQuickScene3D/private/scene3dsgmaterial_p.h>
#include <QtQuick/private/qsgdefaultrendercontext_p.h>
#include <QtQuick/private/qsgdefaultcontext_p.h>
#include <QtQuick/private/qsgmaterialshader_p.h>
#include <QObject>

class TestScene3DSGMaterialShader: public Qt3DRender::Scene3DSGMaterialShader
{
public:
    auto shaders() const
    {
        Q_D(const QSGMaterialShader);
        return d->shaders;
    }
};

class tst_Quick3DScene3D : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void resourcesForMaterialShader()
    {
        QTest::failOnWarning();

        TestScene3DSGMaterialShader shader;

        // trigger the path that makes shader loads the resource files
        QSGDefaultContext context;
        QSGDefaultRenderContext renderContext(&context);

        QCOMPARE(shader.shaders().size(), 0);
        renderContext.initializeRhiShader(&shader, QShader::Variant::StandardShader);
        QCOMPARE(shader.shaders().size(), 2);
    }
};

QTEST_MAIN(tst_Quick3DScene3D)

#include "tst_quick3dscene3d.moc"
