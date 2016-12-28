/****************************************************************************
**
** Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QT3DEXTRAS_QDISTANCEFIELDTEXT_H
#define QT3DEXTRAS_QDISTANCEFIELDTEXT_H

#include <QtCore/qrect.h>
#include <QtGui/qcolor.h>
#include <QtGui/qfont.h>
#include <Qt3DCore/qentity.h>
#include <Qt3DExtras/qt3dextras_global.h>

QT_BEGIN_NAMESPACE

namespace Qt3DExtras {

class QDistanceFieldGlyphCache;
class QDistanceFieldTextPrivate;

class QT3DEXTRASSHARED_EXPORT QDistanceFieldText : public Qt3DCore::QEntity
{
    Q_OBJECT
    Q_PROPERTY(QFont font READ font WRITE setFont NOTIFY fontChanged)
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
    Q_PROPERTY(QRectF position READ position WRITE setPosition NOTIFY positionChanged)
    Q_PROPERTY(float fontScale READ fontScale WRITE setFontScale NOTIFY fontScaleChanged)
    Q_PROPERTY(Qt3DExtras::QDistanceFieldGlyphCache* glyphCache READ glyphCache WRITE setGlyphCache NOTIFY glyphCacheChanged)

public:
    explicit QDistanceFieldText(Qt3DCore::QNode *parent = nullptr);
    ~QDistanceFieldText();

    QFont font() const;
    void setFont(const QFont &font);

    QColor color() const;
    void setColor(const QColor &color);

    QString text() const;
    void setText(const QString &text);

    QRectF position() const;
    void setPosition(const QRectF &position);

    float fontScale() const;
    void setFontScale(float scale);

    QDistanceFieldGlyphCache *glyphCache() const;
    void setGlyphCache(QDistanceFieldGlyphCache *glyphCache);

Q_SIGNALS:
    void fontChanged(const QFont &font);
    void colorChanged(const QColor &color);
    void textChanged(const QString &text);
    void positionChanged(const QRectF &position);
    void fontScaleChanged(float scale);
    void glyphCacheChanged(QDistanceFieldGlyphCache *glyphCache);

private:
    Q_DECLARE_PRIVATE(QDistanceFieldText)
};

} // namespace Qt3DExtras

QT_END_NAMESPACE

#endif // QT3DEXTRAS_QDISTANCEFIELDTEXT_H
