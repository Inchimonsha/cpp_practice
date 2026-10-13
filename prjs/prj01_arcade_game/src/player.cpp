#include <arcade/player.hpp>

namespace arcade {

Player::Player(Aabb bounds, float speed) : bounds_(bounds), speed_(speed) {}

void Player::move_left(float dt) {
    bounds_.x -= speed_ * dt;
}

void Player::move_right(float dt) {
    bounds_.x += speed_ * dt;
}

void Player::clamp_to_field(float min_x, float max_x) {
    if (bounds_.x < min_x) {
        bounds_.x = min_x;
    }
    if (bounds_.x + bounds_.w > max_x) {
        bounds_.x = max_x - bounds_.w;
    }
}

}  // namespace arcade