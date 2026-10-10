#include <Elementary.h>
#include <gst/gst.h>
#include <libopenmpt/libopenmpt.h>
int main(int argc, char **argv) {
    gst_init(&argc, &argv);
    const char *version = openmpt_get_string("library_version");
    openmpt_free_string(version);
    return elm_init(argc, argv) < 0;
}
