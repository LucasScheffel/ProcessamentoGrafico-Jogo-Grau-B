typedef struct Object {
    unsigned int tid;
    const char* filename;
    float offsetx, offsety;
    float z;
    int width, height;
    int currentFrame;
    int direction;
    float speed;
} Object;