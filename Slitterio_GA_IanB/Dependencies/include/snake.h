#ifndef SNAKE_H
#define SNAKE_H

#include <vector>
#include <glm/glm.hpp>

class Snake {
public:
    Snake();
    void move(float deltaTime);
    void grow();
    void setDirection(const glm::vec2& newDirection);
    const std::vector<glm::vec2>& getSegments() const;

private:
    std::vector<glm::vec2> segments;
    glm::vec2 direction;
};

#endif
