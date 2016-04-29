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

#include "qtexturedata_p.h"
#include <QFileInfo>
#include <QFile>
#include <qendian.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace {

struct DdsPixelFormat {
    quint32 size;
    quint32 flags;
    quint32 fourCC;
    quint32 rgbBitCount;
    quint32 redMask;
    quint32 greenMask;
    quint32 blueMask;
    quint32 alphaMask;
};

struct DdsHeader {
    char magic[4];
    quint32 size;
    quint32 flags;
    quint32 height;
    quint32 width;
    quint32 pitchOrLinearSize;
    quint32 depth;
    quint32 mipmapCount;
    quint32 reserved[11];
    DdsPixelFormat pixelFormat;
    quint32 caps;
    quint32 caps2;
    quint32 caps3;
    quint32 caps4;
    quint32 reserved2;
};

struct DdsDX10Header {
    quint32 format;
    quint32 dimension;
    quint32 miscFlag;
    quint32 arraySize;
    quint32 miscFlags2;
};

enum DdsFlags {
    MipmapCountFlag             = 0x20000,
};

enum PixelFormatFlag {
    AlphaFlag                   = 0x1,
    FourCCFlag                  = 0x4,
    RGBFlag                     = 0x40,
    RGBAFlag                    = RGBFlag | AlphaFlag,
    YUVFlag                     = 0x200,
    LuminanceFlag               = 0x20000
};

enum Caps2Flags {
    CubemapFlag                 = 0x200,
    CubemapPositiveXFlag        = 0x400,
    CubemapNegativeXFlag        = 0x800,
    CubemapPositiveYFlag        = 0x1000,
    CubemapNegativeYFlag        = 0x2000,
    CubemapPositiveZFlag        = 0x4000,
    CubemapNegativeZFlag        = 0x8000,
    AllCubemapFaceFlags         = CubemapPositiveXFlag |
                                  CubemapNegativeXFlag |
                                  CubemapPositiveYFlag |
                                  CubemapNegativeYFlag |
                                  CubemapPositiveZFlag |
                                  CubemapNegativeZFlag,
    VolumeFlag                  = 0x200000
};

enum DXGIFormat {
    DXGI_FORMAT_UNKNOWN                     = 0,
    DXGI_FORMAT_R32G32B32A32_TYPELESS       = 1,
    DXGI_FORMAT_R32G32B32A32_FLOAT          = 2,
    DXGI_FORMAT_R32G32B32A32_UINT           = 3,
    DXGI_FORMAT_R32G32B32A32_SINT           = 4,
    DXGI_FORMAT_R32G32B32_TYPELESS          = 5,
    DXGI_FORMAT_R32G32B32_FLOAT             = 6,
    DXGI_FORMAT_R32G32B32_UINT              = 7,
    DXGI_FORMAT_R32G32B32_SINT              = 8,
    DXGI_FORMAT_R16G16B16A16_TYPELESS       = 9,
    DXGI_FORMAT_R16G16B16A16_FLOAT          = 10,
    DXGI_FORMAT_R16G16B16A16_UNORM          = 11,
    DXGI_FORMAT_R16G16B16A16_UINT           = 12,
    DXGI_FORMAT_R16G16B16A16_SNORM          = 13,
    DXGI_FORMAT_R16G16B16A16_SINT           = 14,
    DXGI_FORMAT_R32G32_TYPELESS             = 15,
    DXGI_FORMAT_R32G32_FLOAT                = 16,
    DXGI_FORMAT_R32G32_UINT                 = 17,
    DXGI_FORMAT_R32G32_SINT                 = 18,
    DXGI_FORMAT_R32G8X24_TYPELESS           = 19,
    DXGI_FORMAT_D32_FLOAT_S8X24_UINT        = 20,
    DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS    = 21,
    DXGI_FORMAT_X32_TYPELESS_G8X24_UINT     = 22,
    DXGI_FORMAT_R10G10B10A2_TYPELESS        = 23,
    DXGI_FORMAT_R10G10B10A2_UNORM           = 24,
    DXGI_FORMAT_R10G10B10A2_UINT            = 25,
    DXGI_FORMAT_R11G11B10_FLOAT             = 26,
    DXGI_FORMAT_R8G8B8A8_TYPELESS           = 27,
    DXGI_FORMAT_R8G8B8A8_UNORM              = 28,
    DXGI_FORMAT_R8G8B8A8_UNORM_SRGB         = 29,
    DXGI_FORMAT_R8G8B8A8_UINT               = 30,
    DXGI_FORMAT_R8G8B8A8_SNORM              = 31,
    DXGI_FORMAT_R8G8B8A8_SINT               = 32,
    DXGI_FORMAT_R16G16_TYPELESS             = 33,
    DXGI_FORMAT_R16G16_FLOAT                = 34,
    DXGI_FORMAT_R16G16_UNORM                = 35,
    DXGI_FORMAT_R16G16_UINT                 = 36,
    DXGI_FORMAT_R16G16_SNORM                = 37,
    DXGI_FORMAT_R16G16_SINT                 = 38,
    DXGI_FORMAT_R32_TYPELESS                = 39,
    DXGI_FORMAT_D32_FLOAT                   = 40,
    DXGI_FORMAT_R32_FLOAT                   = 41,
    DXGI_FORMAT_R32_UINT                    = 42,
    DXGI_FORMAT_R32_SINT                    = 43,
    DXGI_FORMAT_R24G8_TYPELESS              = 44,
    DXGI_FORMAT_D24_UNORM_S8_UINT           = 45,
    DXGI_FORMAT_R24_UNORM_X8_TYPELESS       = 46,
    DXGI_FORMAT_X24_TYPELESS_G8_UINT        = 47,
    DXGI_FORMAT_R8G8_TYPELESS               = 48,
    DXGI_FORMAT_R8G8_UNORM                  = 49,
    DXGI_FORMAT_R8G8_UINT                   = 50,
    DXGI_FORMAT_R8G8_SNORM                  = 51,
    DXGI_FORMAT_R8G8_SINT                   = 52,
    DXGI_FORMAT_R16_TYPELESS                = 53,
    DXGI_FORMAT_R16_FLOAT                   = 54,
    DXGI_FORMAT_D16_UNORM                   = 55,
    DXGI_FORMAT_R16_UNORM                   = 56,
    DXGI_FORMAT_R16_UINT                    = 57,
    DXGI_FORMAT_R16_SNORM                   = 58,
    DXGI_FORMAT_R16_SINT                    = 59,
    DXGI_FORMAT_R8_TYPELESS                 = 60,
    DXGI_FORMAT_R8_UNORM                    = 61,
    DXGI_FORMAT_R8_UINT                     = 62,
    DXGI_FORMAT_R8_SNORM                    = 63,
    DXGI_FORMAT_R8_SINT                     = 64,
    DXGI_FORMAT_A8_UNORM                    = 65,
    DXGI_FORMAT_R1_UNORM                    = 66,
    DXGI_FORMAT_R9G9B9E5_SHAREDEXP          = 67,
    DXGI_FORMAT_R8G8_B8G8_UNORM             = 68,
    DXGI_FORMAT_G8R8_G8B8_UNORM             = 69,
    DXGI_FORMAT_BC1_TYPELESS                = 70,
    DXGI_FORMAT_BC1_UNORM                   = 71,
    DXGI_FORMAT_BC1_UNORM_SRGB              = 72,
    DXGI_FORMAT_BC2_TYPELESS                = 73,
    DXGI_FORMAT_BC2_UNORM                   = 74,
    DXGI_FORMAT_BC2_UNORM_SRGB              = 75,
    DXGI_FORMAT_BC3_TYPELESS                = 76,
    DXGI_FORMAT_BC3_UNORM                   = 77,
    DXGI_FORMAT_BC3_UNORM_SRGB              = 78,
    DXGI_FORMAT_BC4_TYPELESS                = 79,
    DXGI_FORMAT_BC4_UNORM                   = 80,
    DXGI_FORMAT_BC4_SNORM                   = 81,
    DXGI_FORMAT_BC5_TYPELESS                = 82,
    DXGI_FORMAT_BC5_UNORM                   = 83,
    DXGI_FORMAT_BC5_SNORM                   = 84,
    DXGI_FORMAT_B5G6R5_UNORM                = 85,
    DXGI_FORMAT_B5G5R5A1_UNORM              = 86,
    DXGI_FORMAT_B8G8R8A8_UNORM              = 87,
    DXGI_FORMAT_B8G8R8X8_UNORM              = 88,
    DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM  = 89,
    DXGI_FORMAT_B8G8R8A8_TYPELESS           = 90,
    DXGI_FORMAT_B8G8R8A8_UNORM_SRGB         = 91,
    DXGI_FORMAT_B8G8R8X8_TYPELESS           = 92,
    DXGI_FORMAT_B8G8R8X8_UNORM_SRGB         = 93,
    DXGI_FORMAT_BC6H_TYPELESS               = 94,
    DXGI_FORMAT_BC6H_UF16                   = 95,
    DXGI_FORMAT_BC6H_SF16                   = 96,
    DXGI_FORMAT_BC7_TYPELESS                = 97,
    DXGI_FORMAT_BC7_UNORM                   = 98,
    DXGI_FORMAT_BC7_UNORM_SRGB              = 99,
    DXGI_FORMAT_AYUV                        = 100,
    DXGI_FORMAT_Y410                        = 101,
    DXGI_FORMAT_Y416                        = 102,
    DXGI_FORMAT_NV12                        = 103,
    DXGI_FORMAT_P010                        = 104,
    DXGI_FORMAT_P016                        = 105,
    DXGI_FORMAT_420_OPAQUE                  = 106,
    DXGI_FORMAT_YUY2                        = 107,
    DXGI_FORMAT_Y210                        = 108,
    DXGI_FORMAT_Y216                        = 109,
    DXGI_FORMAT_NV11                        = 110,
    DXGI_FORMAT_AI44                        = 111,
    DXGI_FORMAT_IA44                        = 112,
    DXGI_FORMAT_P8                          = 113,
    DXGI_FORMAT_A8P8                        = 114,
    DXGI_FORMAT_B4G4R4A4_UNORM              = 115,
    DXGI_FORMAT_P208                        = 130,
    DXGI_FORMAT_V208                        = 131,
    DXGI_FORMAT_V408                        = 132,
};

template <int a, int b, int c, int d>
struct DdsFourCC
{
    static const quint32 value = a | (b << 8) | (c << 16) | (d << 24);
};

struct FormatInfo {
    QOpenGLTexture::PixelFormat pixelFormat;
    QOpenGLTexture::TextureFormat textureFormat;
    QOpenGLTexture::PixelType pixelType;
    int components;
    bool compressed;
};

const struct RGBAFormat {
    quint32 redMask;
    quint32 greenMask;
    quint32 blueMask;
    quint32 alphaMask;
    FormatInfo formatInfo;
} rgbaFormats[] = {
    // unorm formats
    { 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000,   { QOpenGLTexture::RGBA,             QOpenGLTexture::RGBA8_UNorm,    QOpenGLTexture::UInt8,           4, false } },
    { 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000,   { QOpenGLTexture::BGRA,             QOpenGLTexture::RGBA8_UNorm,    QOpenGLTexture::UInt8,           4, false } },
    { 0x000000ff, 0x0000ff00, 0x00ff0000, 0x00000000,   { QOpenGLTexture::RGBA,             QOpenGLTexture::RGBA8_UNorm,    QOpenGLTexture::UInt8,           4, false } },
    { 0x00ff0000, 0x0000ff00, 0x000000ff, 0x00000000,   { QOpenGLTexture::BGRA,             QOpenGLTexture::RGBA8_UNorm,    QOpenGLTexture::UInt8,           4, false } },
    { 0x000000ff, 0x0000ff00, 0x00ff0000, 0x00000000,   { QOpenGLTexture::RGB,              QOpenGLTexture::RGB8_UNorm,     QOpenGLTexture::UInt8,           3, false } },
    { 0x00ff0000, 0x0000ff00, 0x000000ff, 0x00000000,   { QOpenGLTexture::BGR,              QOpenGLTexture::RGB8_UNorm,     QOpenGLTexture::UInt8,           3, false } },

    // packed formats
    { 0x0000f800, 0x000007e0, 0x0000001f, 0x00000000,   { QOpenGLTexture::RGB,              QOpenGLTexture::R5G6B5,         QOpenGLTexture::UInt16_R5G6B5,   2, false } },
    { 0x00007c00, 0x000003e0, 0x0000001f, 0x00008000,   { QOpenGLTexture::RGBA,             QOpenGLTexture::RGB5A1,         QOpenGLTexture::UInt16_RGB5A1,   2, false } },
    { 0x00000f00, 0x000000f0, 0x0000000f, 0x0000f000,   { QOpenGLTexture::RGBA,             QOpenGLTexture::RGBA4,          QOpenGLTexture::UInt16_RGBA4,    2, false } },
    { 0x000000e0, 0x0000001c, 0x00000003, 0x00000000,   { QOpenGLTexture::RGB,              QOpenGLTexture::RG3B2,          QOpenGLTexture::UInt8_RG3B2,     1, false } },
    { 0x3ff00000, 0x000ffc00, 0x000003ff, 0xc0000000,   { QOpenGLTexture::RGBA,             QOpenGLTexture::RGB10A2,        QOpenGLTexture::UInt32_RGB10A2,  4, false } },

    // luminance alpha
    { 0x000000ff, 0x000000ff, 0x000000ff, 0x00000000,   { QOpenGLTexture::Red,              QOpenGLTexture::R8_UNorm,       QOpenGLTexture::UInt8,           1, false } },
    { 0x000000ff, 0x00000000, 0x00000000, 0x00000000,   { QOpenGLTexture::Red,              QOpenGLTexture::R8_UNorm,       QOpenGLTexture::UInt8,           1, false } },
    { 0x000000ff, 0x000000ff, 0x000000ff, 0x0000ff00,   { QOpenGLTexture::RG,               QOpenGLTexture::RG8_UNorm,      QOpenGLTexture::UInt8,           2, false } },
    { 0x000000ff, 0x00000000, 0x00000000, 0x0000ff00,   { QOpenGLTexture::RG,               QOpenGLTexture::RG8_UNorm,      QOpenGLTexture::UInt8,           2, false } },
};

const struct FourCCFormat {
    quint32 fourCC;
    FormatInfo formatInfo;
} fourCCFormats[] = {
    { DdsFourCC<'D','X','T','1'>::value,                { QOpenGLTexture::NoSourceFormat,   QOpenGLTexture::RGBA_DXT1,      QOpenGLTexture::NoPixelType,     8, true } },
    { DdsFourCC<'D','X','T','3'>::value,                { QOpenGLTexture::NoSourceFormat,   QOpenGLTexture::RGBA_DXT3,      QOpenGLTexture::NoPixelType,    16, true } },
    { DdsFourCC<'D','X','T','5'>::value,                { QOpenGLTexture::NoSourceFormat,   QOpenGLTexture::RGBA_DXT5,      QOpenGLTexture::NoPixelType,    16, true } },
    { DdsFourCC<'A','T','I','1'>::value,                { QOpenGLTexture::NoSourceFormat,   QOpenGLTexture::R_ATI1N_UNorm,  QOpenGLTexture::NoPixelType,     8, true } },
    { DdsFourCC<'A','T','I','2'>::value,                { QOpenGLTexture::NoSourceFormat,   QOpenGLTexture::RG_ATI2N_UNorm, QOpenGLTexture::NoPixelType,    16, true } },
};

const struct DX10Format {
    DXGIFormat dxgiFormat;
    FormatInfo formatInfo;
} dx10Formats[] = {
    // unorm formats
    { DXGI_FORMAT_R8_UNORM,                             { QOpenGLTexture::Red,              QOpenGLTexture::R8_UNorm,       QOpenGLTexture::UInt8,           1, false } },
    { DXGI_FORMAT_R8G8_UNORM,                           { QOpenGLTexture::RG,               QOpenGLTexture::RG8_UNorm,      QOpenGLTexture::UInt8,           2, false } },
    { DXGI_FORMAT_R8G8B8A8_UNORM,                       { QOpenGLTexture::RGBA,             QOpenGLTexture::RGBA8_UNorm,    QOpenGLTexture::UInt8,           4, false } },

    { DXGI_FORMAT_R16_UNORM,                            { QOpenGLTexture::Red,              QOpenGLTexture::R16_UNorm,      QOpenGLTexture::UInt16,          2, false } },
    { DXGI_FORMAT_R16G16_UNORM,                         { QOpenGLTexture::RG,               QOpenGLTexture::RG16_UNorm,     QOpenGLTexture::UInt16,          4, false } },
    { DXGI_FORMAT_R16G16B16A16_UNORM,                   { QOpenGLTexture::RGBA,             QOpenGLTexture::RGBA16_UNorm,   QOpenGLTexture::UInt16,          8, false } },

    // snorm formats
    { DXGI_FORMAT_R8_SNORM,                             { QOpenGLTexture::Red,              QOpenGLTexture::R8_SNorm,       QOpenGLTexture::Int8,            1, false } },
    { DXGI_FORMAT_R8G8_SNORM,                           { QOpenGLTexture::RG,               QOpenGLTexture::RG8_SNorm,      QOpenGLTexture::Int8,            2, false } },
    { DXGI_FORMAT_R8G8B8A8_SNORM,                       { QOpenGLTexture::RGBA,             QOpenGLTexture::RGBA8_SNorm,    QOpenGLTexture::Int8,            4, false } },

    { DXGI_FORMAT_R16_SNORM,                            { QOpenGLTexture::Red,              QOpenGLTexture::R16_SNorm,      QOpenGLTexture::Int16,           2, false } },
    { DXGI_FORMAT_R16G16_SNORM,                         { QOpenGLTexture::RG,               QOpenGLTexture::RG16_SNorm,     QOpenGLTexture::Int16,           4, false } },
    { DXGI_FORMAT_R16G16B16A16_SNORM,                   { QOpenGLTexture::RGBA,             QOpenGLTexture::RGBA16_SNorm,   QOpenGLTexture::Int16,           8, false } },

    // unsigned integer formats
    { DXGI_FORMAT_R8_UINT,                              { QOpenGLTexture::Red_Integer,      QOpenGLTexture::R8U,            QOpenGLTexture::UInt8,           1, false } },
    { DXGI_FORMAT_R8G8_UINT,                            { QOpenGLTexture::RG_Integer,       QOpenGLTexture::RG8U,           QOpenGLTexture::UInt8,           2, false } },
    { DXGI_FORMAT_R8G8B8A8_UINT,                        { QOpenGLTexture::RGBA_Integer,     QOpenGLTexture::RGBA8U,         QOpenGLTexture::UInt8,           4, false } },

    { DXGI_FORMAT_R16_UINT,                             { QOpenGLTexture::Red_Integer,      QOpenGLTexture::R16U,           QOpenGLTexture::UInt16,          2, false } },
    { DXGI_FORMAT_R16G16_UINT,                          { QOpenGLTexture::RG_Integer,       QOpenGLTexture::RG16U,          QOpenGLTexture::UInt16,          4, false } },
    { DXGI_FORMAT_R16G16B16A16_UINT,                    { QOpenGLTexture::RGBA_Integer,     QOpenGLTexture::RGBA16U,        QOpenGLTexture::UInt16,          8, false } },

    { DXGI_FORMAT_R32_UINT,                             { QOpenGLTexture::Red_Integer,      QOpenGLTexture::R32U,           QOpenGLTexture::UInt32,          4, false } },
    { DXGI_FORMAT_R32G32_UINT,                          { QOpenGLTexture::RG_Integer,       QOpenGLTexture::RG32U,          QOpenGLTexture::UInt32,          8, false } },
    { DXGI_FORMAT_R32G32B32_UINT,                       { QOpenGLTexture::RGB_Integer,      QOpenGLTexture::RGB32U,         QOpenGLTexture::UInt32,         12, false } },
    { DXGI_FORMAT_R32G32B32A32_UINT,                    { QOpenGLTexture::RGBA_Integer,     QOpenGLTexture::RGBA32U,        QOpenGLTexture::UInt32,         16, false } },

    // signed integer formats
    { DXGI_FORMAT_R8_SINT,                              { QOpenGLTexture::Red_Integer,      QOpenGLTexture::R8I,            QOpenGLTexture::Int8,            1, false } },
    { DXGI_FORMAT_R8G8_SINT,                            { QOpenGLTexture::RG_Integer,       QOpenGLTexture::RG8I,           QOpenGLTexture::Int8,            2, false } },
    { DXGI_FORMAT_R8G8B8A8_SINT,                        { QOpenGLTexture::RGBA_Integer,     QOpenGLTexture::RGBA8I,         QOpenGLTexture::Int8,            4, false } },

    { DXGI_FORMAT_R16_SINT,                             { QOpenGLTexture::Red_Integer,      QOpenGLTexture::R16I,           QOpenGLTexture::Int16,           2, false } },
    { DXGI_FORMAT_R16G16_SINT,                          { QOpenGLTexture::RG_Integer,       QOpenGLTexture::RG16I,          QOpenGLTexture::Int16,           4, false } },
    { DXGI_FORMAT_R16G16B16A16_SINT,                    { QOpenGLTexture::RGBA_Integer,     QOpenGLTexture::RGBA16I,        QOpenGLTexture::Int16,           8, false } },

    { DXGI_FORMAT_R32_SINT,                             { QOpenGLTexture::Red_Integer,      QOpenGLTexture::R32I,           QOpenGLTexture::Int32,           4, false } },
    { DXGI_FORMAT_R32G32_SINT,                          { QOpenGLTexture::RG_Integer,       QOpenGLTexture::RG32I,          QOpenGLTexture::Int32,           8, false } },
    { DXGI_FORMAT_R32G32B32_SINT,                       { QOpenGLTexture::RGB_Integer,      QOpenGLTexture::RGB32I,         QOpenGLTexture::Int32,          12, false } },
    { DXGI_FORMAT_R32G32B32A32_SINT,                    { QOpenGLTexture::RGBA_Integer,     QOpenGLTexture::RGBA32I,        QOpenGLTexture::Int32,          16, false } },

    // floating formats
    { DXGI_FORMAT_R16_FLOAT,                            { QOpenGLTexture::Red,              QOpenGLTexture::R16F,           QOpenGLTexture::Float16,         2, false } },
    { DXGI_FORMAT_R16G16_FLOAT,                         { QOpenGLTexture::RG,               QOpenGLTexture::RG16F,          QOpenGLTexture::Float16,         4, false } },
    { DXGI_FORMAT_R16G16B16A16_FLOAT,                   { QOpenGLTexture::RGBA,             QOpenGLTexture::RGBA16F,        QOpenGLTexture::Float16,         8, false } },

    { DXGI_FORMAT_R32_FLOAT,                            { QOpenGLTexture::Red,              QOpenGLTexture::R32F,           QOpenGLTexture::Float32,         4, false } },
    { DXGI_FORMAT_R32G32_FLOAT,                         { QOpenGLTexture::RG,               QOpenGLTexture::RG32F,          QOpenGLTexture::Float32,         8, false } },
    { DXGI_FORMAT_R32G32B32_FLOAT,                      { QOpenGLTexture::RGB,              QOpenGLTexture::RGB32F,         QOpenGLTexture::Float32,        12, false } },
    { DXGI_FORMAT_R32G32B32A32_FLOAT,                   { QOpenGLTexture::RGBA,             QOpenGLTexture::RGBA32F,        QOpenGLTexture::Float32,        16, false } },

    // sRGB formats
    { DXGI_FORMAT_B8G8R8X8_UNORM_SRGB,                  { QOpenGLTexture::RGB,              QOpenGLTexture::SRGB8,          QOpenGLTexture::UInt8,           4, false } },
    { DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,                  { QOpenGLTexture::RGBA,             QOpenGLTexture::SRGB8_Alpha8,   QOpenGLTexture::UInt8,           4, false } },

    // packed formats
    // { DXGI_FORMAT_R10G10B10A2_UNORM,                 { QOpenGLTexture::RGB10A2_UNORM, QOpenGLTexture::RGBA, QOpenGLTexture::UInt32_RGB10A2, 4, false } },
    { DXGI_FORMAT_R10G10B10A2_UINT,                     { QOpenGLTexture::RGBA_Integer,     QOpenGLTexture::RGB10A2,        QOpenGLTexture::UInt32_RGB10A2,  4, false } },
    { DXGI_FORMAT_R9G9B9E5_SHAREDEXP,                   { QOpenGLTexture::RGB,              QOpenGLTexture::RGB9E5,         QOpenGLTexture::UInt32_RGB9_E5,  4, false } },
    { DXGI_FORMAT_R11G11B10_FLOAT,                      { QOpenGLTexture::RGB,              QOpenGLTexture::RG11B10F,       QOpenGLTexture::UInt32_RG11B10F, 4, false } },
    { DXGI_FORMAT_B5G6R5_UNORM,                         { QOpenGLTexture::RGB,              QOpenGLTexture::R5G6B5,         QOpenGLTexture::UInt16_R5G6B5,   2, false } },
    { DXGI_FORMAT_B5G5R5A1_UNORM,                       { QOpenGLTexture::RGBA,             QOpenGLTexture::RGB5A1,         QOpenGLTexture::UInt16_RGB5A1,   2, false } },
    { DXGI_FORMAT_B4G4R4A4_UNORM,                       { QOpenGLTexture::RGBA,             QOpenGLTexture::RGBA4,          QOpenGLTexture::UInt16_RGBA4,    2, false } },

    // swizzle formats
    { DXGI_FORMAT_B8G8R8X8_UNORM,                       { QOpenGLTexture::BGRA,             QOpenGLTexture::RGB8_UNorm,     QOpenGLTexture::UInt8,           4, false } },
    { DXGI_FORMAT_B8G8R8A8_UNORM,                       { QOpenGLTexture::BGRA,             QOpenGLTexture::RGBA8_UNorm,    QOpenGLTexture::UInt8,           4, false } },
    { DXGI_FORMAT_B8G8R8X8_UNORM_SRGB,                  { QOpenGLTexture::BGRA,             QOpenGLTexture::SRGB8,          QOpenGLTexture::UInt8,           4, false } },
    { DXGI_FORMAT_B8G8R8A8_UNORM_SRGB,                  { QOpenGLTexture::BGRA,             QOpenGLTexture::SRGB8_Alpha8,   QOpenGLTexture::UInt8,           4, false } },

    // luminance alpha formats
    { DXGI_FORMAT_R8_UNORM,                             { QOpenGLTexture::Red,              QOpenGLTexture::R8_UNorm,       QOpenGLTexture::UInt8,           1, false } },
    { DXGI_FORMAT_R8G8_UNORM,                           { QOpenGLTexture::RG,               QOpenGLTexture::RG8_UNorm,      QOpenGLTexture::UInt8,           2, false } },
    { DXGI_FORMAT_R16_UNORM,                            { QOpenGLTexture::Red,              QOpenGLTexture::R16_UNorm,      QOpenGLTexture::UInt16,          2, false } },
    { DXGI_FORMAT_R16_UNORM,                            { QOpenGLTexture::RG,               QOpenGLTexture::RG16_UNorm,     QOpenGLTexture::UInt16,          4, false } },

    // depth formats
    { DXGI_FORMAT_D16_UNORM,                            { QOpenGLTexture::Depth,            QOpenGLTexture::D16,            QOpenGLTexture::NoPixelType,     2, false } },
    { DXGI_FORMAT_D24_UNORM_S8_UINT,                    { QOpenGLTexture::DepthStencil,     QOpenGLTexture::D24S8,          QOpenGLTexture::NoPixelType,     4, false } },
    { DXGI_FORMAT_D32_FLOAT,                            { QOpenGLTexture::Depth,            QOpenGLTexture::D32F,           QOpenGLTexture::NoPixelType,     4, false } },
    { DXGI_FORMAT_D32_FLOAT_S8X24_UINT,                 { QOpenGLTexture::DepthStencil,     QOpenGLTexture::D32FS8X24,      QOpenGLTexture::NoPixelType,     8, false } },

    // compressed formats
    { DXGI_FORMAT_BC1_UNORM,                            { QOpenGLTexture::NoSourceFormat,   QOpenGLTexture::RGBA_DXT1,      QOpenGLTexture::NoPixelType,     8, true } },
    { DXGI_FORMAT_BC2_UNORM,                            { QOpenGLTexture::NoSourceFormat,   QOpenGLTexture::RGBA_DXT3,      QOpenGLTexture::NoPixelType,    16, true } },
    { DXGI_FORMAT_BC3_UNORM,                            { QOpenGLTexture::NoSourceFormat,   QOpenGLTexture::RGBA_DXT5,      QOpenGLTexture::NoPixelType,    16, true } },
    { DXGI_FORMAT_BC4_UNORM,                            { QOpenGLTexture::NoSourceFormat,   QOpenGLTexture::R_ATI1N_UNorm,  QOpenGLTexture::NoPixelType,     8, true } },
    { DXGI_FORMAT_BC4_SNORM,                            { QOpenGLTexture::NoSourceFormat,   QOpenGLTexture::R_ATI1N_SNorm,  QOpenGLTexture::NoPixelType,     8, true } },
    { DXGI_FORMAT_BC5_UNORM,                            { QOpenGLTexture::NoSourceFormat,   QOpenGLTexture::RG_ATI2N_UNorm, QOpenGLTexture::NoPixelType,    16, true } },
    { DXGI_FORMAT_BC5_SNORM,                            { QOpenGLTexture::NoSourceFormat,   QOpenGLTexture::RG_ATI2N_SNorm, QOpenGLTexture::NoPixelType,    16, true } },
    { DXGI_FORMAT_BC6H_UF16,                            { QOpenGLTexture::NoSourceFormat,   QOpenGLTexture::RGB_BP_UNSIGNED_FLOAT, QOpenGLTexture::NoPixelType, 16, true } },
    { DXGI_FORMAT_BC6H_SF16,                            { QOpenGLTexture::NoSourceFormat,   QOpenGLTexture::RGB_BP_SIGNED_FLOAT, QOpenGLTexture::NoPixelType, 16, true } },
    { DXGI_FORMAT_BC7_UNORM,                            { QOpenGLTexture::NoSourceFormat,   QOpenGLTexture::RGB_BP_UNorm, QOpenGLTexture::NoPixelType,      16, true } },

    // compressed sRGB formats
    { DXGI_FORMAT_BC1_UNORM_SRGB,                       { QOpenGLTexture::NoSourceFormat,   QOpenGLTexture::SRGB_DXT1,      QOpenGLTexture::NoPixelType,     8, true } },
    { DXGI_FORMAT_BC1_UNORM_SRGB,                       { QOpenGLTexture::NoSourceFormat,   QOpenGLTexture::SRGB_Alpha_DXT1, QOpenGLTexture::NoPixelType,    8, true } },
    { DXGI_FORMAT_BC2_UNORM_SRGB,                       { QOpenGLTexture::NoSourceFormat,   QOpenGLTexture::SRGB_Alpha_DXT3, QOpenGLTexture::NoPixelType,   16, true } },
    { DXGI_FORMAT_BC3_UNORM_SRGB,                       { QOpenGLTexture::NoSourceFormat,   QOpenGLTexture::SRGB_Alpha_DXT5, QOpenGLTexture::NoPixelType,   16, true } },
    { DXGI_FORMAT_BC7_UNORM_SRGB,                       { QOpenGLTexture::NoSourceFormat,   QOpenGLTexture::SRGB_BP_UNorm,  QOpenGLTexture::NoPixelType,    16, true } },
};

} // namespace

QTexImageDataPrivate::QTexImageDataPrivate()
    : m_width(-1)
    , m_height(-1)
    , m_depth(-1)
    , m_layers(-1)
    , m_faces(-1)
    , m_mipLevels(-1)
    , m_blockSize(-1)
    , m_target(QOpenGLTexture::Target2D)
    , m_format(QOpenGLTexture::RGBA8_UNorm)
    , m_pixelFormat(QOpenGLTexture::RGBA)
    , m_pixelType(QOpenGLTexture::UInt8)
    , m_isCompressed(false)
{
}

QByteArray QTexImageDataPrivate::data(int layer, int face, int mipmapLevel) const
{
    if (layer < 0 || layer >= m_layers ||
            face < 0 || face >= m_faces ||
            mipmapLevel < 0 || mipmapLevel >= m_mipLevels)
        return QByteArray();

    int offset = layer*layerSize() + face*faceSize();

    for (int i = 0; i < mipmapLevel; i++)
        offset += mipmapLevelSize(i);

    return QByteArray::fromRawData(m_data.constData() + offset, mipmapLevelSize(mipmapLevel));
}

void QTexImageDataPrivate::setData(const QByteArray &data, QOpenGLTexture::PixelFormat fmt,
                                   QOpenGLTexture::PixelType ptype)
{
    m_isCompressed = false;
    m_data = data;
    m_layers = 1;
    m_faces = 1;
    m_mipLevels = 1;
    m_target = QOpenGLTexture::Target2D;
    m_pixelFormat = fmt;
    m_pixelType = ptype;
}

bool QTexImageDataPrivate::setCompressedFile(const QString &source)
{
    QString suffix = QFileInfo(source).suffix();

    if (suffix == QStringLiteral("pkm"))
        return setPkmFile(source);
    else if (suffix == QStringLiteral("dds"))
        return setDdsFile(source);
    else
        return false;
}

bool QTexImageDataPrivate::setPkmFile(const QString &source)
{
    QFile f(source);
    if (!f.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open" << source;
        return false;
    }

    // ETC1 in PKM, as generated by f.ex. Android's etc1tool
    static const char pkmMagic[] = { 'P', 'K', 'M', ' ', '1', '0' };
    const int pkmHeaderSize = 6 + 2 + 4 * 2;
    QByteArray header = f.read(pkmHeaderSize);
    if (header.size() >= pkmHeaderSize && !qstrncmp(header.constData(), pkmMagic, 6)) {
        m_format = QOpenGLTexture::RGB8_ETC1; // may get changed to RGB8_ETC2 later on
        // get the extended (multiple of 4) width and height
        m_width = qFromBigEndian(*(reinterpret_cast<const quint16 *>(header.constData() + 6 + 2)));
        m_height = qFromBigEndian(*(reinterpret_cast<const quint16 *>(header.constData() + 6 + 2 + 2)));
        m_depth = 1;
        QByteArray data = f.readAll();
        if (data.size() < (m_width / 4) * (m_height / 4) * 8)
            qWarning() << "Unexpected end of ETC1 data in" << source;
        setData(data, QOpenGLTexture::RGBA, QOpenGLTexture::UInt8);
        m_blockSize = 8;
        m_isCompressed = true;
        return true;
    }

    return false;
}

static int bitCount(quint32 n)
{
    int r = 0;

    for (; n; n >>= 1)
        r += (n & 1);

    return r;
}

bool QTexImageDataPrivate::setDdsFile(const QString &source)
{
    QFile f(source);
    if (!f.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open" << source;
        return false;
    }

    DdsHeader header;
    if (f.read(reinterpret_cast<char *>(&header), sizeof header) != sizeof header)
        return false;

    if (qstrncmp(header.magic, "DDS ", 4) != 0)
        return false;

    // dimensions

    m_width = qFromLittleEndian(header.width);
    m_height = qFromLittleEndian(header.height);

    // mip levels

    if ((qFromLittleEndian(header.flags) & MipmapCountFlag) == MipmapCountFlag)
        m_mipLevels = qFromLittleEndian(header.mipmapCount);
    else
        m_mipLevels = 1;

    // texture format

    m_layers = 1;

    const quint32 pixelFlags = qFromLittleEndian(header.pixelFormat.flags);
    const quint32 fourCC = qFromLittleEndian(header.pixelFormat.fourCC);

    const FormatInfo *formatInfo = nullptr;

    if ((pixelFlags & FourCCFlag) == FourCCFlag) {
        if (fourCC == DdsFourCC<'D', 'X', '1', '0'>::value) {
            // DX10 texture

            DdsDX10Header dx10Header;
            if (f.read(reinterpret_cast<char *>(&dx10Header), sizeof dx10Header) != sizeof dx10Header)
                return false;

            m_layers = qFromLittleEndian(dx10Header.arraySize);

            DXGIFormat format = static_cast<DXGIFormat>(qFromLittleEndian(dx10Header.format));

            for (unsigned i = 0; i < sizeof dx10Formats/sizeof *dx10Formats; i++) {
                if (dx10Formats[i].dxgiFormat == format) {
                    formatInfo = &dx10Formats[i].formatInfo;
                    break;
                }
            }
        } else {
            // compressed, FourCC texture

            for (unsigned i = 0; i < sizeof fourCCFormats/sizeof *fourCCFormats; i++) {
                if (fourCCFormats[i].fourCC == fourCC) {
                    formatInfo = &fourCCFormats[i].formatInfo;
                    break;
                }
            }
        }
    } else {
        // uncompressed texture

        m_pixelFormat = QOpenGLTexture::NoSourceFormat;

        const quint32 rgbBitCount   = qFromLittleEndian(header.pixelFormat.rgbBitCount);
        const quint32 redMask       = qFromLittleEndian(header.pixelFormat.redMask);
        const quint32 greenMask     = qFromLittleEndian(header.pixelFormat.greenMask);
        const quint32 blueMask      = qFromLittleEndian(header.pixelFormat.blueMask);
        const quint32 alphaMask     = qFromLittleEndian(header.pixelFormat.alphaMask);

        for (unsigned i = 0; i < sizeof rgbaFormats/sizeof *rgbaFormats; i++) {
            const RGBAFormat *info = &rgbaFormats[i];

            if (info->formatInfo.components*8u == rgbBitCount &&
                    info->redMask == redMask && info->greenMask == greenMask &&
                    info->blueMask == blueMask && info->alphaMask == alphaMask) {
                formatInfo = &info->formatInfo;
                break;
            }
        }
    }

    if (formatInfo == nullptr) {
            qWarning() << "Unrecognized pixel format in" << source;
            return false;
    }

    m_pixelFormat = formatInfo->pixelFormat;
    m_format = formatInfo->textureFormat;
    m_pixelType = formatInfo->pixelType;
    m_blockSize = formatInfo->components;
    m_isCompressed = formatInfo->compressed;

    // target

    // XXX should worry about Target1D?

    const quint32 caps2Flags = qFromLittleEndian(header.caps2);

    if ((caps2Flags & VolumeFlag) == VolumeFlag) {
        m_target = QOpenGLTexture::Target3D;
        m_depth = qFromLittleEndian(header.depth);
        m_faces = 1;
    } else if ((caps2Flags & CubemapFlag) == CubemapFlag) {
        m_target = m_layers > 1 ? QOpenGLTexture::TargetCubeMapArray : QOpenGLTexture::TargetCubeMap;
        m_depth = 1;
        m_faces = bitCount(caps2Flags & AllCubemapFaceFlags);
    } else {
        m_target = m_layers > 1 ? QOpenGLTexture::Target2DArray : QOpenGLTexture::Target2D;
        m_depth = 1;
        m_faces = 1;
    }

    // data

    m_data = f.readAll();

    if (m_data.size() != m_layers*layerSize()) {
        qWarning() << "Unexpected data size (got " << m_data.size() << ", expecting" << m_layers*layerSize() << ")";
        return false;
    }

    return true;
}

int QTexImageDataPrivate::layerSize() const
{
    return m_faces*faceSize();
}

int QTexImageDataPrivate::faceSize() const
{
    int size = 0;

    for (int i = 0; i < m_mipLevels; i++)
        size += mipmapLevelSize(i);

    return size;
}

// XXX check if this works for ETC1 compression
int QTexImageDataPrivate::mipmapLevelSize(int level) const
{
    int w = qMax(m_width >> level, 1);
    int h = qMax(m_height >> level, 1);
    int d = qMax(m_depth >> level, 1);

    if (m_isCompressed)
        return ((w + 3) / 4) * ((h + 3) / 4) * m_blockSize * d;
    else
        return w * h * m_blockSize * d;
}

QTexImageData::QTexImageData()
    : d_ptr(new QTexImageDataPrivate())
{
}

QTexImageData::QTexImageData(QTexImageDataPrivate &dd)
    : d_ptr(&dd)
{
}

QTexImageData::~QTexImageData()
{
    cleanup();
    delete d_ptr;
}

QTexImageData &QTexImageData::operator=(const QTexImageData &other)
{
    Q_D(QTexImageData);
    *d = *other.d_ptr;
    return *this;
}

void QTexImageData::cleanup()
{
    Q_D(QTexImageData);
    d->m_width = -1;
    d->m_height = -1;
    d->m_depth = -1;
    d->m_layers = -1;
    d->m_faces = -1;
    d->m_mipLevels = -1;
    d->m_blockSize = 0;
    d->m_isCompressed = false;
    d->m_data.clear();
}

bool QTexImageData::isCompressed() const
{
    Q_D(const QTexImageData);
    return d->m_isCompressed;
}

int QTexImageData::width() const
{
    Q_D(const QTexImageData);
    return d->m_width;
}

int QTexImageData::height() const
{
    Q_D(const QTexImageData);
    return d->m_height;
}

int QTexImageData::depth() const
{
    Q_D(const QTexImageData);
    return d->m_depth;
}

int QTexImageData::layers() const
{
    Q_D(const QTexImageData);
    return d->m_layers;
}

int QTexImageData::mipLevels() const
{
    Q_D(const QTexImageData);
    return d->m_mipLevels;
}

int QTexImageData::faces() const
{
    Q_D(const QTexImageData);
    return d->m_faces;;
}

QOpenGLTexture::Target QTexImageData::target() const
{
    Q_D(const QTexImageData);
    return d->m_target;
}

QOpenGLTexture::TextureFormat QTexImageData::format() const
{
    Q_D(const QTexImageData);
    return d->m_format;
}

void QTexImageData::setImage(const QImage &image)
{
    Q_D(QTexImageData);
    d->m_width = image.width();
    d->m_height = image.height();
    d->m_depth = 1;

    QImage glImage = image.convertToFormat(QImage::Format_RGBA8888);
    Q_ASSERT_X(glImage.bytesPerLine() == (glImage.width() * glImage.depth() + 7) / 8,
               "QTexImageData::setImage", "glImage is not packed"); // QTBUG-48330
    QByteArray imageBytes((const char*) glImage.constBits(), glImage.byteCount());
    setData(imageBytes, QOpenGLTexture::RGBA, QOpenGLTexture::UInt8);
    d->m_format = QOpenGLTexture::RGBA8_UNorm;
    d->m_blockSize = 4;
}

void QTexImageData::setData(const QByteArray &data, QOpenGLTexture::PixelFormat fmt,
                            QOpenGLTexture::PixelType ptype)
{
    Q_D(QTexImageData);
    d->setData(data, fmt, ptype);
}

bool QTexImageData::setCompressedFile(const QString &source)
{
    Q_D(QTexImageData);
    return d->setCompressedFile(source);
}

QByteArray QTexImageData::data(int layer, int face, int mipmapLevel) const
{
    Q_D(const QTexImageData);
    return d->data(layer, face, mipmapLevel);
}

QOpenGLTexture::PixelFormat QTexImageData::pixelFormat() const
{
    Q_D(const QTexImageData);
    return d->m_pixelFormat;
}

QOpenGLTexture::PixelType QTexImageData::pixelType() const
{
    Q_D(const QTexImageData);
    return d->m_pixelType;
}

} // namespace Qt3DRender

QT_END_NAMESPACE
