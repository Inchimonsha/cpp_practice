// m09_practice/ex08_raytracer — trassirovshchik sfer (ASCII-vyvod).
// Standart: C++23. Zapusk: ./run.ps1 m09_practice/ex08_raytracer
#include <algorithm>
#include <cmath>
#include <iostream>
#include <optional>
#include <print>
#include <string>
#include <vector>

namespace {

struct Vec3 {
    double x;
    double y;
    double z;
};

Vec3 operator+(const Vec3& a, const Vec3& b) { return {a.x + b.x, a.y + b.y, a.z + b.z}; }
Vec3 operator-(const Vec3& a, const Vec3& b) { return {a.x - b.x, a.y - b.y, a.z - b.z}; }
Vec3 operator*(const Vec3& v, double s) { return {v.x * s, v.y * s, v.z * s}; }
double dot(const Vec3& a, const Vec3& b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
double length(const Vec3& v) { return std::sqrt(dot(v, v)); }
Vec3 normalize(const Vec3& v) { return v * (1.0 / length(v)); }

struct Ray {
    Vec3 origin;
    Vec3 direction;
    Vec3 at(double t) const { return origin + direction * t; }
};

struct Sphere {
    Vec3 center;
    double radius;
    double albedo;
};

std::optional<double> hit(const Sphere& sphere, const Ray& ray) {
    const Vec3 oc = ray.origin - sphere.center;
    const double a = dot(ray.direction, ray.direction);
    const double b = 2.0 * dot(oc, ray.direction);
    const double c = dot(oc, oc) - sphere.radius * sphere.radius;
    const double discriminant = b * b - 4.0 * a * c;
    if (discriminant < 0.0) { return std::nullopt; }

    const double root = std::sqrt(discriminant);
    double t = (-b - root) / (2.0 * a);
    if (t < 1e-4) { t = (-b + root) / (2.0 * a); }
    if (t < 1e-4) { return std::nullopt; }
    return t;
}

constexpr int kWidth = 32;
constexpr int kHeight = 16;

char shade_char(double intensity) {
    static const std::string ramp = " .:-=+*#%@";
    const int index = std::clamp(static_cast<int>(intensity * (ramp.size() - 1) + 0.5),
                                 0, static_cast<int>(ramp.size() - 1));
    return ramp[static_cast<std::size_t>(index)];
}

}  // namespace

int main() {
    const std::vector<Sphere> scene = {
        {{0.0, 0.0, -3.0}, 1.0, 0.95},
        {{1.6, 0.4, -4.0}, 0.7, 0.6},
    };
    const Vec3 light = normalize(Vec3{0.5, 1.0, 0.3});
    const double aspect = static_cast<double>(kWidth) / kHeight;

    int lit = 0;
    for (int y = 0; y < kHeight; ++y) {
        std::string row;
        for (int x = 0; x < kWidth; ++x) {
            const double u = (2.0 * (x + 0.5) / kWidth - 1.0) * aspect;
            const double v = 1.0 - 2.0 * (y + 0.5) / kHeight;
            const Ray ray{{0.0, 0.0, 0.0}, normalize(Vec3{u, v, -1.0})};

            double closest = 1e30;
            double intensity = 0.0;
            for (const Sphere& sphere : scene) {
                if (auto t = hit(sphere, ray); t && *t < closest) {
                    closest = *t;
                    const Vec3 point = ray.at(*t);
                    const Vec3 normal = normalize(point - sphere.center);
                    const double diffuse = std::max(0.0, dot(normal, light));
                    intensity = sphere.albedo * (0.1 + 0.9 * diffuse);
                }
            }
            if (intensity > 0.0) { ++lit; }
            row.push_back(shade_char(intensity));
        }
        std::println("{}", row);
    }

    const Ray center{{0.0, 0.0, 0.0}, {0.0, 0.0, -1.0}};
    const auto center_hit = hit(scene[0], center);
    const auto up_miss = hit(scene[0], Ray{{0.0, 0.0, 0.0}, {0.0, 1.0, 0.0}});

    const bool center_ok = center_hit && std::abs(*center_hit - 2.0) < 1e-9;
    std::println("center hit t = {}", center_hit ? *center_hit : -1.0);
    std::println("up ray hit = {}", up_miss.has_value());
    std::println("lit pixels = {}", lit);

    std::cout << "raytracer done\n";
    return (center_ok && !up_miss && lit > 0) ? 0 : 1;
}
