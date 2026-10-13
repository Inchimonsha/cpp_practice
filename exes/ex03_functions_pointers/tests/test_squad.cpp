// Тесты логики отряда ex03. Проверяют контракт shared/squad.hpp.

#include <ex03/squad.hpp>

#include <cpp_course/test.hpp>

using namespace ex03;

CPP_COURSE_TEST("heal: восстанавливает HP с капом по max_hp") {
    Fighter f{"Test", 40, 100, 10};
    heal(f, 10);
    CHECK_EQ(f.hp, 50);
    heal(f, 1000);
    CHECK_EQ(f.hp, 100);
}

CPP_COURSE_TEST("heal: не воскрешает мёртвых") {
    Fighter f{"Corpse", 0, 100, 0};
    heal(f, 50);
    CHECK_EQ(f.hp, 0);
}

CPP_COURSE_TEST("heal: не воскрешает и лечит с нуля") {
    Fighter f{"Almost", 0, 50, 5};
    heal(f, 5);
    CHECK_EQ(f.hp, 0);  // мёртвый остаётся мёртвым
}

CPP_COURSE_TEST("try_hit: nullptr безопасен") {
    CHECK(!try_hit(nullptr, 5));
}

CPP_COURSE_TEST("try_hit: уменьшает HP и возвращает жив ли") {
    Fighter f{"Guard", 40, 40, 8};
    CHECK(try_hit(&f, 10));
    CHECK_EQ(f.hp, 30);
    CHECK(!try_hit(&f, 100));
    CHECK_EQ(f.hp, 0);
}

CPP_COURSE_TEST("try_hit: урон не опускает ниже нуля") {
    Fighter f{"Stone", 3, 10, 1};
    (void)try_hit(&f, 1000);
    CHECK_EQ(f.hp, 0);
}

CPP_COURSE_TEST("find_weakest: пустой отряд → nullptr") {
    std::vector<Fighter> empty;
    CHECK(find_weakest(empty) == nullptr);
}

CPP_COURSE_TEST("find_weakest: возвращает живого с минимальным HP") {
    std::vector<Fighter> squad{
        {"Mage", 60, 100, 25},
        {"Warrior", 90, 100, 20},
        {"Rogue", 30, 60, 35},
    };
    const Fighter* weakest = find_weakest(squad);
    CHECK(weakest != nullptr);
    CHECK_EQ(weakest->name, "Rogue");
}

CPP_COURSE_TEST("find_weakest: игнорирует мёртвых") {
    std::vector<Fighter> squad{
        {"Dead", 0, 100, 10},
        {"Solo", 40, 100, 20},
        {"AlsoDead", 0, 100, 5},
    };
    const Fighter* weakest = find_weakest(squad);
    CHECK(weakest != nullptr);
    CHECK_EQ(weakest->name, "Solo");
}

CPP_COURSE_TEST("find_weakest: все мертвы → nullptr") {
    std::vector<Fighter> squad{
        {"A", 0, 10, 1},
        {"B", 0, 10, 1},
    };
    CHECK(find_weakest(squad) == nullptr);
}

CPP_COURSE_TEST("total_damage: суммирует урон только живых") {
    std::vector<Fighter> squad{
        {"Mage", 60, 100, 25},
        {"Dead", 0, 100, 100},
        {"Rogue", 30, 60, 35},
    };
    CHECK_EQ(total_damage(squad), 60);
}

CPP_COURSE_TEST("total_damage: пустой отряд → 0") {
    std::vector<Fighter> empty;
    CHECK_EQ(total_damage(empty), 0);
}