/****************************************************************************
**
** Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
****************************************************************************/

#ifndef QT3DRENDER_QPAINTEDTEXTURE_H
#define QT3DRENDER_QPAINTEDTEXTURE_H

#include <Qt3DRender/qabstracttextureimage.h>

QT_BEGIN_NAMESPACE

class QPainter;

namespace Qt3DRender {

class QPaintedTextureImagePrivate;

class Q_3DRENDERSHARED_EXPORT QPaintedTextureImage : public QAbstractTextureImage
{
    Q_OBJECT
    Q_PROPERTY(int width READ width WRITE setWidth NOTIFY widthChanged)
    Q_PROPERTY(int height READ height WRITE setHeight NOTIFY heightChanged)
    Q_PROPERTY(QSize size READ size WRITE setSize NOTIFY sizeChanged)

public:
    explicit QPaintedTextureImage(Qt3DCore::QNode *parent = nullptr);
    ~QPaintedTextureImage();

    int width() const;
    int height() const;
    QSize size() const;

    void update(const QRect &rect = QRect());

public Q_SLOTS:
    void setWidth(int w);
    void setHeight(int h);
    void setSize(QSize size);

Q_SIGNALS:
    void widthChanged(int w);
    void heightChanged(int w);
    void sizeChanged(QSize size);

protected:
    virtual void paint(QPainter *painter) = 0;

private:
    Q_DECLARE_PRIVATE(QPaintedTextureImage)

    QTextureImageDataGeneratorPtr dataGenerator() const override;
};

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QPAINTEDTEXTURE_H
