#include <GL/gl.h>
#include <GL/glext.h>
#include "glImpl.h"
#include "driver_3ds.h"

GLAPI void APIENTRY glScissorMode( GLenum mode ) {
    CHECK_NULL(g_state);

    switch (mode) {
        case GL_SCISSOR_NORMAL_DMP:
        case GL_SCISSOR_INVERT_DMP: {
            ((gfx_device_3ds *)g_state->device)->ext_state.scissorMode = mode;
        } break;

        default: {
            setError(GL_INVALID_ENUM);
            return;
        } break;
    }
}
