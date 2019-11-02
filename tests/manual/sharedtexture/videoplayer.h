/****************************************************************************
**
** Copyright (C) 2018 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QOpenGLTexture>
#include <QOpenGLShaderProgram>

#include <QtMultimedia/QAbstractVideoSurface>
#include <QtMultimedia/QMediaPlayer>

#include <memory>

class TextureWidget : public QOpenGLWidget, private QOpenGLFunctions
{
    Q_OBJECT
    Q_PROPERTY(int textureId READ textureId NOTIFY textureIdChanged)
public:
    TextureWidget(QWidget *parent = nullptr);

    int textureId() { return m_texture.textureId(); }

private:
    // MainThread
    void initializeGL() override;

    // Main thread
    void paintGL() override;

public Q_SLOTS:
    // Called from Video player thread
    void setVideoFrame(const QVideoFrame &frame);

Q_SIGNALS:
    void textureIdChanged(int textureId);

private:
    QOpenGLVertexArrayObject m_vao;
    QOpenGLShaderProgram m_shader;
    QOpenGLTexture m_texture;
};


class GLVideoSurface : public QAbstractVideoSurface
{
    Q_OBJECT
public:
    QList<QVideoFrame::PixelFormat> supportedPixelFormats(QAbstractVideoBuffer::HandleType type) const override;

    // Call in VideaPlayerThread context
    bool present(const QVideoFrame &frame) override;

Q_SIGNALS:
    void onNewFrame(const QVideoFrame &frame);
};


class VideoPlayer : public QObject
{
    Q_OBJECT
public:
    VideoPlayer(TextureWidget *textureWidget);
    ~VideoPlayer();

private:
    TextureWidget *m_textureWidget;
    std::unique_ptr<QMediaPlayer> m_player;
    std::unique_ptr<GLVideoSurface> m_surface;
};
