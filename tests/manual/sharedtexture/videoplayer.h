// Copyright (C) 2018 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include <QtGui/QOpenGLFunctions>

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QtOpenGLWidgets/QOpenGLWidget>
#else
#include <QtWidgets/QOpenGLWidget>
#endif

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
