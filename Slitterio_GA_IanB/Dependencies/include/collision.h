#ifndef COLLISION_H
#define COLLISION_H

#include "snake.h"

class Collision {
public:
    static bool checkSelfCollision(const Snake& snake);
    static bool checkBorderCollision(const Snake& snake, float width, float height);
};

#endif
