.pragma library

function rgbToHsv(r, g, b) {
    r = r/255; g = g/255; b = b/255;
    var max = Math.max(r, g, b), min = Math.min(r, g, b);
    var h, s, v = max;

    var d = max - min;
    s = max == 0 ? 0 : d / max;

    if (max == min){
        h = 0; // achromatic
    } else {
        switch (max) {
            case r: h = (g - b) / d + (g < b ? 6 : 0); break;
            case g: h = (b - r) / d + 2; break;
            case b: h = (r - g) / d + 4; break;
        }
        h /= 6;
    }

    return new Array(h, s, v);
}

function hsvToRgb(h, s, v) {
    var r, g, b;

    var i = Math.floor(h * 6);
    var f = h * 6 - i;
    var p = v * (1 - s);
    var q = v * (1 - f * s);
    var t = v * (1 - (1 - f) * s);

    switch (i % 6){
        case 0: r = v; g = t; b = p; break;
        case 1: r = q; g = v; b = p; break;
        case 2: r = p; g = v; b = t; break;
        case 3: r = p; g = q; b = v; break;
        case 4: r = t; g = p; b = v; break;
        case 5: r = v; g = p; b = q; break;
    }

    return new Array(r * 255, g * 255, b * 255);
}

function hsvToHsl(h, s, v) {
    var ss, l;

    l = ((2.0 - s)*v);

    ss = s*v*((l <= 1) ? l : 2 - l);

    l /= 2;

    return new Array(h, ss, l);
}

function hsvToColor(h, s, v, a) {
    var hsl = hsvToHsl(h,s,v);
    return Qt.hsla(hsl[0], hsl[1], hsl[2], a);
}
