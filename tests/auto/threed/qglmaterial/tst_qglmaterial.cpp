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

#include <QtTest/QtTest>
#include <QtGui/QOpenGLContext>
#include <QOpenGLShaderProgram>

#include "qglmaterial.h"
#include "qglcolormaterial.h"
#include "qgltwosidedmaterial.h"
#include "qgllightparameters.h"
#include "qgllightmodel.h"
#include "qgltexture2d.h"
#include "qglcamera.h"
#include "qglpainter.h"
#include "qglvertexbundle.h"
#include "qvector2darray.h"
#include "qvector3darray.h"
#include <QtCore/qeventloop.h>
#include <QtGui/qvector3d.h>
#include <QtGui/qvector4d.h>

#include "qglframebufferobjectsurface.h"
#include "qglview.h"

class TestWidget;

class tst_QGLMaterial : public QObject
{
    Q_OBJECT
public:
    tst_QGLMaterial() : widget(0) {}
    ~tst_QGLMaterial() {}

private slots:
    void initTestCase();

    void standardMaterial();
    void textureLayers();
    void colorMaterial();
    void twoSidedMaterial();

    void standardMaterialDraw();
    void colorMaterialDraw();

    void cleanupTestCase();

private:
    TestWidget *widget;
};

class TestWidget : public QGLView
{
    Q_OBJECT
public:
    TestWidget(QWindow *parent = 0);
    ~TestWidget();

    bool runTest(QGLAbstractMaterial *mat, int timeout = 5000);
    bool sameColor(const QColor &color) const;

    void setClearColor(const QColor &color) { clearColor = color; }

protected:
    void initializeGL();
    void paintGL();

private:
    QGLCamera camera;
    QGLVertexBundle bundle;
    QGLAbstractMaterial *material;
    QEventLoop *eventLoop;
    bool paintDone;
    QColor pixel;
    QColor clearColor;
};

TestWidget::TestWidget(QWindow *parent)
    : QGLView(parent)
    , material(0)
    , eventLoop(0)
    , paintDone(false)
    , clearColor(Qt::black)
{
    setGeometry(QRect(0, 0, 100, 100));
    QVector3DArray positions;
    QVector3DArray normals;
    QVector2DArray texCoords;

    positions.append(QVector3D(-0.5, -0.5, 0.0));
    positions.append(QVector3D(0.5, -0.5, 0.0));
    positions.append(QVector3D(0.5, 0.5, 0.0));
    positions.append(QVector3D(-0.5, 0.5, 0.0));

    normals.append(QVector3D(0, 0, 1));
    normals.append(QVector3D(0, 0, 1));
    normals.append(QVector3D(0, 0, 1));
    normals.append(QVector3D(0, 0, 1));

    texCoords.append(QVector2D(0.0f, 0.0f));
    texCoords.append(QVector2D(1.0f, 0.0f));
    texCoords.append(QVector2D(1.0f, 1.0f));
    texCoords.append(QVector2D(0.0f, 1.0f));

    bundle.addAttribute(QGL::Position, positions);
    bundle.addAttribute(QGL::Normal, normals);
    bundle.addAttribute(QGL::TextureCoord0, texCoords);
}

TestWidget::~TestWidget()
{
}

bool TestWidget::runTest(QGLAbstractMaterial *mat, int timeout)
{
    material = mat;
    paintDone = false;
    pixel = QColor();
    eventLoop = new QEventLoop(this);
    QTimer::singleShot(timeout, eventLoop, SLOT(quit()));
    if (!isVisible()) {
        show();
        QTest::qWaitForWindowExposed(this);
    } else {
        update();
    }
    if (!paintDone)
        eventLoop->exec();
    delete eventLoop;
    material = 0;
    eventLoop = 0;
    return paintDone;
}

bool TestWidget::sameColor(const QColor &color) const
{
    if (!pixel.isValid()) {
        qWarning("did not get a valid pixel");
        return false;
    }
    if (qAbs(pixel.red() - color.red()) <= 16 &&
            qAbs(pixel.green() - color.green()) <= 16 &&
            qAbs(pixel.blue() - color.blue()) <= 16)
        return true;
    qWarning() << "pixel color does not match, actual:"
               << pixel.name() << "expected:" << color.name();
    return false;
}

void TestWidget::initializeGL()
{
    bundle.upload();

    QGLPainter painter(this);
    if (painter.hasOpenGLFeature(QOpenGLFunctions::BlendColor))
        painter.glBlendColor(0, 0, 0, 0);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    if (painter.hasOpenGLFeature(QOpenGLFunctions::BlendEquation))
        painter.glBlendEquation(GL_FUNC_ADD);
    else if (painter.hasOpenGLFeature(QOpenGLFunctions::BlendEquationSeparate))
        painter.glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
}

void TestWidget::paintGL()
{
    QGLPainter painter(this);
    QOpenGLFramebufferObject fbo(512, 512);
    QGLFramebufferObjectSurface surf(&fbo);

    surf.activate();

    painter.setClearColor(clearColor);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    painter.setCamera(&camera);

    painter.clearAttributes();
    painter.setVertexBundle(bundle);

    if (material) {
        material->bind(&painter);
        material->prepareToDraw(&painter, painter.attributes());
        painter.draw(QGL::TriangleFan, 4);
        material->release(&painter, 0);
    }

    surf.deactivate();

    QImage capture = fbo.toImage();

    if (capture.width() > 0 && capture.height() > 0)
        pixel = QColor(capture.pixel(capture.width() / 2, capture.height() / 2));

    if (!paintDone && eventLoop) {
        paintDone = true;
        eventLoop->quit();
    }
}

void tst_QGLMaterial::initTestCase()
{
    widget = new TestWidget();
    widget->show();
    QOpenGLContext *ctx = widget->context();
    if (!QOpenGLShaderProgram::hasOpenGLShaderPrograms(ctx))
        QSKIP("Shaders not supported");
    if (!ctx || !ctx->isValid())
        QSKIP("Could not create valid GL context");

    // TODO: if there is no context the whole thing gets skipped.  We should at
    // least be able to test for properties, setters & getters and so on.
}

void tst_QGLMaterial::cleanupTestCase()
{
    delete widget;
}

// Fix the problem where a compared value happens to be zero (and
// you cannot always predict this, and should not predict it
// since then you produce self-fulling prophecies instead of tests).
// In that case qFuzzyCompare has a completely strict criterion since
// it finds the "fudge factor" by multiplying by zero...
static inline bool fuzzyCompare(float p1, float p2)
{
    float fac = qMin(qAbs(p1), qAbs(p2));
    return (qAbs(p1 - p2) <= (qIsNull(fac) ? 0.00001f : 0.00001f * fac));
}

void tst_QGLMaterial::standardMaterial()
{
    // Test that a newly created object has the correct defaults.
    QGLMaterial mat1;
    QVERIFY(fuzzyCompare(mat1.ambientColor().redF(), 0.2f));
    QVERIFY(fuzzyCompare(mat1.ambientColor().greenF(), 0.2f));
    QVERIFY(fuzzyCompare(mat1.ambientColor().blueF(), 0.2f));
    QVERIFY(fuzzyCompare(mat1.ambientColor().alphaF(), 1.0f));
    QVERIFY(fuzzyCompare(mat1.diffuseColor().redF(), 0.8f));
    QVERIFY(fuzzyCompare(mat1.diffuseColor().greenF(), 0.8f));
    QVERIFY(fuzzyCompare(mat1.diffuseColor().blueF(), 0.8f));
    QVERIFY(fuzzyCompare(mat1.diffuseColor().alphaF(), 1.0f));
    QCOMPARE(mat1.specularColor().red(), 0);
    QCOMPARE(mat1.specularColor().green(), 0);
    QCOMPARE(mat1.specularColor().blue(), 0);
    QCOMPARE(mat1.specularColor().alpha(), 255);
    QCOMPARE(mat1.emittedLight().red(), 0);
    QCOMPARE(mat1.emittedLight().green(), 0);
    QCOMPARE(mat1.emittedLight().blue(), 0);
    QCOMPARE(mat1.emittedLight().alpha(), 255);
    QCOMPARE(mat1.shininess(), 0.0f);
    QVERIFY(!mat1.isTransparent());

    // Test modifying each field individually, including expected signals.
    QGLMaterial mat2;
    QSignalSpy ambientSpy(&mat2, SIGNAL(ambientColorChanged()));
    QSignalSpy diffuseSpy(&mat2, SIGNAL(diffuseColorChanged()));
    QSignalSpy specularSpy(&mat2, SIGNAL(specularColorChanged()));
    QSignalSpy emittedSpy(&mat2, SIGNAL(emittedLightChanged()));
    QSignalSpy shininessSpy(&mat2, SIGNAL(shininessChanged()));
    QSignalSpy texturesSpy(&mat2, SIGNAL(texturesChanged()));
    QSignalSpy materialSpy(&mat2, SIGNAL(materialChanged()));
    mat2.setAmbientColor(Qt::green);
    QCOMPARE(mat2.ambientColor().red(), 0);
    QCOMPARE(mat2.ambientColor().green(), 255);
    QCOMPARE(mat2.ambientColor().blue(), 0);
    QCOMPARE(mat2.ambientColor().alpha(), 255);
    QCOMPARE(ambientSpy.count(), 1);
    QCOMPARE(diffuseSpy.count(), 0);
    QCOMPARE(specularSpy.count(), 0);
    QCOMPARE(emittedSpy.count(), 0);
    QCOMPARE(shininessSpy.count(), 0);
    QCOMPARE(texturesSpy.count(), 0);
    QCOMPARE(materialSpy.count(), 1);
    mat2.setDiffuseColor(Qt::blue);
    QCOMPARE(mat2.diffuseColor().red(), 0);
    QCOMPARE(mat2.diffuseColor().green(), 0);
    QCOMPARE(mat2.diffuseColor().blue(), 255);
    QCOMPARE(mat2.diffuseColor().alpha(), 255);
    QCOMPARE(ambientSpy.count(), 1);
    QCOMPARE(diffuseSpy.count(), 1);
    QCOMPARE(specularSpy.count(), 0);
    QCOMPARE(emittedSpy.count(), 0);
    QCOMPARE(shininessSpy.count(), 0);
    QCOMPARE(texturesSpy.count(), 0);
    QCOMPARE(materialSpy.count(), 2);
    mat2.setSpecularColor(Qt::cyan);
    QCOMPARE(mat2.specularColor().red(), 0);
    QCOMPARE(mat2.specularColor().green(), 255);
    QCOMPARE(mat2.specularColor().blue(), 255);
    QCOMPARE(mat2.specularColor().alpha(), 255);
    QCOMPARE(ambientSpy.count(), 1);
    QCOMPARE(diffuseSpy.count(), 1);
    QCOMPARE(specularSpy.count(), 1);
    QCOMPARE(emittedSpy.count(), 0);
    QCOMPARE(shininessSpy.count(), 0);
    QCOMPARE(texturesSpy.count(), 0);
    QCOMPARE(materialSpy.count(), 3);
    mat2.setEmittedLight(Qt::white);
    QCOMPARE(mat2.emittedLight().red(), 255);
    QCOMPARE(mat2.emittedLight().green(), 255);
    QCOMPARE(mat2.emittedLight().blue(), 255);
    QCOMPARE(mat2.emittedLight().alpha(), 255);
    QCOMPARE(ambientSpy.count(), 1);
    QCOMPARE(diffuseSpy.count(), 1);
    QCOMPARE(specularSpy.count(), 1);
    QCOMPARE(emittedSpy.count(), 1);
    QCOMPARE(shininessSpy.count(), 0);
    QCOMPARE(texturesSpy.count(), 0);
    QCOMPARE(materialSpy.count(), 4);
    mat2.setShininess(128);
    QCOMPARE(mat2.shininess(), 128.0f);
    QCOMPARE(ambientSpy.count(), 1);
    QCOMPARE(diffuseSpy.count(), 1);
    QCOMPARE(specularSpy.count(), 1);
    QCOMPARE(emittedSpy.count(), 1);
    QCOMPARE(shininessSpy.count(), 1);
    QCOMPARE(texturesSpy.count(), 0);
    QCOMPARE(materialSpy.count(), 5);

    // Modify to the same values and check for no further signals.
    mat2.setAmbientColor(Qt::green);
    mat2.setDiffuseColor(Qt::blue);
    mat2.setSpecularColor(Qt::cyan);
    mat2.setEmittedLight(Qt::white);
    mat2.setShininess(128);
    QCOMPARE(ambientSpy.count(), 1);
    QCOMPARE(diffuseSpy.count(), 1);
    QCOMPARE(specularSpy.count(), 1);
    QCOMPARE(emittedSpy.count(), 1);
    QCOMPARE(shininessSpy.count(), 1);
    QCOMPARE(texturesSpy.count(), 0);
    QCOMPARE(materialSpy.count(), 5);

    // Check clamping of shininess to the range 0..128.
    mat1.setShininess(-0.5f);
    QCOMPARE(mat1.shininess(), 0.0f);
    mat1.setShininess(128.5f);
    QCOMPARE(mat1.shininess(), 128.0f);

    // Test that we don't get any side effects between properties.
    QCOMPARE(mat2.ambientColor().red(), 0);
    QCOMPARE(mat2.ambientColor().green(), 255);
    QCOMPARE(mat2.ambientColor().blue(), 0);
    QCOMPARE(mat2.ambientColor().alpha(), 255);
    QCOMPARE(mat2.diffuseColor().red(), 0);
    QCOMPARE(mat2.diffuseColor().green(), 0);
    QCOMPARE(mat2.diffuseColor().blue(), 255);
    QCOMPARE(mat2.diffuseColor().alpha(), 255);
    QCOMPARE(mat2.specularColor().red(), 0);
    QCOMPARE(mat2.specularColor().green(), 255);
    QCOMPARE(mat2.specularColor().blue(), 255);
    QCOMPARE(mat2.specularColor().alpha(), 255);
    QCOMPARE(mat2.emittedLight().red(), 255);
    QCOMPARE(mat2.emittedLight().green(), 255);
    QCOMPARE(mat2.emittedLight().blue(), 255);
    QCOMPARE(mat2.emittedLight().alpha(), 255);
    QCOMPARE(mat2.shininess(), 128.0f);

    // Test the material fetch functions from QGLAbstractMaterial.
    QVERIFY(mat1.front() == &mat1);
    QVERIFY(mat1.back() == 0);
    QVERIFY(mat2.front() == &mat2);
    QVERIFY(mat2.back() == 0);

    // Test setting a pseudo-flat color via QGLMaterial::setColor().
    mat2.setColor(QColor::fromRgbF(0.2f, 0.8f, 0.5f, 0.7f));
    QVERIFY(fuzzyCompare(mat2.ambientColor().redF(), 0.2f * 0.2f));
    QVERIFY(fuzzyCompare(mat2.ambientColor().greenF(), 0.2f * 0.8f));
    QVERIFY(fuzzyCompare(mat2.ambientColor().blueF(), 0.2f * 0.5f));
    QVERIFY(fuzzyCompare(mat2.ambientColor().alphaF(), 0.7f));
    QVERIFY(fuzzyCompare(mat2.diffuseColor().redF(), 0.8f * 0.2f));
    QVERIFY(fuzzyCompare(mat2.diffuseColor().greenF(), 0.8f * 0.8f));
    QVERIFY(fuzzyCompare(mat2.diffuseColor().blueF(), 0.8f * 0.5f));
    QVERIFY(fuzzyCompare(mat2.diffuseColor().alphaF(), 0.7f));
    QCOMPARE(ambientSpy.count(), 2);
    QCOMPARE(diffuseSpy.count(), 2);
    QCOMPARE(specularSpy.count(), 1);
    QCOMPARE(emittedSpy.count(), 1);
    QCOMPARE(shininessSpy.count(), 1);
    QCOMPARE(texturesSpy.count(), 0);
    QCOMPARE(materialSpy.count(), 6);
}

void tst_QGLMaterial::textureLayers()
{
    QGLMaterial mat1;
    QCOMPARE(mat1.textureLayerCount(), 0);
    QVERIFY(mat1.texture() == 0);
    QVERIFY(mat1.texture(3) == 0);

    QSignalSpy texturesSpy(&mat1, SIGNAL(texturesChanged()));
    QSignalSpy materialSpy(&mat1, SIGNAL(materialChanged()));

    QGLTexture2D *tex1 = new QGLTexture2D(this);
    QSignalSpy tex1Spy(tex1, SIGNAL(destroyed()));
    mat1.setTexture(tex1);
    QCOMPARE(texturesSpy.count(), 1);
    QCOMPARE(materialSpy.count(), 1);
    QCOMPARE(mat1.textureLayerCount(), 1);
    QVERIFY(mat1.texture() == tex1);

    QGLTexture2D *tex2 = new QGLTexture2D(this);
    mat1.setTexture(tex2, 3);
    QCOMPARE(texturesSpy.count(), 2);
    QCOMPARE(materialSpy.count(), 2);
    QCOMPARE(mat1.textureLayerCount(), 4);
    QVERIFY(mat1.texture() == tex1);
    QVERIFY(mat1.texture(1) == 0);
    QVERIFY(mat1.texture(2) == 0);
    QVERIFY(mat1.texture(3) == tex2);

    QVERIFY(mat1.textureCombineMode() == QGLMaterial::Modulate);
    QVERIFY(mat1.textureCombineMode(2) == QGLMaterial::Modulate);
    QVERIFY(mat1.textureCombineMode(3) == QGLMaterial::Modulate);
    QVERIFY(mat1.textureCombineMode(-32) == QGLMaterial::Modulate);

    mat1.setTextureCombineMode(QGLMaterial::Decal);
    QCOMPARE(texturesSpy.count(), 3);
    QCOMPARE(materialSpy.count(), 3);

    QVERIFY(mat1.textureCombineMode() == QGLMaterial::Decal);
    QVERIFY(mat1.textureCombineMode(2) == QGLMaterial::Modulate);
    QVERIFY(mat1.textureCombineMode(3) == QGLMaterial::Modulate);

    mat1.setTextureCombineMode(QGLMaterial::Replace, 3);
    QCOMPARE(texturesSpy.count(), 4);
    QCOMPARE(materialSpy.count(), 4);

    QVERIFY(mat1.textureCombineMode() == QGLMaterial::Decal);
    QVERIFY(mat1.textureCombineMode(2) == QGLMaterial::Modulate);
    QVERIFY(mat1.textureCombineMode(3) == QGLMaterial::Replace);

    mat1.setTextureCombineMode(QGLMaterial::Modulate, 0);
    QCOMPARE(texturesSpy.count(), 5);
    QCOMPARE(materialSpy.count(), 5);

    QVERIFY(mat1.textureCombineMode() == QGLMaterial::Modulate);
    QVERIFY(mat1.textureCombineMode(2) == QGLMaterial::Modulate);
    QVERIFY(mat1.textureCombineMode(3) == QGLMaterial::Replace);

    QVERIFY(!mat1.textureUrl().isValid());
    QVERIFY(!mat1.textureUrl(2).isValid());
    QVERIFY(!mat1.textureUrl(3).isValid());

    // This will delete the texture object and replace it with a new one.
    // This texture is not intended to actually "load" as such, but rather
    // to simply test the setting of notional textures.
    mat1.setTextureUrl(QUrl(QLatin1String("foo://bar.com/texture.png")));
    QCOMPARE(tex1Spy.count(), 1);
    QCOMPARE(texturesSpy.count(), 6);
    QCOMPARE(materialSpy.count(), 6);
    QVERIFY(mat1.texture() != 0);
    QVERIFY(mat1.texture()->url() == QUrl(QLatin1String("foo://bar.com/texture.png")));

    // Setting the url to empty will delete the texture object entirely.
    QSignalSpy tex3Spy(mat1.texture(), SIGNAL(destroyed()));
    mat1.setTextureUrl(QUrl());
    QCOMPARE(tex3Spy.count(), 1);
    QCOMPARE(texturesSpy.count(), 7);
    QCOMPARE(materialSpy.count(), 7);
    QVERIFY(mat1.texture() == 0);

    // Changing properties to the same thing should do nothing.
    mat1.setTexture(tex2, 3);
    mat1.setTextureCombineMode(QGLMaterial::Replace, 3);
    mat1.setTextureUrl(QUrl());

    // Check final texture assignments and properties.
    QVERIFY(mat1.texture() == 0);
    QVERIFY(mat1.texture(1) == 0);
    QVERIFY(mat1.texture(2) == 0);
    QVERIFY(mat1.texture(3) == tex2);
    QVERIFY(mat1.textureCombineMode() == QGLMaterial::Modulate);
    QVERIFY(mat1.textureCombineMode(2) == QGLMaterial::Modulate);
    QVERIFY(mat1.textureCombineMode(3) == QGLMaterial::Replace);
    QCOMPARE(mat1.textureLayerCount(), 4);
}

void tst_QGLMaterial::colorMaterial()
{
    QGLColorMaterial mat1;
    QVERIFY(mat1.color() == QColor(Qt::white));
    QVERIFY(!mat1.isTransparent());

    QSignalSpy colorSpy(&mat1, SIGNAL(colorChanged()));
    QSignalSpy materialSpy(&mat1, SIGNAL(materialChanged()));

    mat1.setColor(Qt::red);
    QCOMPARE(colorSpy.count(), 1);
    QCOMPARE(materialSpy.count(), 1);
    QVERIFY(mat1.color() == QColor(Qt::red));

    mat1.setColor(Qt::red);
    QCOMPARE(colorSpy.count(), 1);
    QCOMPARE(materialSpy.count(), 1);

    mat1.setColor(QColor(24, 56, 98, 43));
    QVERIFY(mat1.isTransparent());
}

void tst_QGLMaterial::twoSidedMaterial()
{
    QGLTwoSidedMaterial mat1;
    QVERIFY(mat1.front() == 0);
    QVERIFY(mat1.back() == 0);

    QSignalSpy frontSpy(&mat1, SIGNAL(frontChanged()));
    QSignalSpy backSpy(&mat1, SIGNAL(backChanged()));
    QSignalSpy materialSpy(&mat1, SIGNAL(materialChanged()));

    QGLMaterial mat2;
    QGLMaterial mat3;
    mat1.setFront(&mat2);
    QCOMPARE(frontSpy.count(), 1);
    QCOMPARE(backSpy.count(), 0);
    QCOMPARE(materialSpy.count(), 1);
    mat1.setBack(&mat3);
    QCOMPARE(frontSpy.count(), 1);
    QCOMPARE(backSpy.count(), 1);
    QCOMPARE(materialSpy.count(), 2);
    QVERIFY(mat1.front() == &mat2);
    QVERIFY(mat1.back() == &mat3);

    mat2.setAmbientColor(Qt::red);
    QCOMPARE(materialSpy.count(), 3);

    mat3.setAmbientColor(Qt::green);
    QCOMPARE(materialSpy.count(), 4);

    QGLMaterial mat4;
    QGLMaterial mat5;

    mat1.setFront(&mat4);
    QCOMPARE(frontSpy.count(), 2);
    QCOMPARE(backSpy.count(), 1);
    QCOMPARE(materialSpy.count(), 5);

    mat2.setAmbientColor(Qt::blue);
    QCOMPARE(materialSpy.count(), 5);

    mat1.setBack(&mat5);
    QCOMPARE(frontSpy.count(), 2);
    QCOMPARE(backSpy.count(), 2);
    QCOMPARE(materialSpy.count(), 6);
    QVERIFY(mat1.front() == &mat4);
    QVERIFY(mat1.back() == &mat5);

    mat3.setAmbientColor(Qt::cyan);
    QCOMPARE(materialSpy.count(), 6);

    mat1.setFront(&mat4);
    mat1.setBack(&mat5);
    QCOMPARE(frontSpy.count(), 2);
    QCOMPARE(backSpy.count(), 2);
    QCOMPARE(materialSpy.count(), 6);

    mat1.setFront(0);
    QCOMPARE(frontSpy.count(), 3);
    QCOMPARE(backSpy.count(), 2);
    QCOMPARE(materialSpy.count(), 7);
    mat1.setBack(0);
    QCOMPARE(frontSpy.count(), 3);
    QCOMPARE(backSpy.count(), 3);
    QCOMPARE(materialSpy.count(), 8);
    QVERIFY(mat1.front() == 0);
    QVERIFY(mat1.back() == 0);

    mat1.setFront(&mat2);
    mat1.setBack(&mat2);
    QCOMPARE(frontSpy.count(), 4);
    QCOMPARE(backSpy.count(), 4);
    QCOMPARE(materialSpy.count(), 10);

    mat2.setAmbientColor(Qt::white);
    QCOMPARE(materialSpy.count(), 11);
}

static inline QVector4D colorToVector4D(const QColor &color)
{
    return QVector4D(color.redF(), color.greenF(),
                     color.blueF(), color.alphaF());
}

static inline float clampRange(float value)
{
    if (value < 0.0f)
        return 0.0f;
    else if (value > 1.0f)
        return 1.0f;
    else
        return value;
}

static inline QColor vector4DToColor(const QVector4D &value, float alpha)
{
    return QColor::fromRgbF(clampRange(value.x()),
                            clampRange(value.y()),
                            clampRange(value.z()),
                            alpha);
}

// Evaluate the standard GL lighting algorithm to determine what the
// lit pixel on the screen will be with the supplied parameters.
static QColor litColor(const QGLMaterial &material)
{
    QGLLightParameters light;
    QGLLightModel model;

    QVector4D color = colorToVector4D(material.emittedLight());
    color += colorToVector4D(material.ambientColor()) *
             colorToVector4D(model.ambientSceneColor());
    QVector4D scolor(0, 0, 0, 0);

    QVector3D normal(0, 0, 1);
    QVector3D vertex(-0.5f, -0.5f, 0.0f);

    QVector3D toEye;
    if (model.viewerPosition() == QGLLightModel::ViewerAtInfinity)
        toEye = QVector3D(0, 0, 1);
    else
        toEye = (-vertex).normalized();

    QVector3D toLight;
    QVector4D pli = light.eyePosition(QMatrix4x4());
    if (pli.w() == 0.0f)
        toLight = pli.toVector3D().normalized();
    else
        toLight = (pli.toVector3D() - vertex).normalized();

    float angle = qMax(QVector3D::dotProduct(normal, toLight), 0.0f);

    QVector4D adcomponent = colorToVector4D(material.ambientColor()) *
                            colorToVector4D(light.ambientColor());
    adcomponent += angle * colorToVector4D(material.diffuseColor()) *
                   colorToVector4D(light.diffuseColor());

    QVector4D scomponent;
    if (angle != 0.0f) {
        QVector3D h = (toLight + toEye).normalized();
        angle = qMax(QVector3D::dotProduct(normal, h), 0.0f);
        if (material.shininess() != 0.0f) {
            scomponent = qPow(angle, material.shininess()) *
                         colorToVector4D(material.specularColor()) *
                         colorToVector4D(light.specularColor());
        } else {
            scomponent = colorToVector4D(material.specularColor()) *
                         colorToVector4D(light.specularColor());
        }
    } else {
        scomponent = QVector4D(0, 0, 0, 0);
    }

    if (light.spotAngle() != 180.0f) {
        float spot = qMax(QVector3D::dotProduct
            (vertex - pli.toVector3D(), light.spotDirection()), 0.0f);
        if (spot < light.spotCosAngle()) {
            adcomponent = QVector4D(0.0f, 0.0f, 0.0f, 0.0f);
            scomponent = QVector4D(0.0f, 0.0f, 0.0f, 0.0f);
        } else {
            spot = qPow(spot, light.spotExponent());
            adcomponent *= spot;
            scomponent *= spot;
        }
    }

    if (pli.w() != 0.0f) {
        float attenuation = light.constantAttenuation();
        float len = (pli.toVector3D() - vertex).length();
        attenuation += light.linearAttenuation() * len;
        attenuation += light.quadraticAttenuation() * len * len;
        color += adcomponent / attenuation;
        scolor += scomponent / attenuation;
    } else {
        color += adcomponent;
        scolor += scomponent;
    }

    return vector4DToColor(color + scolor, material.diffuseColor().alphaF());
}

void tst_QGLMaterial::standardMaterialDraw()
{
    QSKIP("not currently working");

    if (!widget->context() || !widget->context()->isValid())
        QSKIP("GL Implementation not valid");

    QGLMaterial mat1;
    QVERIFY(widget->runTest(&mat1));
    QVERIFY(widget->sameColor(litColor(mat1)));
}

void tst_QGLMaterial::colorMaterialDraw()
{
    QSKIP("not currently working");

    if (!widget->context()->isValid())
        QSKIP("GL Implementation not valid");

    QGLColorMaterial mat1;
    mat1.setColor(Qt::blue);
    QVERIFY(widget->runTest(&mat1));
    QVERIFY(widget->sameColor(Qt::blue));
}

QTEST_MAIN(tst_QGLMaterial)

#include "tst_qglmaterial.moc"
