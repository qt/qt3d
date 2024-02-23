// Copyright (C) 2018 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include <QMutexLocker>
#include <QtMultimedia/QVideoFrame>

#include "videoplayer.h"

TextureWidget::TextureWidget(QWidget *parent)
    : QOpenGLWidget(parent)
    , m_texture(QOpenGLTexture::Target2D)
{
}

// Main thread
void TextureWidget::initializeGL()
{
    initializeOpenGLFunctions();
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    if (!m_shader.addShaderFromSourceCode(QOpenGLShader::Vertex,
                                          "#version 330\n"
                                          "out vec2 coords;\n"
                                          "const vec2 positions[6] = vec2[] ("
                                          "    vec2(-1.0, 1.0),"
                                          "    vec2(-1.0, -1.0),"
                                          "    vec2(1.0, 1.0),"
                                          "    vec2(1.0, 1.0),"
                                          "    vec2(-1.0, -1.0),"
                                          "    vec2(1.0, -1.0));\n"
                                          "const vec2 texCoords[6] = vec2[] ("
                                          "    vec2(0.0, 0.0),"
                                          "    vec2(0.0, 1.0),"
                                          "    vec2(1.0, 0.0),"
                                          "    vec2(1.0, 0.0),"
                                          "    vec2(0.0, 1.0),"
                                          "    vec2(1.0, 1.0));\n"
                                          "void main() {\n"
                                          "    coords = texCoords[gl_VertexID];\n"
                                          "    gl_Position = vec4(positions[gl_VertexID], 0.0, 1.0);\n"
                                          "}"))
        qDebug() << "Failed to load vertex shader" << m_shader.log();
    if (!m_shader.addShaderFromSourceCode(QOpenGLShader::Fragment,
                                          "#version 330\n"
                                          "in vec2 coords;\n"
                                          "uniform sampler2D video_texture;\n"
                                          "out vec4 fragColor;\n"
                                          "void main() {\n"
                                          "    fragColor = texture(video_texture, coords);\n"
                                          "}"))
        qDebug() << "Failed to load fragment shader" << m_shader.log();
    if (!m_shader.link())
        qDebug() << "Failed to link shaders" << m_shader.log();

    qDebug() << Q_FUNC_INFO << context()->shareContext();

    m_vao.create();
}

// Main thread
void TextureWidget::paintGL()
{
    glViewport(0, 0, width(), height());

    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    if (!m_texture.isCreated())
        return;

    m_shader.bind();

    m_texture.bind(0);
    m_shader.setUniformValue("video_texture", 0);

    m_vao.bind();
    glDrawArrays(GL_TRIANGLES, 0, 6);

    m_vao.release();
    m_shader.release();
}

void TextureWidget::setVideoFrame(const QVideoFrame &frame)
{
    QVideoFrame f = frame;

    // Map frame
    if (!f.map(QAbstractVideoBuffer::ReadOnly))
        return;

    makeCurrent();

    // Create or recreate texture
    if (m_texture.width() != f.width() || m_texture.height() != f.height()) {
        if (m_texture.isCreated())
            m_texture.destroy();

        m_texture.setSize(f.width(), f.height());
        m_texture.setFormat(QOpenGLTexture::RGBA32F);
        m_texture.setWrapMode(QOpenGLTexture::ClampToBorder);
        m_texture.setMinificationFilter(QOpenGLTexture::Nearest);
        m_texture.setMagnificationFilter(QOpenGLTexture::Nearest);
        m_texture.allocateStorage();

        m_texture.create();
        emit textureIdChanged(m_texture.textureId());
    }

    const QVideoFrame::PixelFormat pFormat = f.pixelFormat();
    if (pFormat == QVideoFrame::Format_RGB32) {
        m_texture.setData(QOpenGLTexture::RGBA, QOpenGLTexture::UInt8, f.bits());
    }

    doneCurrent();

    // Request display udpate
    QOpenGLWidget::update();

    // Unmap
    f.unmap();
}

QList<QVideoFrame::PixelFormat> GLVideoSurface::supportedPixelFormats(QAbstractVideoBuffer::HandleType type) const
{
    if (type == QAbstractVideoBuffer::NoHandle)
        return {
            QVideoFrame::Format_RGB32,
            QVideoFrame::Format_ARGB32,
            QVideoFrame::Format_BGR32,
            QVideoFrame::Format_BGRA32
        };
    return {};
}

// Video player thread
bool GLVideoSurface::present(const QVideoFrame &frame)
{
    emit onNewFrame(frame);
    return true;
}

VideoPlayer::VideoPlayer(TextureWidget *textureWidget)
    : QObject(textureWidget)
    , m_player(new QMediaPlayer(nullptr, QMediaPlayer::VideoSurface))
    , m_surface(new GLVideoSurface())
{
    m_player->setMedia(QUrl("https://www.sample-videos.com/video123/mp4/720/big_buck_bunny_720p_1mb.mp4"));

    // Tell player to render on GLVideoSurface
    m_player->setVideoOutput(m_surface.get());

    // Display errors
    QObject::connect(m_player.get(), QOverload<QMediaPlayer::Error>::of(&QMediaPlayer::error),
                     m_player.get(), [this] (QMediaPlayer::Error e) {
        qDebug() << Q_FUNC_INFO << e << m_player->errorString();
    });

    // Repeat video indefinitely
    QObject::connect(m_player.get(), &QMediaPlayer::stateChanged, m_player.get(), [this] (QMediaPlayer::State state) {
        if (state == QMediaPlayer::StoppedState)
            m_player->play();
    });

    QObject::connect(m_surface.get(), &GLVideoSurface::onNewFrame,
                     textureWidget, &TextureWidget::setVideoFrame, Qt::DirectConnection);

    // Start playing
    m_player->play();
}

VideoPlayer::~VideoPlayer()
{
}
