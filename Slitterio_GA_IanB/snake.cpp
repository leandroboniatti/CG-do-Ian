#include "snake.h"

Snake::Snake() : direction(1.0f, 0.0f) {
    segments.push_back(glm::vec2(400.0f, 300.0f)); // Posição inicial
}

void Snake::move(float deltaTime) {
    for (size_t i = segments.size() - 1; i > 0; --i) {
        segments[i] = segments[i - 1]; // Cada segmento segue o anterior
    }
    segments[0] += direction * deltaTime * 100.0f; // Cabeça se move
}

void Snake::grow() {
    segments.push_back(segments.back()); // Adiciona um novo segmento
}

const std::vector<glm::vec2>& Snake::getSegments() const {
    return segments;
}

void Snake::setDirection(const glm::vec2& newDirection) {
    // Verifique se a nova direção não é oposta à direção atual
    if (glm::length(newDirection + direction) > 0.1f) {
        direction = newDirection;
    }
}
