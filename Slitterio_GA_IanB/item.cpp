#include "item.h"

Item::Item(float x, float y) : position(x, y) {}

glm::vec2 Item::getPosition() const {
    return position;
}
