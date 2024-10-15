#ifndef ITEM_H
#define ITEM_H

#include <glm/glm.hpp>

class Item {
public:
    Item(float x, float y);
    glm::vec2 getPosition() const;

private:
    glm::vec2 position;
};

#endif
