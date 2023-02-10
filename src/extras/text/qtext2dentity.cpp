// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qtext2dentity.h"
#include "qtext2dentity_p.h"
#include "qtext2dmaterial_p.h"

#include <QtGui/qtextlayout.h>
#include <QtGui/qglyphrun.h>
#include <QtGui/private/qdistancefield_p.h>
#include <QtGui/private/qtextureglyphcache_p.h>
#include <QtGui/private/qfont_p.h>
#include <QtGui/private/qdistancefield_p.h>

#include <Qt3DCore/qbuffer.h>
#include <Qt3DCore/qattribute.h>
#include <Qt3DCore/qgeometry.h>
#include <Qt3DRender/qmaterial.h>
#include <Qt3DRender/qgeometryrenderer.h>

#include <Qt3DCore/private/qscene_p.h>

QT_BEGIN_NAMESPACE

namespace {

inline constexpr QRectF scaleRectF(const QRectF &rect, qreal scale)
{
    return QRectF(rect.left() * scale, rect.top() * scale, rect.width() * scale, rect.height() * scale);
}

} // anonymous

namespace Qt3DExtras {

/*!
 * \qmltype Text2DEntity
 * \instantiates Qt3DExtras::QText2DEntity
 * \inqmlmodule Qt3D.Extras
 * \brief Text2DEntity allows creation of a 2D text in 3D space.
 *
 * The Text2DEntity renders text as triangles in the XY plane. The geometry will be fitted
 * in the rectangle of specified width and height. If the resulting geometry is wider than
 * the specified width, the remainder will be rendered on the new line.
 *
 * The entity can be positionned in the scene by adding a transform component.
 *
 * Text2DEntity will create geometry based on the shape of the glyphs and a solid
 * material using the specified color.
 *
 */

/*!
 * \qmlproperty QString Text2DEntity::text
 *
 * Holds the text used for the mesh.
 */

/*!
 * \qmlproperty QFont Text2DEntity::font
 *
 * Holds the font of the text.
 */

/*!
 * \qmlproperty QColor Text2DEntity::color
 *
 * Holds the color of the text.
 */

/*!
 * \qmlproperty float Text2DEntity::width
 *
 * Holds the width of the text's bounding rectangle.
 */

/*!
 * \qmlproperty float Text2DEntity::height
 *
 * Holds the height of the text's bounding rectangle.
 */


/*!
 * \class Qt3DExtras::QText2DEntity
 * \inheaderfile Qt3DExtras/QText2DEntity
 * \inmodule Qt3DExtras
 *
 * \brief QText2DEntity allows creation of a 2D text in 3D space.
 *
 * The QText2DEntity renders text as triangles in the XY plane. The geometry will be fitted
 * in the rectangle of specified width and height. If the resulting geometry is wider than
 * the specified width, the remainder will be rendered on the new line.
 *
 * The entity can be positionned in the scene by adding a transform component.
 *
 * QText2DEntity will create geometry based on the shape of the glyphs and a solid
 * material using the specified color.
 *
 */

QHash<Qt3DCore::QScene *, QText2DEntityPrivate::CacheEntry> QText2DEntityPrivate::m_glyphCacheInstances;

QText2DEntityPrivate::QText2DEntityPrivate()
    : m_glyphCache(nullptr)
    , m_font(QLatin1String("Times"), 10)
    , m_scaledFont(QLatin1String("Times"), 10)
    , m_color(QColor(255, 255, 255, 255))
    , m_width(0.0f)
    , m_height(0.0f)
{
}

QText2DEntityPrivate::~QText2DEntityPrivate()
{
}

void QText2DEntityPrivate::setScene(Qt3DCore::QScene *scene)
{
    if (scene == m_scene)
        return;

    // Unref old glyph cache if it exists
    if (m_scene != nullptr) {
        // Ensure we don't keep reference to glyphs
        // if we are changing the cache
        if (m_glyphCache != nullptr)
            clearCurrentGlyphRuns();

        m_glyphCache = nullptr;

        QText2DEntityPrivate::CacheEntry &entry = QText2DEntityPrivate::m_glyphCacheInstances[m_scene];
        --entry.count;
        if (entry.count == 0 && entry.glyphCache != nullptr) {

            delete entry.glyphCache;
            entry.glyphCache = nullptr;
        }
    }

    QEntityPrivate::setScene(scene);

    // Ref new glyph cache is scene is valid
    if (scene != nullptr) {
        QText2DEntityPrivate::CacheEntry &entry = QText2DEntityPrivate::m_glyphCacheInstances[scene];
        if (entry.glyphCache == nullptr) {
            entry.glyphCache = new QDistanceFieldGlyphCache();
            entry.glyphCache->setRootNode(scene->rootNode());
        }
        m_glyphCache = entry.glyphCache;
        ++entry.count;
        // Update to populate glyphCache if needed
        updateGlyphs();
    }
}

QText2DEntity::QText2DEntity(QNode *parent)
    : Qt3DCore::QEntity(*new QText2DEntityPrivate(), parent)
{
}

/*! \internal */
QText2DEntity::~QText2DEntity()
{
}

qreal QText2DEntityPrivate::computeActualScale() const
{
    // scale font based on fontScale property and given QFont
    float scale = 1.0f;
    if (m_font.pointSizeF() > 0)
        scale *= m_font.pointSizeF() / m_scaledFont.pointSizeF();
    return scale;
}

struct RenderData {
    int vertexCount = 0;
    std::vector<float> vertex;
    std::vector<quint16> index;
};

void QText2DEntityPrivate::setCurrentGlyphRuns(const QList<QGlyphRun> &runs)
{
    // For each distinct texture, we need a separate DistanceFieldTextRenderer,
    // for which we need vertex and index data
    QHash<Qt3DRender::QAbstractTexture*, RenderData> renderData;
    const qreal scale = computeActualScale();

    // process glyph runs
    for (const QGlyphRun &run : runs) {
        const auto glyphs = run.glyphIndexes();
        const auto pos = run.positions();

        Q_ASSERT(glyphs.size() == pos.size());

        const bool doubleGlyphResolution = m_glyphCache->doubleGlyphResolution(run.rawFont());

        // faithfully copied from QSGDistanceFieldGlyphNode::updateGeometry()
        const qreal pixelSize = run.rawFont().pixelSize();
        const qreal fontScale = pixelSize / QT_DISTANCEFIELD_BASEFONTSIZE(doubleGlyphResolution);
        const qreal margin = QT_DISTANCEFIELD_RADIUS(doubleGlyphResolution) / QT_DISTANCEFIELD_SCALE(doubleGlyphResolution) * fontScale;

        for (int i = 0; i < glyphs.size(); i++) {
            const QDistanceFieldGlyphCache::Glyph &dfield = m_glyphCache->refGlyph(run.rawFont(), glyphs[i]);

            if (!dfield.texture)
                continue;

            RenderData &data = renderData[dfield.texture];

            // faithfully copied from QSGDistanceFieldGlyphNode::updateGeometry()
            QRectF metrics = scaleRectF(dfield.glyphPathBoundingRect, fontScale);
            metrics.adjust(-margin, margin, margin, 3*margin);

            const qreal top = 0.0;
            const qreal left = 0.0;
            const qreal right = qreal(m_width);
            const qreal bottom = qreal(m_height);

            qreal x1 = left + scale * (pos[i].x() + metrics.left());
            qreal y2 = bottom - scale * (pos[i].y() - metrics.top());
            qreal x2 = x1 + scale * metrics.width();
            qreal y1 = y2 - scale * metrics.height();

            // only draw glyphs that are at least partly visible
            if (y2 < top || x1 > right)
                continue;

            QRectF texCoords = dfield.texCoords;

            // if a glyph is only partly visible within the given rectangle,
            // cut it in half and adjust tex coords
            if (y1 < top) {
                const auto insideRatio = (top - y2) / (y1 - y2);
                y1 = top;
                texCoords.setHeight(texCoords.height() * insideRatio);
            }

            // do the same thing horizontally
            if (x2 > right) {
                const auto insideRatio = (right - x1) / (x2 - x1);
                x2 = right;
                texCoords.setWidth(texCoords.width() * insideRatio);
            }

            for (auto v: std::vector<qreal>{x1, y1, qreal(i), texCoords.left(), texCoords.bottom()})
                data.vertex.push_back(float(v));
            for (auto v: std::vector<qreal>{x1, y2, qreal(i), texCoords.left(), texCoords.top()})
                data.vertex.push_back(float(v));
            for (auto v: std::vector<qreal>{x2, y1, qreal(i), texCoords.right(), texCoords.bottom()})
                data.vertex.push_back(float(v));
            for (auto v: std::vector<qreal>{x2, y2, qreal(i), texCoords.right(), texCoords.top()})
                data.vertex.push_back(float(v));

            for (int i: std::vector<int>{data.vertexCount, data.vertexCount + 3, data.vertexCount + 1})
                data.index.push_back(quint16(i));
            for (int i: std::vector<int>{data.vertexCount, data.vertexCount + 2, data.vertexCount + 3})
                data.index.push_back(quint16(i));

            data.vertexCount += 4;
        }
    }

    // de-ref all glyphs for previous QGlyphRuns
    for (int i = 0; i < m_currentGlyphRuns.size(); i++)
        m_glyphCache->derefGlyphs(m_currentGlyphRuns[i]);
    m_currentGlyphRuns = runs;

    // make sure we have the correct number of DistanceFieldTextRenderers
    // TODO: we might keep one renderer at all times, so we won't delete and
    // re-allocate one every time the text changes from an empty to a non-empty string
    // and vice-versa
    while (m_renderers.size() > renderData.size())
        delete m_renderers.takeLast();

    while (m_renderers.size() < renderData.size()) {
        DistanceFieldTextRenderer *renderer = new DistanceFieldTextRenderer();
        renderer->setColor(m_color);
        renderer->setParent(q_func());
        m_renderers << renderer;
    }

    Q_ASSERT(m_renderers.size() == renderData.size());

    // assign vertex data for all textures to the renderers
    int rendererIdx = 0;
    for (auto it = renderData.begin(); it != renderData.end(); ++it)
        m_renderers[rendererIdx++]->setGlyphData(it.key(), it.value().vertex, it.value().index);
}

void QText2DEntityPrivate::clearCurrentGlyphRuns()
{
    for (int i = 0; i < m_currentGlyphRuns.size(); i++)
        m_glyphCache->derefGlyphs(m_currentGlyphRuns[i]);
    m_currentGlyphRuns.clear();
}

void QText2DEntityPrivate::updateGlyphs()
{
    if (m_glyphCache == nullptr)
        return;

    QList<QGlyphRun> glyphRuns;

    // collect all GlyphRuns generated by the QTextLayout
    if ((m_width > 0.0f || m_height > 0.0f) && !m_text.isEmpty()) {
        QTextLayout layout(m_text, m_scaledFont);
        const float lineWidth = m_width / computeActualScale();
        float height = 0;
        layout.beginLayout();

        while (true) {
            QTextLine line = layout.createLine();
            if (!line.isValid())
                break;

            // position current line
            line.setLineWidth(lineWidth);
            line.setPosition(QPointF(0, height));
            height += line.height();

            // add glyph runs created by line
            const QList<QGlyphRun> runs = line.glyphRuns();
            for (const QGlyphRun &run : runs)
                glyphRuns << run;
        }

        layout.endLayout();
    }

    setCurrentGlyphRuns(glyphRuns);
}

/*!
  \property QText2DEntity::font

  Holds the font for the text item that is displayed
  in the Qt Quick scene.
*/
QFont QText2DEntity::font() const
{
    Q_D(const QText2DEntity);
    return d->m_font;
}

void QText2DEntity::setFont(const QFont &font)
{
    Q_D(QText2DEntity);
    if (d->m_font != font) {
        // ignore the point size of the font, just make it a default value.
        // still we want to make sure that font() returns the same value
        // that was passed to setFont(), so we store it nevertheless
        d->m_font = font;
        d->m_scaledFont = font;
        d->m_scaledFont.setPointSize(10);

        emit fontChanged(font);

        if (!d->m_text.isEmpty())
            d->updateGlyphs();
    }
}

/*!
  \property QText2DEntity::color

  Holds the color for the text item that is displayed in the Qt
  Quick scene.
*/
QColor QText2DEntity::color() const
{
    Q_D(const QText2DEntity);
    return d->m_color;
}

void QText2DEntity::setColor(const QColor &color)
{
    Q_D(QText2DEntity);
    if (d->m_color != color) {
        d->m_color = color;

        emit colorChanged(color);

        for (DistanceFieldTextRenderer *renderer : std::as_const(d->m_renderers))
            renderer->setColor(color);
    }
}

/*!
  \property QText2DEntity::text

  Holds the text that is displayed in the Qt Quick scene.
*/
QString QText2DEntity::text() const
{
    Q_D(const QText2DEntity);
    return d->m_text;
}

void QText2DEntity::setText(const QString &text)
{
    Q_D(QText2DEntity);
    if (d->m_text != text) {
        d->m_text = text;
        emit textChanged(text);

        d->updateGlyphs();
    }
}

/*!
  \property QText2DEntity::width

  Returns the width of the text item that is displayed in the
  Qt Quick scene.
*/
float QText2DEntity::width() const
{
    Q_D(const QText2DEntity);
    return d->m_width;
}

/*!
  \property QText2DEntity::height

  Returns the height of the text item that is displayed in the
  Qt Quick scene.
*/
float QText2DEntity::height() const
{
    Q_D(const QText2DEntity);
    return d->m_height;
}

void QText2DEntity::setWidth(float width)
{
    Q_D(QText2DEntity);
    if (width != d->m_width) {
        d->m_width = width;
        emit widthChanged(width);
        d->updateGlyphs();
    }
}

void QText2DEntity::setHeight(float height)
{
    Q_D(QText2DEntity);
    if (height != d->m_height) {
        d->m_height = height;
        emit heightChanged(height);
        d->updateGlyphs();
    }
}

} // namespace Qt3DExtras

QT_END_NAMESPACE

#include "moc_qtext2dentity.cpp"
