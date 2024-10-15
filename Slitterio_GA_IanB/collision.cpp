#include "collision.h"

bool Collision::checkSelfCollision(const Snake& snake) {
    const auto& segments = snake.getSegments();
    for (size_t i = 1; i < segments.size(); ++i) {
        if (glm::distance(segments[0], segments[i]) < 1.0f) {
            return true;
        }
    }
    return false;
}

bool Collision::checkBorderCollision(const Snake& snake, float width, float height) {
    const glm::vec2& head = snake.getSegments()[0];
    if (head.x < 0 || head.x > width || head.y < 0 || head.y > height) {
        return true;
    }
    return false;
}
