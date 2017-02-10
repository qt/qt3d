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

#ifndef QT3DEXTRAS_QDISTANCEFIELDGLYPHCACHE_P_H
#define QT3DEXTRAS_QDISTANCEFIELDGLYPHCACHE_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of other Qt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QtCore/qscopedpointer.h>
#include <QtGui/qglyphrun.h>
#include <QtGui/qfont.h>
#include <QtGui/private/qdistancefield_p.h>
#include <Qt3DCore/private/qnode_p.h>
#include <Qt3DExtras/qdistancefieldglyphcache.h>
#include <Qt3DExtras/qtextureatlas.h>

QT_BEGIN_NAMESPACE

namespace Qt3DExtras {

// ref-count glyphs and keep track of where they are stored
class StoredGlyph {
public:
    StoredGlyph() = default;
    StoredGlyph(const StoredGlyph &other) = default;
    StoredGlyph(const QRawFont &font, quint32 glyph, bool doubleResolution);

    int refCount() const { return m_ref; }
    void ref() { ++m_ref; }
    int deref() { return m_ref = std::max(m_ref - 1, (quint32) 0); }

    bool addToTextureAtlas(QTextureAtlas *atlas);
    void removeFromTextureAtlas();

    QTextureAtlas *atlas() const { return m_atlas; }
    QRectF glyphPathBoundingRect() const { return m_glyphPathBoundingRect; }
    QRectF texCoords() const;

private:
    quint32 m_glyph = (quint32) -1;
    quint32 m_ref = 0;
    QTextureAtlas *m_atlas = nullptr;
    QTextureAtlas::TextureId m_atlasEntry = QTextureAtlas::InvalidTexture;
    QRectF m_glyphPathBoundingRect;
    QImage m_distanceFieldImage;    // only used until added to texture atlas
};

// A DistanceFieldFont stores all glyphs for a given QRawFont.
// it will use multiple QTextureAtlasess to store the distance
// fields and uses ref-counting for each glyph to ensure that
// unused glyphs are removed from the texture atlasses.
class DistanceFieldFont
{
public:
    DistanceFieldFont(const QRawFont &font, bool doubleRes, Qt3DCore::QNode *parent);
    ~DistanceFieldFont();

    StoredGlyph findGlyph(quint32 glyph) const;
    StoredGlyph refGlyph(quint32 glyph);
    void derefGlyph(quint32 glyph);

    bool doubleGlyphResolution() const { return m_doubleGlyphResolution; }

private:
    QRawFont m_font;
    bool m_doubleGlyphResolution;
    Qt3DCore::QNode *m_parentNode; // parent node for the QTextureAtlasses

    QHash<quint32, StoredGlyph> m_glyphs;

    QVector<QTextureAtlas*> m_atlasses;
};

class QDistanceFieldGlyphCachePrivate : public Qt3DCore::QNodePrivate
{
public:
    Q_DECLARE_PUBLIC(QDistanceFieldGlyphCache)

    DistanceFieldFont* getOrCreateDistanceFieldFont(const QRawFont &font);

private:
    static QString fontKey(const QRawFont &font);

    QHash<QString, DistanceFieldFont*> m_fonts;
};

} // namespace Qt3DExtras

QT_END_NAMESPACE

#endif // QT3DEXTRAS_QDISTANCEFIELDGLYPHCACHE_P_H
