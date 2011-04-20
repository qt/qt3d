/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtQuick3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtTest/QtTest>
#include "qopenglfunctions.h"

class tst_QOpenGLFunctions : public QObject
{
    Q_OBJECT
public:
    tst_QOpenGLFunctions() {}
    ~tst_QOpenGLFunctions() {}

private slots:
    void features();
    void multitexture();
    void blendColor();

private:
    static bool hasExtension(const char *name);
};

bool tst_QOpenGLFunctions::hasExtension(const char *name)
{
    QString extensions =
        QString::fromLatin1
            (reinterpret_cast<const char *>(glGetString(GL_EXTENSIONS)));
    return extensions.split(QLatin1Char(' ')).contains
        (QString::fromLatin1(name));
}

// Check that the reported features are consistent with the platform.
void tst_QOpenGLFunctions::features()
{
    // Before being associated with a context, there should be
    // no features enabled.
    QOpenGLFunctions funcs;
    QVERIFY(!funcs.openGLFeatures());
    QVERIFY(!funcs.hasOpenGLFeature(QOpenGLFunctions::Multitexture));
    QVERIFY(!funcs.hasOpenGLFeature(QOpenGLFunctions::Shaders));
    QVERIFY(!funcs.hasOpenGLFeature(QOpenGLFunctions::Buffers));
    QVERIFY(!funcs.hasOpenGLFeature(QOpenGLFunctions::Framebuffers));
    QVERIFY(!funcs.hasOpenGLFeature(QOpenGLFunctions::BlendColor));
    QVERIFY(!funcs.hasOpenGLFeature(QOpenGLFunctions::BlendEquation));
    QVERIFY(!funcs.hasOpenGLFeature(QOpenGLFunctions::BlendEquationSeparate));
    QVERIFY(!funcs.hasOpenGLFeature(QOpenGLFunctions::BlendFuncSeparate));
    QVERIFY(!funcs.hasOpenGLFeature(QOpenGLFunctions::BlendSubtract));
    QVERIFY(!funcs.hasOpenGLFeature(QOpenGLFunctions::CompressedTextures));
    QVERIFY(!funcs.hasOpenGLFeature(QOpenGLFunctions::Multisample));
    QVERIFY(!funcs.hasOpenGLFeature(QOpenGLFunctions::StencilSeparate));
    QVERIFY(!funcs.hasOpenGLFeature(QOpenGLFunctions::NPOTTextures));

    // Make a context current.
    QGLWidget glw;
    if (!glw.isValid())
        QSKIP("GL Implementation not valid", SkipSingle);

    glw.makeCurrent();
    funcs.initializeGLFunctions();

    // Validate the features against what we expect for this platform.
#if defined(QT_OPENGL_ES_2)
    QOpenGLFunctions::OpenGLFeatures allFeatures =
        (QOpenGLFunctions::Multitexture |
         QOpenGLFunctions::Shaders |
         QOpenGLFunctions::Buffers |
         QOpenGLFunctions::Framebuffers |
         QOpenGLFunctions::BlendColor |
         QOpenGLFunctions::BlendEquation |
         QOpenGLFunctions::BlendEquationSeparate |
         QOpenGLFunctions::BlendFuncSeparate |
         QOpenGLFunctions::BlendSubtract |
         QOpenGLFunctions::CompressedTextures |
         QOpenGLFunctions::Multisample |
         QOpenGLFunctions::StencilSeparate |
         QOpenGLFunctions::NPOTTextures);
    QVERIFY((funcs.openGLFeatures() & allFeatures) == allFeatures);
    QVERIFY(funcs.hasOpenGLFeature(QOpenGLFunctions::Multitexture));
    QVERIFY(funcs.hasOpenGLFeature(QOpenGLFunctions::Shaders));
    QVERIFY(funcs.hasOpenGLFeature(QOpenGLFunctions::Buffers));
    QVERIFY(funcs.hasOpenGLFeature(QOpenGLFunctions::Framebuffers));
    QVERIFY(funcs.hasOpenGLFeature(QOpenGLFunctions::BlendColor));
    QVERIFY(funcs.hasOpenGLFeature(QOpenGLFunctions::BlendEquation));
    QVERIFY(funcs.hasOpenGLFeature(QOpenGLFunctions::BlendEquationSeparate));
    QVERIFY(funcs.hasOpenGLFeature(QOpenGLFunctions::BlendFuncSeparate));
    QVERIFY(funcs.hasOpenGLFeature(QOpenGLFunctions::BlendSubtract));
    QVERIFY(funcs.hasOpenGLFeature(QOpenGLFunctions::CompressedTextures));
    QVERIFY(funcs.hasOpenGLFeature(QOpenGLFunctions::Multisample));
    QVERIFY(funcs.hasOpenGLFeature(QOpenGLFunctions::StencilSeparate));
    QVERIFY(funcs.hasOpenGLFeature(QOpenGLFunctions::NPOTTextures));
#elif defined(QT_OPENGL_ES)
    QVERIFY(funcs.hasOpenGLFeature(QOpenGLFunctions::Multitexture));
    QVERIFY(funcs.hasOpenGLFeature(QOpenGLFunctions::Buffers));
    QVERIFY(funcs.hasOpenGLFeature(QOpenGLFunctions::CompressedTextures));
    QVERIFY(funcs.hasOpenGLFeature(QOpenGLFunctions::Multisample));

    QVERIFY(!funcs.hasOpenGLFeature(QOpenGLFunctions::Shaders));
    QVERIFY(!funcs.hasOpenGLFeature(QOpenGLFunctions::BlendColor));
    QVERIFY(!funcs.hasOpenGLFeature(QOpenGLFunctions::StencilSeparate));

    QCOMPARE(funcs.hasOpenGLFeature(QOpenGLFunctions::Framebuffers),
             hasExtension("GL_OES_framebuffer_object"));
    QCOMPARE(funcs.hasOpenGLFeature(QOpenGLFunctions::BlendEquationSeparate),
             hasExtension("GL_OES_blend_equation_separate"));
    QCOMPARE(funcs.hasOpenGLFeature(QOpenGLFunctions::BlendFuncSeparate),
             hasExtension("GL_OES_blend_func_separate"));
    QCOMPARE(funcs.hasOpenGLFeature(QOpenGLFunctions::BlendSubtract),
             hasExtension("GL_OES_blend_subtract"));
    QCOMPARE(funcs.hasOpenGLFeature(QOpenGLFunctions::NPOTTextures),
             hasExtension("GL_OES_texture_npot"));
#else
    // We check for both the extension name and the minimum OpenGL version
    // for the feature.  This will help us catch situations where a platform
    // doesn't list an extension by name but does have the feature by virtue
    // of its version number.
    QGLFormat::OpenGLVersionFlags versions = QGLFormat::openGLVersionFlags();
    QCOMPARE(funcs.hasOpenGLFeature(QOpenGLFunctions::Multitexture),
             hasExtension("GL_ARB_multitexture") ||
             (versions & QGLFormat::OpenGL_Version_1_3) != 0);
    QCOMPARE(funcs.hasOpenGLFeature(QOpenGLFunctions::Shaders),
             hasExtension("GL_ARB_shader_objects") ||
             (versions & QGLFormat::OpenGL_Version_2_0) != 0);
    QCOMPARE(funcs.hasOpenGLFeature(QOpenGLFunctions::Buffers),
             (versions & QGLFormat::OpenGL_Version_1_5) != 0);
    QCOMPARE(funcs.hasOpenGLFeature(QOpenGLFunctions::Framebuffers),
             hasExtension("GL_EXT_framebuffer_object") ||
             hasExtension("GL_ARB_framebuffer_object"));
    QCOMPARE(funcs.hasOpenGLFeature(QOpenGLFunctions::BlendColor),
             hasExtension("GL_EXT_blend_color") ||
             (versions & QGLFormat::OpenGL_Version_1_2) != 0);
    QCOMPARE(funcs.hasOpenGLFeature(QOpenGLFunctions::BlendEquation),
             (versions & QGLFormat::OpenGL_Version_1_2) != 0);
    QCOMPARE(funcs.hasOpenGLFeature(QOpenGLFunctions::BlendEquationSeparate),
             hasExtension("GL_EXT_blend_equation_separate") ||
             (versions & QGLFormat::OpenGL_Version_2_0) != 0);
    QCOMPARE(funcs.hasOpenGLFeature(QOpenGLFunctions::BlendFuncSeparate),
             hasExtension("GL_EXT_blend_func_separate") ||
             (versions & QGLFormat::OpenGL_Version_1_4) != 0);
    QCOMPARE(funcs.hasOpenGLFeature(QOpenGLFunctions::BlendSubtract),
             hasExtension("GL_EXT_blend_subtract"));
    QCOMPARE(funcs.hasOpenGLFeature(QOpenGLFunctions::CompressedTextures),
             hasExtension("GL_ARB_texture_compression") ||
             (versions & QGLFormat::OpenGL_Version_1_3) != 0);
    QCOMPARE(funcs.hasOpenGLFeature(QOpenGLFunctions::Multisample),
             hasExtension("GL_ARB_multisample") ||
             (versions & QGLFormat::OpenGL_Version_1_3) != 0);
    QCOMPARE(funcs.hasOpenGLFeature(QOpenGLFunctions::StencilSeparate),
             (versions & QGLFormat::OpenGL_Version_2_0) != 0);
    QCOMPARE(funcs.hasOpenGLFeature(QOpenGLFunctions::NPOTTextures),
             hasExtension("GL_ARB_texture_non_power_of_two") ||
             (versions & QGLFormat::OpenGL_Version_2_0) != 0);
#endif
}

// Verify that the multitexture functions appear to resolve and work.
void tst_QOpenGLFunctions::multitexture()
{
    QOpenGLFunctions funcs;
    QGLWidget glw;
    if (!glw.isValid())
        QSKIP("GL Implementation not valid", SkipSingle);
    glw.makeCurrent();
    funcs.initializeGLFunctions();

    if (!funcs.hasOpenGLFeature(QOpenGLFunctions::Multitexture))
        QSKIP("Multitexture functions are not supported", SkipSingle);

    funcs.glActiveTexture(GL_TEXTURE1);

    GLint active = 0;
    glGetIntegerv(GL_ACTIVE_TEXTURE, &active);
    QVERIFY(active == GL_TEXTURE1);

    funcs.glActiveTexture(GL_TEXTURE0);

    active = 0;
    glGetIntegerv(GL_ACTIVE_TEXTURE, &active);
    QVERIFY(active == GL_TEXTURE0);
}

// Verify that the glBlendColor() function appears to resolve and work.
void tst_QOpenGLFunctions::blendColor()
{
    QOpenGLFunctions funcs;
    QGLWidget glw;
    if (!glw.isValid())
        QSKIP("GL Implementation not valid", SkipSingle);
    glw.makeCurrent();
    funcs.initializeGLFunctions();

    if (!funcs.hasOpenGLFeature(QOpenGLFunctions::BlendColor))
        QSKIP("glBlendColor() is not supported", SkipSingle);

    funcs.glBlendColor(0.0f, 1.0f, 0.0f, 1.0f);

    GLfloat colors[4] = {0.5f, 0.5f, 0.5f, 0.5f};
    glGetFloatv(GL_BLEND_COLOR, colors);

    QCOMPARE(colors[0], 0.0f);
    QCOMPARE(colors[1], 1.0f);
    QCOMPARE(colors[2], 0.0f);
    QCOMPARE(colors[3], 1.0f);
}

QTEST_MAIN(tst_QOpenGLFunctions)

#include "tst_qopenglfunctions.moc"
