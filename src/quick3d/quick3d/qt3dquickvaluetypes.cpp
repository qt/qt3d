// Copyright (C) 2016 The Qt Company Ltd and/or its subsidiary(-ies).
// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qt3dquickvaluetypes_p.h"

QT_BEGIN_NAMESPACE

namespace {

template<typename T, int NumParams>
QVariant createValueTypeFromNumberString(const QString &s)
{
    Q_STATIC_ASSERT_X(NumParams == 2 || NumParams == 3 || NumParams == 4 || NumParams == 16,
                      "Unsupported number of params; add an additional case below if necessary.");

    if (s.count(u',') != NumParams - 1)
        return QVariant();

    QVarLengthArray<float, NumParams> parameters;
    bool ok = true;
    for (qsizetype prev = 0, next = s.indexOf(u','), length = s.size(); ok && prev < length;) {
        parameters.append(s.mid(prev, next - prev).toFloat(&ok));
        prev = next + 1;
        next = (parameters.size() == NumParams - 1) ? length : s.indexOf(u',', prev);
    }

    if (!ok)
        return QVariant();

    if constexpr (NumParams == 2) {
        return T(parameters[0], parameters[1]);
    } else if constexpr (NumParams == 3) {
        return T(parameters[0], parameters[1], parameters[2]);
    } else if constexpr (NumParams == 4) {
        return T(parameters[0], parameters[1], parameters[2], parameters[3]);
    } else if constexpr (NumParams == 16) {
        return T(parameters[0], parameters[1], parameters[2], parameters[3],
                 parameters[4], parameters[5], parameters[6], parameters[7],
                 parameters[8], parameters[9], parameters[10], parameters[11],
                 parameters[12], parameters[13], parameters[14], parameters[15]);
    } else {
        Q_UNREACHABLE();
    }

    return QVariant();
}

}

namespace Qt3DCore {
namespace Quick {

QVariant QQuick3DColorValueType::create(const QJSValue &params)
{
    return params.isString() ? QColor::fromString(params.toString()) : QVariant();
}

QString QQuick3DColorValueType::toString() const
{
    return v.name(v.alpha() != 255 ? QColor::HexArgb : QColor::HexRgb);
}

QVariant QQuick3DColorValueType::lighter(qreal factor) const
{
    return QQml_colorProvider()->lighter(this->v, factor);
}

QVariant QQuick3DColorValueType::darker(qreal factor) const
{
    return QQml_colorProvider()->darker(this->v, factor);
}

QVariant QQuick3DColorValueType::alpha(qreal value) const
{
    return QQml_colorProvider()->alpha(this->v, value);
}

QVariant QQuick3DColorValueType::tint(QVariant tintColor) const
{
    return QQml_colorProvider()->tint(this->v, tintColor);
}

qreal QQuick3DColorValueType::r() const
{
    return v.redF();
}

qreal QQuick3DColorValueType::g() const
{
    return v.greenF();
}

qreal QQuick3DColorValueType::b() const
{
    return v.blueF();
}

qreal QQuick3DColorValueType::a() const
{
    return v.alphaF();
}

qreal QQuick3DColorValueType::hsvHue() const
{
    return v.hsvHueF();
}

qreal QQuick3DColorValueType::hsvSaturation() const
{
    return v.hsvSaturationF();
}

qreal QQuick3DColorValueType::hsvValue() const
{
    return v.valueF();
}

qreal QQuick3DColorValueType::hslHue() const
{
    return v.hslHueF();
}

qreal QQuick3DColorValueType::hslSaturation() const
{
    return v.hslSaturationF();
}

qreal QQuick3DColorValueType::hslLightness() const
{
    return v.lightnessF();
}

bool QQuick3DColorValueType::isValid() const
{
    return v.isValid();
}

void QQuick3DColorValueType::setR(qreal r)
{
    v.setRedF(r);
}

void QQuick3DColorValueType::setG(qreal g)
{
    v.setGreenF(g);
}

void QQuick3DColorValueType::setB(qreal b)
{
    v.setBlueF(b);
}

void QQuick3DColorValueType::setA(qreal a)
{
    v.setAlphaF(a);
}

void QQuick3DColorValueType::setHsvHue(qreal hsvHue)
{
    float hue, saturation, value, alpha;
    v.getHsvF(&hue, &saturation, &value, &alpha);
    v.setHsvF(hsvHue, saturation, value, alpha);
}

void QQuick3DColorValueType::setHsvSaturation(qreal hsvSaturation)
{
    float hue, saturation, value, alpha;
    v.getHsvF(&hue, &saturation, &value, &alpha);
    v.setHsvF(hue, hsvSaturation, value, alpha);
}

void QQuick3DColorValueType::setHsvValue(qreal hsvValue)
{
    float hue, saturation, value, alpha;
    v.getHsvF(&hue, &saturation, &value, &alpha);
    v.setHsvF(hue, saturation, hsvValue, alpha);
}

void QQuick3DColorValueType::setHslHue(qreal hslHue)
{
    float hue, saturation, lightness, alpha;
    v.getHslF(&hue, &saturation, &lightness, &alpha);
    v.setHslF(hslHue, saturation, lightness, alpha);
}

void QQuick3DColorValueType::setHslSaturation(qreal hslSaturation)
{
    float hue, saturation, lightness, alpha;
    v.getHslF(&hue, &saturation, &lightness, &alpha);
    v.setHslF(hue, hslSaturation, lightness, alpha);
}

void QQuick3DColorValueType::setHslLightness(qreal hslLightness)
{
    float hue, saturation, lightness, alpha;
    v.getHslF(&hue, &saturation, &lightness, &alpha);
    v.setHslF(hue, saturation, hslLightness, alpha);
}

QVariant QQuick3DMatrix4x4ValueType::create(const QJSValue &params)
{
    if (params.isNull() || params.isUndefined())
        return QMatrix4x4();

    if (params.isString())
        return createValueTypeFromNumberString<QMatrix4x4, 16>(params.toString());

    if (params.isArray() && params.property(QStringLiteral("length")).toInt() == 16) {
        return QMatrix4x4(params.property(0).toNumber(),
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
                          params.property(15).toNumber());
    }

    return {};
}

QMatrix4x4 QQuick3DMatrix4x4ValueType::times(const QMatrix4x4 &m) const
{
    return v * m;
}

QVector4D QQuick3DMatrix4x4ValueType::times(const QVector4D &vec) const
{
    return v * vec;
}

QVector3D QQuick3DMatrix4x4ValueType::times(const QVector3D &vec) const
{
    return v.map(vec);
}

QMatrix4x4 QQuick3DMatrix4x4ValueType::times(qreal factor) const
{
    return v * float(factor);
}

QMatrix4x4 QQuick3DMatrix4x4ValueType::plus(const QMatrix4x4 &m) const
{
    return v + m;
}

QMatrix4x4 QQuick3DMatrix4x4ValueType::minus(const QMatrix4x4 &m) const
{
    return v - m;
}

QVector4D QQuick3DMatrix4x4ValueType::row(int n) const
{
    return v.row(n);
}

QVector4D QQuick3DMatrix4x4ValueType::column(int m) const
{
    return v.column(m);
}

qreal QQuick3DMatrix4x4ValueType::determinant() const
{
    return v.determinant();
}

QMatrix4x4 QQuick3DMatrix4x4ValueType::inverted() const
{
    return v.inverted();
}

QMatrix4x4 QQuick3DMatrix4x4ValueType::transposed() const
{
    return v.transposed();
}

bool QQuick3DMatrix4x4ValueType::fuzzyEquals(const QMatrix4x4 &m, qreal epsilon) const
{
    qreal absEps = qAbs(epsilon);
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            if (qAbs(v(i,j) - m(i,j)) > absEps) {
                return false;
            }
        }
    }
    return true;
}

bool QQuick3DMatrix4x4ValueType::fuzzyEquals(const QMatrix4x4 &m) const
{
    return qFuzzyCompare(v, m);
}

QVariant QQuick3DVector3DValueType::create(const QJSValue &params)
{
    if (params.isString())
        return createValueTypeFromNumberString<QVector3D, 3>(params.toString());

    if (params.isArray()) {
        return QVector3D(params.property(0).toNumber(), params.property(1).toNumber(),
                         params.property(2).toNumber());
    }
    return QVariant();
}

QString QQuick3DVector3DValueType::toString() const
{
    return QString(QLatin1String("QVector3D(%1, %2, %3)")).arg(v.x()).arg(v.y()).arg(v.z());
}

qreal QQuick3DVector3DValueType::x() const
{
    return qreal(v.x());
}

qreal QQuick3DVector3DValueType::y() const
{
    return qreal(v.y());
}

qreal QQuick3DVector3DValueType::z() const
{
    return qreal(v.z());
}

void QQuick3DVector3DValueType::setX(qreal x)
{
    v.setX(float(x));
}

void QQuick3DVector3DValueType::setY(qreal y)
{
    v.setY(float(y));
}

void QQuick3DVector3DValueType::setZ(qreal z)
{
    v.setZ(float(z));
}

QVector3D QQuick3DVector3DValueType::crossProduct(const QVector3D &vec) const
{
    return QVector3D::crossProduct(v, vec);
}

qreal QQuick3DVector3DValueType::dotProduct(const QVector3D &vec) const
{
    return qreal(QVector3D::dotProduct(v, vec));
}

QVector3D QQuick3DVector3DValueType::times(const QMatrix4x4 &m) const
{
    return (QVector4D(v, 1.f) * m).toVector3DAffine();
}

QVector3D QQuick3DVector3DValueType::times(const QVector3D &vec) const
{
    return v * vec;
}

QVector3D QQuick3DVector3DValueType::times(qreal scalar) const
{
    return v * float(scalar);
}

QVector3D QQuick3DVector3DValueType::plus(const QVector3D &vec) const
{
    return v + vec;
}

QVector3D QQuick3DVector3DValueType::minus(const QVector3D &vec) const
{
    return v - vec;
}

QVector3D QQuick3DVector3DValueType::normalized() const
{
    return v.normalized();
}

qreal QQuick3DVector3DValueType::length() const
{
    return qreal(v.length());
}

QVector2D QQuick3DVector3DValueType::toVector2d() const
{
    return v.toVector2D();
}

QVector4D QQuick3DVector3DValueType::toVector4d() const
{
    return v.toVector4D();
}

bool QQuick3DVector3DValueType::fuzzyEquals(const QVector3D &vec, qreal epsilon) const
{
    qreal absEps = qAbs(epsilon);
    if (qAbs(v.x() - vec.x()) > absEps)
        return false;
    if (qAbs(v.y() - vec.y()) > absEps)
        return false;
    if (qAbs(v.z() - vec.z()) > absEps)
        return false;
    return true;
}

bool QQuick3DVector3DValueType::fuzzyEquals(const QVector3D &vec) const
{
    return qFuzzyCompare(v, vec);
}

QVariant QQuick3DVector2DValueType::create(const QJSValue &params)
{
    if (params.isString())
        return createValueTypeFromNumberString<QVector2D, 2>(params.toString());
    if (params.isArray())
        return QVector2D(params.property(0).toNumber(), params.property(1).toNumber());
    return QVariant();
}

QString QQuick3DVector2DValueType::toString() const
{
    return QString(QLatin1String("QVector2D(%1, %2)")).arg(v.x()).arg(v.y());
}

qreal QQuick3DVector2DValueType::x() const
{
    return v.x();
}

qreal QQuick3DVector2DValueType::y() const
{
    return v.y();
}

void QQuick3DVector2DValueType::setX(qreal x)
{
    v.setX(x);
}

void QQuick3DVector2DValueType::setY(qreal y)
{
    v.setY(y);
}

qreal QQuick3DVector2DValueType::dotProduct(const QVector2D &vec) const
{
    return QVector2D::dotProduct(v, vec);
}

QVector2D QQuick3DVector2DValueType::times(const QVector2D &vec) const
{
    return v * vec;
}

QVector2D QQuick3DVector2DValueType::times(qreal scalar) const
{
    return v * scalar;
}

QVector2D QQuick3DVector2DValueType::plus(const QVector2D &vec) const
{
    return v + vec;
}

QVector2D QQuick3DVector2DValueType::minus(const QVector2D &vec) const
{
    return v - vec;
}

QVector2D QQuick3DVector2DValueType::normalized() const
{
    return v.normalized();
}

qreal QQuick3DVector2DValueType::length() const
{
    return v.length();
}

QVector3D QQuick3DVector2DValueType::toVector3d() const
{
    return v.toVector3D();
}

QVector4D QQuick3DVector2DValueType::toVector4d() const
{
    return v.toVector4D();
}

bool QQuick3DVector2DValueType::fuzzyEquals(const QVector2D &vec, qreal epsilon) const
{
    qreal absEps = qAbs(epsilon);
    if (qAbs(v.x() - vec.x()) > absEps)
        return false;
    if (qAbs(v.y() - vec.y()) > absEps)
        return false;
    return true;
}

bool QQuick3DVector2DValueType::fuzzyEquals(const QVector2D &vec) const
{
    return qFuzzyCompare(v, vec);
}

QVariant QQuick3DVector4DValueType::create(const QJSValue &params)
{
    if (params.isString())
        return createValueTypeFromNumberString<QVector4D, 4>(params.toString());

    if (params.isArray()) {
        return QVector4D(params.property(0).toNumber(), params.property(1).toNumber(),
                         params.property(2).toNumber(), params.property(3).toNumber());
    }

    return QVariant();
}

QString QQuick3DVector4DValueType::toString() const
{
    return QString(QLatin1String("QVector4D(%1, %2, %3, %4)")).arg(v.x()).arg(v.y()).arg(v.z()).arg(v.w());
}

qreal QQuick3DVector4DValueType::x() const
{
    return v.x();
}

qreal QQuick3DVector4DValueType::y() const
{
    return v.y();
}

qreal QQuick3DVector4DValueType::z() const
{
    return v.z();
}

qreal QQuick3DVector4DValueType::w() const
{
    return v.w();
}

void QQuick3DVector4DValueType::setX(qreal x)
{
    v.setX(x);
}

void QQuick3DVector4DValueType::setY(qreal y)
{
    v.setY(y);
}

void QQuick3DVector4DValueType::setZ(qreal z)
{
    v.setZ(z);
}

void QQuick3DVector4DValueType::setW(qreal w)
{
    v.setW(w);
}

qreal QQuick3DVector4DValueType::dotProduct(const QVector4D &vec) const
{
    return QVector4D::dotProduct(v, vec);
}

QVector4D QQuick3DVector4DValueType::times(const QVector4D &vec) const
{
    return v * vec;
}

QVector4D QQuick3DVector4DValueType::times(const QMatrix4x4 &m) const
{
    return v * m;
}

QVector4D QQuick3DVector4DValueType::times(qreal scalar) const
{
    return v * scalar;
}

QVector4D QQuick3DVector4DValueType::plus(const QVector4D &vec) const
{
    return v + vec;
}

QVector4D QQuick3DVector4DValueType::minus(const QVector4D &vec) const
{
    return v - vec;
}

QVector4D QQuick3DVector4DValueType::normalized() const
{
    return v.normalized();
}

qreal QQuick3DVector4DValueType::length() const
{
    return v.length();
}

QVector2D QQuick3DVector4DValueType::toVector2d() const
{
    return v.toVector2D();
}

QVector3D QQuick3DVector4DValueType::toVector3d() const
{
    return v.toVector3D();
}

bool QQuick3DVector4DValueType::fuzzyEquals(const QVector4D &vec, qreal epsilon) const
{
    qreal absEps = qAbs(epsilon);
    if (qAbs(v.x() - vec.x()) > absEps)
        return false;
    if (qAbs(v.y() - vec.y()) > absEps)
        return false;
    if (qAbs(v.z() - vec.z()) > absEps)
        return false;
    if (qAbs(v.w() - vec.w()) > absEps)
        return false;
    return true;
}

bool QQuick3DVector4DValueType::fuzzyEquals(const QVector4D &vec) const
{
    return qFuzzyCompare(v, vec);
}

QVariant QQuick3DQuaternionValueType::create(const QJSValue &params)
{
    if (params.isString())
        return createValueTypeFromNumberString<QQuaternion, 4>(params.toString());

    if (params.isArray()) {
        return QQuaternion(params.property(0).toNumber(), params.property(1).toNumber(),
                           params.property(2).toNumber(), params.property(3).toNumber());
    }

    return QVariant();
}

QString QQuick3DQuaternionValueType::toString() const
{
    return QString(QLatin1String("QQuaternion(%1, %2, %3, %4)")).arg(v.scalar()).arg(v.x()).arg(v.y()).arg(v.z());
}

qreal QQuick3DQuaternionValueType::scalar() const
{
    return v.scalar();
}

qreal QQuick3DQuaternionValueType::x() const
{
    return v.x();
}

qreal QQuick3DQuaternionValueType::y() const
{
    return v.y();
}

qreal QQuick3DQuaternionValueType::z() const
{
    return v.z();
}

void QQuick3DQuaternionValueType::setScalar(qreal scalar)
{
    v.setScalar(scalar);
}

void QQuick3DQuaternionValueType::setX(qreal x)
{
    v.setX(x);
}

void QQuick3DQuaternionValueType::setY(qreal y)
{
    v.setY(y);
}

void QQuick3DQuaternionValueType::setZ(qreal z)
{
    v.setZ(z);
}

} // namespace Quick
} // namespace Qt3DCore

QT_END_NAMESPACE

#include "moc_qt3dquickvaluetypes_p.cpp"
