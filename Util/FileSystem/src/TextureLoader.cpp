#include "include/TextureLoader.h"

#include <stb_image.h>

#include <Debug.h>

unsigned char *file_system::LoadTexture(const std::string &file, int *width, int *height) {
    int channels;
    // Weird texture because of different tex coordinates
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(file.c_str(), width, height, &channels, STBI_rgb_alpha);

    DEBUG_ASSERT(data != nullptr);

    return data;
}
