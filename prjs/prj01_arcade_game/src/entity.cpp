#include <arcade/entity.hpp>

namespace arcade {

void Entity::clamp_x(float min_x, float max_x) {
    if (bounds_.x < min_x) {
        bounds_.x = min_x;
    }
    if (bounds_.x + bounds_.w > max_x) {
        bounds_.x = max_x - bounds_.w;
    }
}

}  // namespace arcade