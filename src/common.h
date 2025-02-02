#ifndef COMMON_H
#define COMMON_H

#include <GL/freeglut.h>
#include <iostream>

#define ASSERT(x) if (!(x)) __builtin_debugtrap()
#define GLCall(x) GLClearError(); \
        x; \
        ASSERT(GLLogCall(#x, __FILE__, __LINE__))

inline void GLClearError() {
    while (glGetError() != GL_NO_ERROR);
}

inline bool GLLogCall(const char* func, const char* file, int line) {
    while (GLenum err = glGetError()) {
        std::cerr << "Error (OpenGL): " << err << " in function " << func << " at " << file << ":" << line << std::endl;
        return false;
    }

    return true;
}

#endif // COMMON_H 