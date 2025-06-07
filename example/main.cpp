#include <string>
#include <iostream>
#include <cmath>
#include <stdio.h>

#include <emscripten.h>
#include <emscripten/html5.h>
#include <emscripten/bind.h>

#include <GLES3/gl3.h>
#include <nanovg.h>
#include <nanovg_gl.h>

NVGcontext *vg = nullptr;
float angle = 0.0f;
float textX = 0.0f;
int fontHandle = -1;
int width, height;

// Loop de animação
void loop() {
    glViewport(0, 0, width, height);
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    nvgBeginFrame(vg, width, height, 1.0f);

    // Retângulo rotativo
    nvgSave(vg);
    nvgTranslate(vg, width / 2.0f, height / 2.0f);
    nvgRotate(vg, angle);
    nvgBeginPath(vg);
    nvgRect(vg, -50, -50, 100, 100);
    nvgFillColor(vg, nvgRGBA(255, 0, 0, 255));
    nvgFill(vg);
    nvgRestore(vg);

    // Texto móvel
    if (fontHandle >= 0) {
        nvgFontFaceId(vg, fontHandle);
        nvgFontSize(vg, 48.0f);
        nvgFillColor(vg, nvgRGBA(185, 150, 0, 255));
        nvgTextAlign(vg, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
        nvgText(vg, textX, height * 0.8f, "Hello World!", nullptr);
    }

    nvgEndFrame(vg);

    // Atualiza variáveis para animação
    angle += 0.01f;
    textX += 2.0f;
    if (textX > width) {
        textX = -400; // reseta para fora da tela
    }
}

void nanoVG(std::string canvas) {
    EmscriptenWebGLContextAttributes attr;
    emscripten_webgl_init_context_attributes(&attr);
    attr.alpha = EM_TRUE;
    attr.depth = EM_TRUE;
    attr.stencil = EM_TRUE;
    attr.antialias = EM_TRUE;
    attr.majorVersion = 2;

    std::string selector = "#" + canvas;
    emscripten_get_canvas_element_size(selector.c_str(), &width, &height);

    EMSCRIPTEN_WEBGL_CONTEXT_HANDLE ctx = emscripten_webgl_create_context(selector.c_str(), &attr);
    if (ctx <= 0) {
        printf("Failed to create WebGL context\n");
        return;
    }

    emscripten_webgl_make_context_current(ctx);

    vg = nvgCreateGLES3(NVG_ANTIALIAS | NVG_STENCIL_STROKES);
    if (!vg) {
        printf("Could not init NanoVG.\n");
        return;
    }

    fontHandle = nvgCreateFont(vg, "roboto", "Roboto-Regular.ttf");
    if (fontHandle == -1) {
        printf("Failed to load font Roboto-Regular.ttf\n");
    }

    emscripten_set_main_loop(loop, 0, 1);
}

EMSCRIPTEN_BINDINGS(Module) {
    emscripten::function("nanoVG", &nanoVG);
}
