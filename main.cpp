#include "sokol_app.h"
#include "sokol_gfx.h"
#include "sokol_log.h"
#include "sokol_glue.h"
#include "util/sokol_gl.h"
// #include "dbgui/dbgui.h"

#include <math.h>

static struct {
    sg_pass_action pass_action;
    sg_image img;
    sg_sampler smp;
    sgl_pipeline pip_3d;
} state;

static void init(void) {
    sg_setup(&(sg_desc){
        .logger.func = slog_func,
        .context = sapp_sgcontext(),
        
    });
    // __dbgui_setup(sapp_sample_count());

    // setup sokol-gl
    sgl_setup(&(sgl_desc_t){
        .logger.func = slog_func,
    });

    // a checkerboard texture
    uint32_t pixels[8][8];
    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            pixels[y][x] = ((y ^ x) & 1) ? 0xFFFFFFFF : 0xFF0000FF;
        }
    }
    state.img = sg_make_image(&(sg_image_desc){
        .width = 8,
        .height = 8,
        .data.subimage[0][0] = SG_RANGE(pixels)
    });

    // ... and a sampler
    state.smp = sg_make_sampler(&(sg_sampler_desc){
        .min_filter = SG_FILTER_NEAREST,
        .mag_filter = SG_FILTER_NEAREST,
    });

    // default pass action
    state.pass_action = (sg_pass_action) {
        .colors[0] = {
            .load_action = SG_LOADACTION_CLEAR,
            .clear_value = { 0.0f, 0.0f, 0.0f, 1.0f }
        }
    };
}

static void draw_tex_cube(const float t) {
    sgl_defaults();

    sgl_enable_texture();
    sgl_texture(state.img, state.smp);

    sgl_begin_quads();

    sgl_c3f(1.0f, 1.0f, 1.0f);
    sgl_v2f_t2f(-1.0f,  1.0f, -1.0f,  1.0f);
    sgl_v2f_t2f( 1.0f,  1.0f,  1.0f,  1.0f);
    sgl_v2f_t2f( 1.0f, -1.0f,  1.0f, -1.0f);
    sgl_v2f_t2f(-1.0f, -1.0f, -1.0f, -1.0f);

    sgl_end();
}

static void frame(void) {
    const float t = (float)(sapp_frame_duration() * 60.0);

    draw_tex_cube(t);

    /* Render the sokol-gfx default pass, all sokol-gl commands
       that happened so far are rendered inside sgl_draw(), and this
       is the only sokol-gl function that must be called inside
       a sokol-gfx begin/end pass pair.
       sgl_draw() also 'rewinds' sokol-gl for the next frame.
    */
    sg_begin_default_pass(&state.pass_action, sapp_width(), sapp_height());
    sgl_draw();
    // __dbgui_draw();
    sg_end_pass();
    sg_commit();
}

static void cleanup(void) {
    // __dbgui_shutdown();
    sgl_shutdown();
    sg_shutdown();
}

sapp_desc sokol_main(int argc, char* argv[]) {
    (void)argc; (void)argv;
    return (sapp_desc){
        .init_cb = init,
        .frame_cb = frame,
        .cleanup_cb = cleanup,
        // .event_cb = __dbgui_event,
        .width = 512,
        .height = 512,
        .sample_count = 4,
        .window_title = "sokol_gl.h (sokol-app)",
        .icon = { .sokol_default = true },
        .logger = { .func = slog_func },
    };
}