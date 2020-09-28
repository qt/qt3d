/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
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

#include "qt3dquick_global_p.h"

#include <Qt3DQuick/private/qt3dquicknodefactory_p.h>
#include <Qt3DQuick/private/qt3dquickvaluetypes_p.h>
#include <QtQml/private/qqmlglobal_p.h>
#include <QtQml/private/qv4engine_p.h>
#include <QtQml/private/qv4object_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {
namespace Quick {

class Quick3DColorProvider : public QQmlColorProvider
{
public:
    QVariant colorFromString(const QString &s, bool *ok) override
    {
        QColor c(s);
        if (c.isValid()) {
            if (ok) *ok = true;
            return QVariant(c);
        }

        if (ok) *ok = false;
        return QVariant();
    }

    unsigned rgbaFromString(const QString &s, bool *ok) override
    {
        QColor c(s);
        if (c.isValid()) {
            if (ok) *ok = true;
            return c.rgba();
        }

        if (ok) *ok = false;
        return 0;
    }

    QString stringFromRgba(unsigned rgba)
    {
        QColor c(QColor::fromRgba(rgba));
        if (c.isValid()) {
            return QVariant(c).toString();
        }

        return QString();
    }

    QVariant fromRgbF(double r, double g, double b, double a) override
    {
        return QVariant(QColor::fromRgbF(r, g, b, a));
    }

    QVariant fromHslF(double h, double s, double l, double a) override
    {
        return QVariant(QColor::fromHslF(h, s, l, a));
    }

    QVariant fromHsvF(double h, double s, double v, double a) override
    {
        return QVariant(QColor::fromHsvF(h, s, v, a));
    }

    QVariant lighter(const QVariant &var, qreal factor) override
    {
        QColor color = var.value<QColor>();
        color = color.lighter(int(qRound(factor*100.)));
        return QVariant::fromValue(color);
    }

    QVariant darker(const QVariant &var, qreal factor) override
    {
        QColor color = var.value<QColor>();
        color = color.darker(int(qRound(factor*100.)));
        return QVariant::fromValue(color);
    }

    QVariant tint(const QVariant &baseVar, const QVariant &tintVar) override
    {
        QColor tintColor = tintVar.value<QColor>();

        int tintAlpha = tintColor.alpha();
        if (tintAlpha == 0xFF) {
            return tintVar;
        } else if (tintAlpha == 0x00) {
            return baseVar;
        }

        // tint the base color and return the final color
        QColor baseColor = baseVar.value<QColor>();
        qreal a = tintColor.alphaF();
        qreal inv_a = 1.0 - a;

        qreal r = tintColor.redF() * a + baseColor.redF() * inv_a;
        qreal g = tintColor.greenF() * a + baseColor.greenF() * inv_a;
        qreal b = tintColor.blueF() * a + baseColor.blueF() * inv_a;

        return QVariant::fromValue(QColor::fromRgbF(r, g, b, a + inv_a * baseColor.alphaF()));
    }
};


// Note: The functions in this class provide handling only for the types
// that the QML engine will currently actually call them for, so many
// appear incompletely implemented.  For some functions, the implementation
// would be obvious, but for others (particularly create and createFromString)
// the exact semantics are unknown.  For this reason unused functionality
// has been omitted.

class Quick3DValueTypeProvider : public QQmlValueTypeProvider
{
public:

#if defined(QT_NO_DEBUG) && !defined(QT_FORCE_ASSERTS)
    #define ASSERT_VALID_SIZE(size, min) Q_UNUSED(size);
#else
    #define ASSERT_VALID_SIZE(size, min) Q_ASSERT(size >= min)
#endif

    static QVector2D vector2DFromString(const QString &s, bool *ok)
    {
        if (s.count(QLatin1Char(',')) == 1) {
            int index = s.indexOf(QLatin1Char(','));

            bool xGood, yGood;
            float xCoord = QStringView{s}.left(index).toFloat(&xGood);
            float yCoord = QStringView{s}.mid(index + 1).toFloat(&yGood);

            if (xGood && yGood) {
                if (ok) *ok = true;
                return QVector2D(xCoord, yCoord);
            }
        }

        if (ok) *ok = false;
        return QVector2D();
    }

    static QVector3D vector3DFromString(const QString &s, bool *ok)
    {
        if (s.count(QLatin1Char(',')) == 2) {
            int index = s.indexOf(QLatin1Char(','));
            int index2 = s.indexOf(QLatin1Char(','), index+1);

            bool xGood, yGood, zGood;
            float xCoord = QStringView{s}.left(index).toFloat(&xGood);
            float yCoord = QStringView{s}.mid(index + 1, index2 - index - 1).toFloat(&yGood);
            float zCoord = QStringView{s}.mid(index2 + 1).toFloat(&zGood);

            if (xGood && yGood && zGood) {
                if (ok) *ok = true;
                return QVector3D(xCoord, yCoord, zCoord);
            }
        }

        if (ok) *ok = false;
        return QVector3D();
    }

    static QVector4D vector4DFromString(const QString &s, bool *ok)
    {
        if (s.count(QLatin1Char(',')) == 3) {
            int index = s.indexOf(QLatin1Char(','));
            int index2 = s.indexOf(QLatin1Char(','), index+1);
            int index3 = s.indexOf(QLatin1Char(','), index2+1);

            bool xGood, yGood, zGood, wGood;
            float xCoord = QStringView{s}.left(index).toFloat(&xGood);
            float yCoord = QStringView{s}.mid(index + 1, index2 - index - 1).toFloat(&yGood);
            float zCoord = QStringView{s}.mid(index2 + 1, index3 - index2 - 1).toFloat(&zGood);
            float wCoord = QStringView{s}.mid(index3 + 1).toFloat(&wGood);

            if (xGood && yGood && zGood && wGood) {
                if (ok) *ok = true;
                return QVector4D(xCoord, yCoord, zCoord, wCoord);
            }
        }

        if (ok) *ok = false;
        return QVector4D();
    }

    static QQuaternion quaternionFromString(const QString &s, bool *ok)
    {
        if (s.count(QLatin1Char(',')) == 3) {
            int index = s.indexOf(QLatin1Char(','));
            int index2 = s.indexOf(QLatin1Char(','), index+1);
            int index3 = s.indexOf(QLatin1Char(','), index2+1);

            bool sGood, xGood, yGood, zGood;
            qreal sCoord = QStringView{s}.left(index).toDouble(&sGood);
            qreal xCoord = QStringView{s}.mid(index+1, index2-index-1).toDouble(&xGood);
            qreal yCoord = QStringView{s}.mid(index2+1, index3-index2-1).toDouble(&yGood);
            qreal zCoord = QStringView{s}.mid(index3+1).toDouble(&zGood);

            if (sGood && xGood && yGood && zGood) {
                if (ok) *ok = true;
                return QQuaternion(sCoord, xCoord, yCoord, zCoord);
            }
        }

        if (ok) *ok = false;
        return QQuaternion();
    }

    static QMatrix4x4 matrix4x4FromString(const QString &s, bool *ok)
    {
        if (s.count(QLatin1Char(',')) == 15) {
            float matValues[16];
            bool vOK = true;
            QStringView mutableStr(s);
            for (int i = 0; vOK && i < 16; ++i) {
                int cidx = mutableStr.indexOf(QLatin1Char(','));
                matValues[i] = mutableStr.left(cidx).toDouble(&vOK);
                mutableStr = mutableStr.mid(cidx + 1);
            }

            if (vOK) {
                if (ok) *ok = true;
                return QMatrix4x4(matValues);
            }
        }

        if (ok) *ok = false;
        return QMatrix4x4();
    }

    bool create(int type, const QJSValue &params, QVariant *v) override
    {
        switch (type) {
        case QMetaType::QColor:
            if (params.isString()) {
                *v = QVariant(QColor(params.toString()));
                return true;
            }
            break;
        case QMetaType::QVector2D:
            if (params.isArray()) {
                *v = QVariant(QVector2D(params.property(0).toNumber(),
                                        params.property(1).toNumber()));
                return true;
            } else if (params.isString()) {
                bool ok = false;
                auto vector = vector2DFromString(params.toString(), &ok);
                if (ok) {
                    *v = QVariant(vector);
                    return true;
                }
            }
            break;
        case QMetaType::QVector3D:
            if (params.isArray()) {
                *v = QVariant(QVector3D(params.property(0).toNumber(),
                                        params.property(1).toNumber(),
                                        params.property(2).toNumber()));
                return true;
            } else if (params.isString()) {
                bool ok = false;
                auto vector = vector3DFromString(params.toString(), &ok);
                if (ok) {
                    *v = QVariant(vector);
                    return true;
                }
            }
            break;
        case QMetaType::QVector4D:
            if (params.isArray()) {
                *v = QVariant(QVector4D(params.property(0).toNumber(),
                                        params.property(1).toNumber(),
                                        params.property(2).toNumber(),
                                        params.property(3).toNumber()));
                return true;
            } else if (params.isString()) {
                bool ok = false;
                auto vector = vector4DFromString(params.toString(), &ok);
                if (ok) {
                    *v = QVariant(vector);
                    return true;
                }
            }
            break;
        case QMetaType::QQuaternion:
            if (params.isArray()) {
                *v = QVariant(QQuaternion(params.property(0).toNumber(),
                                          params.property(1).toNumber(),
                                          params.property(2).toNumber(),
                                          params.property(3).toNumber()));
                return true;
            } else if (params.isString()) {
                bool ok = false;
                auto vector = quaternionFromString(params.toString(), &ok);
                if (ok) {
                    *v = QVariant(vector);
                    return true;
                }
            }
            break;
        case QMetaType::QMatrix4x4:
            if (params.isNull() || params.isUndefined()) {
                QMatrix4x4 m;
                *v = QVariant(m);
                return true;
            } else if (params.isArray()
                       && params.property(QStringLiteral("length")).toInt() == 16) {
                *v = QVariant(QMatrix4x4(params.property(0).toNumber(),
                                         params.property(1).toNumber(),
                                         params.property(2).toNumber(),
                                         params.property(3).toNumber(),
                                         params.property(4).toNumber(),
                                         params.property(5).toNumber(),
                                         params.property(6).toNumber(),
                                         params.property(7).toNumber(),
                                         params.property(8).toNumber(),
                                         params.property(9).toNumber(),
                                         params.property(10).toNumber(),
                                         params.property(11).toNumber(),
                                         params.property(12).toNumber(),
                                         params.property(13).toNumber(),
                                         params.property(14).toNumber(),
                                         params.property(15).toNumber()));
                return true;
            } else if (params.isString()) {
                bool ok = false;
                auto vector = matrix4x4FromString(params.toString(), &ok);
                if (ok) {
                    *v = QVariant(vector);
                    return true;
                }
            }
            break;
        default: break;
        }

        return false;
    }

#undef ASSERT_VALID_SIZE
};

Quick3DValueTypeProvider *valueTypeProvider = nullptr;
static Quick3DValueTypeProvider *getValueTypeProvider()
{
    if (valueTypeProvider == nullptr)
        valueTypeProvider = new Quick3DValueTypeProvider();
    return valueTypeProvider;
}

static Quick3DColorProvider *getColorProvider()
{
    static Quick3DColorProvider colorProvider;
    return &colorProvider;
}

static QQmlPrivate::AutoParentResult qquick3ditem_autoParent(QObject *obj, QObject *parent)
{
    // When setting a parent (especially during dynamic object creation) in QML,
    // also try to set up the analogous item/window relationship.
    auto parentNode = qmlobject_cast<Qt3DCore::QNode *>(parent);
    if (parentNode) {
        auto node = qmlobject_cast<Qt3DCore::QNode *>(obj);
        if (node) {
            // A QNode has another QNode child
            node->setParent(parentNode);
            return QQmlPrivate::Parented;
        }
    } else {
        return QQmlPrivate::IncompatibleParent;
    }
    return QQmlPrivate::IncompatibleObject;
}

void Quick3D_initialize()
{
    QQml_addValueTypeProvider(getValueTypeProvider());
    QQml_setColorProvider(getColorProvider());
    QAbstractNodeFactory::registerNodeFactory(QuickNodeFactory::instance());

    // Register a hook called when we do component.create() that sets the
    // parent. We need this as QObject::setParent() is insufficient to propagate
    // the arbiter and scene to the children (see QNode::setParent(QNode *).
    // TODO: Replace this with virtual void QObjectPrivate::setParent(QObject *)
    // that can be called from QObject ctor and QObject::setParent(). That would
    // allow removal of this hook here and in QtQuick.
    QQmlPrivate::RegisterAutoParent autoparent = { 0, &qquick3ditem_autoParent };
    QQmlPrivate::qmlregister(QQmlPrivate::AutoParentRegistration, &autoparent);
}

void Quick3D_uninitialize()
{
    delete valueTypeProvider;
    valueTypeProvider = nullptr;
}

void Quick3D_registerType(const char *className, const char *quickName, int major, int minor)
{
    QuickNodeFactory::instance()->registerType(className, quickName, major, minor);
}

} // namespace Quick
} // namespace Qt3DCore

QT_END_NAMESPACE
