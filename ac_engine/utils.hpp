#ifndef AC_UTILS
#define AC_UTILS

struct vec2 { int x; int y; };
struct vec2d { double x; double y; };

struct color { int R; int G; int B; int A; };

struct cellOBJ {
    int x;
    int y;
    color setColor;
};
#endif