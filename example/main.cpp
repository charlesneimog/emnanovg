#include <cmath>
#include <cstdio>
#include <string>

#include <emscripten.h>
#include <emscripten/html5.h>
#include <emscripten/bind.h>

#include <GLES3/gl3.h>
#include <nanovg.h>
#include <nanovg_gl.h>

using namespace emscripten;

NVGcontext *vg = nullptr;
int width = 0, height = 0;
float animTime = 0.0f;

// Permutation table para Perlin Noise
int perm[512];

// Funções auxiliares Perlin
float fade(float t) {
    return t * t * t * (t * (t * 6 - 15) + 10);
}

float lerp(float a, float b, float t) {
    return a + t * (b - a);
}

float grad(int hash, float x) {
    int h = hash & 15;
    float grad = 1.0f + (h & 7);
    if (h & 8) {
        grad = -grad;
    }
    return grad * x;
}

void initPerm() {
    int p[256] = {151, 160, 137, 91,  90,  15,  131, 13,  201, 95,  96,  53,  194, 233, 7,   225,
                  140, 36,  103, 30,  69,  142, 8,   99,  37,  240, 21,  10,  23,  190, 6,   148,
                  247, 120, 234, 75,  0,   26,  197, 62,  94,  252, 219, 203, 117, 35,  11,  32,
                  57,  177, 33,  88,  237, 149, 56,  87,  174, 20,  125, 136, 171, 168, 68,  175,
                  74,  165, 71,  134, 139, 48,  27,  166, 77,  146, 158, 231, 83,  111, 229, 122,
                  60,  211, 133, 230, 220, 105, 92,  41,  55,  46,  245, 40,  244, 102, 143, 54,
                  65,  25,  63,  161, 1,   216, 80,  73,  209, 76,  132, 187, 208, 89,  18,  169,
                  200, 196, 135, 130, 116, 188, 159, 86,  164, 100, 109, 198, 173, 186, 3,   64,
                  52,  217, 226, 250, 124, 123, 5,   202, 38,  147, 118, 126, 255, 82,  85,  212,
                  207, 206, 59,  227, 47,  16,  58,  17,  182, 189, 28,  42,  223, 183, 170, 213,
                  119, 248, 152, 2,   44,  154, 163, 70,  221, 153, 101, 155, 167, 43,  172, 9,
                  129, 22,  39,  253, 19,  98,  108, 110, 79,  113, 224, 232, 178, 185, 112, 104,
                  218, 246, 97,  228, 251, 34,  242, 193, 238, 210, 144, 12,  191, 179, 162, 241,
                  81,  51,  145, 235, 249, 14,  239, 107, 49,  192, 214, 31,  181, 199, 106, 157,
                  184, 84,  204, 176, 115, 121, 50,  45,  127, 4,   150, 254, 138, 236, 205, 93,
                  222, 114, 67,  29,  24,  72,  243, 141, 128, 195, 78,  66,  215, 61,  156, 180};
    for (int i = 0; i < 256; ++i) {
        perm[i] = p[i];
        perm[i + 256] = p[i];
    }
}

float perlinNoise(float x) {
    int X = (int)floorf(x) & 255;
    float xf = x - floorf(x);
    float u = fade(xf);

    int a = perm[X];
    int b = perm[X + 1];

    return lerp(grad(a, xf), grad(b, xf - 1.0f), u);
}

void drawAnimation() {
    int circleCount = 15;
    int squareCount = 10;
    int lineCount = 20;

    float centerX = width * 0.5f;
    float centerY = height * 0.5f;

    // Círculos animados
    for (int i = 0; i < circleCount; ++i) {
        float nx = perlinNoise(animTime + i * 0.4f);
        float ny = perlinNoise(animTime + i * 0.4f + 100.0f);
        float x = centerX + nx * 250.0f;
        float y = centerY + ny * 250.0f;
        float radius = 10.0f + perlinNoise(animTime * 3 + i) * 8.0f;

        int r = (int)(200 + 55 * sinf(animTime + i));
        int g = (int)(50 + 205 * sinf(animTime * 0.7f + i));
        int b = (int)(50 + 205 * sinf(animTime * 1.3f + i));

        nvgBeginPath(vg);
        nvgCircle(vg, x, y, radius);
        nvgFillColor(vg, nvgRGBA(r, g, b, 220));
        nvgFill(vg);
    }

    // Quadrados animados
    for (int i = 0; i < squareCount; ++i) {
        float nx = perlinNoise(animTime * 1.5f + i * 0.6f + 300);
        float ny = perlinNoise(animTime * 1.5f + i * 0.6f + 400);
        float x = centerX + nx * 200.0f;
        float y = centerY + ny * 200.0f;
        float size = 15.0f + perlinNoise(animTime * 5 + i) * 10.0f;

        int r = (int)(50 + 205 * sinf(animTime * 1.1f + i));
        int g = (int)(200 + 55 * sinf(animTime * 0.9f + i));
        int b = (int)(50 + 205 * sinf(animTime * 1.7f + i));

        nvgBeginPath(vg);
        nvgRect(vg, x - size * 0.5f, y - size * 0.5f, size, size);
        nvgFillColor(vg, nvgRGBA(r, g, b, 210));
        nvgFill(vg);
    }

    // Linhas animadas
    for (int i = 0; i < lineCount; ++i) {
        float nx1 = perlinNoise(animTime * 2 + i * 0.3f);
        float ny1 = perlinNoise(animTime * 2 + i * 0.3f + 100.0f);
        float nx2 = perlinNoise(animTime * 2 + i * 0.3f + 200.0f);
        float ny2 = perlinNoise(animTime * 2 + i * 0.3f + 300.0f);

        float x1 = centerX + nx1 * 300.0f;
        float y1 = centerY + ny1 * 300.0f;
        float x2 = centerX + nx2 * 300.0f;
        float y2 = centerY + ny2 * 300.0f;

        int r = (int)(200 + 55 * sinf(animTime * 0.8f + i));
        int g = (int)(50 + 205 * sinf(animTime * 1.2f + i));
        int b = (int)(50 + 205 * sinf(animTime * 1.7f + i));

        nvgBeginPath(vg);
        nvgMoveTo(vg, x1, y1);
        nvgLineTo(vg, x2, y2);
        nvgStrokeColor(vg, nvgRGBA(r, g, b, 180));
        nvgStrokeWidth(vg, 2.0f);
        nvgStroke(vg);
    }
}

void loop(void *) {
    double pixelRatio = emscripten_get_device_pixel_ratio();
    glViewport(0, 0, width, height);

    glClearColor(1, 1, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    nvgBeginFrame(vg, width, height, pixelRatio);

    drawAnimation();

    nvgEndFrame(vg);

    animTime += 0.01f;
}

void nanoVG(const std::string &canvas) {
    EmscriptenWebGLContextAttributes attr;
    emscripten_webgl_init_context_attributes(&attr);
    attr.alpha = EM_TRUE;
    attr.depth = EM_TRUE;
    attr.stencil = EM_TRUE;
    attr.antialias = EM_TRUE;
    attr.majorVersion = 2;

    double cssWidth, cssHeight;

    std::string canvasId = "#" + canvas;
    emscripten_get_element_css_size(canvasId.c_str(), &cssWidth, &cssHeight);

    double devicePixelRatio = emscripten_get_device_pixel_ratio();
    width = static_cast<int>(cssWidth * devicePixelRatio);
    height = static_cast<int>(cssHeight * devicePixelRatio);

    EMSCRIPTEN_WEBGL_CONTEXT_HANDLE ctx = emscripten_webgl_create_context(canvasId.c_str(), &attr);
    emscripten_webgl_make_context_current(ctx);

    vg = nvgCreateGLES3(NVG_ANTIALIAS | NVG_STENCIL_STROKES);
    if (!vg) {
        printf("Could not init nanovg.\n");
        return;
    }

    initPerm();

    emscripten_set_main_loop_arg(loop, nullptr, 0, 1);
}

EMSCRIPTEN_BINDINGS(module) {
    function("nanoVG", &nanoVG);
}
