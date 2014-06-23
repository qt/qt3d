/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
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

#include "quick3dtexture.h"
#include <QDebug>

QT_BEGIN_NAMESPACE

namespace Qt3D {

namespace Render {

namespace Quick {


Quick3DTexture::Quick3DTexture(QObject *parent)
    : QObject(parent)
{
    parentTexture()->setTarget(Texture::Target2D);
    parentTexture()->setInternalFormat(Texture::RGBA8_UNorm);
}

QUrl Quick3DTexture::source() const
{
    return m_source;
}

void Quick3DTexture::setSource(QUrl arg)
{
    if (m_source != arg) {
        m_source = arg;
        // There is maybe a better way to check for resources files
        if (m_source.isLocalFile() || m_source.scheme() == QStringLiteral("qrc")) {
            QString source = m_source.toString().replace(QStringLiteral("qrc"), QStringLiteral(""));
            QImage img;
            if (img.load(source)) {
                parentTexture()->setInternalFormat(img.hasAlphaChannel() ?
                                                       Texture::RGBA8_UNorm :
                                                       Texture::RGB8_UNorm);
                parentTexture()->setFromQImage(img);
            }
            else {
                qWarning() << "Failed to load image : " << source;
            }

        } else {
            qWarning() << "implement loading from remote URLs";
        }
        emit sourceChanged();
    }
}

void Quick3DTexture::setRectangle(bool r)
{
    parentTexture()->setTarget(r ? Texture::TargetRectangle :
                                   Texture::Target2D);
}

bool Quick3DTexture::isRectangle() const
{
    return (parentTexture()->target() == Texture::TargetRectangle);
}

} // Quick

} // Render

} // Qt3D

QT_END_NAMESPACE
