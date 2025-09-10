// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DEXTRAS_QTEXT2DENTITY_P_H
#define QT3DEXTRAS_QTEXT2DENTITY_P_H

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

#include <Qt3DCore/private/qentity_p.h>
#include <Qt3DExtras/private/distancefieldtextrenderer_p.h>
#include <Qt3DExtras/private/qdistancefieldglyphcache_p.h>
#include <QFont>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {
class QScene;
}

namespace Qt3DRender {
class QGeometryRenderer;
class QGeometry;
class QMaterial;
class QAttribute;
class QBuffer;
}

namespace Qt3DExtras {

class QText2DMaterial;
class QText2DEntity;

class QText2DEntityPrivate : public Qt3DCore::QEntityPrivate
{
public:
    QText2DEntityPrivate();
    ~QText2DEntityPrivate();

    Q_DECLARE_PUBLIC(QText2DEntity)

    // keep track of the glyphs currently being displayed,
    // to guarantee proper glyph ref-counting in the
    // QDistanceFieldGlyphCache
    QList<QGlyphRun> m_currentGlyphRuns;
    QDistanceFieldGlyphCache *m_glyphCache;

    void setScene(Qt3DCore::QScene *scene) override;

    QFont m_font;
    QFont m_scaledFont; // ignore point or pixel size, set to default value

    QColor m_color;
    QString m_text;
    float m_width;
    float m_height;
    Qt::Alignment m_alignment;

    QList<DistanceFieldTextRenderer*> m_renderers;

    qreal computeActualScale() const;

    void setCurrentGlyphRuns(const QList<QGlyphRun> &runs);
    void clearCurrentGlyphRuns();
    void updateGlyphs();

    struct CacheEntry
    {
        QDistanceFieldGlyphCache *glyphCache = nullptr;
        int count = 0;
    };

    static QHash<Qt3DCore::QScene *, CacheEntry> m_glyphCacheInstances;
};

} // namespace Qt3DExtras

QT_END_NAMESPACE

#endif // QT3DEXTRAS_QTEXT2DENTITY_P_H
