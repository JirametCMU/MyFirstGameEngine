#include <catch2/catch_test_macros.hpp>
#include "Engine/ECS/World.hpp"

using namespace Engine;

struct TestComponentA { int x; };
struct TestComponentB { float y; };

TEST_CASE("World Create/Destroy Entity", "[ECS]") {
    World world;
    Entity e1 = world.CreateEntity();
    REQUIRE(e1.IsValid());
    REQUIRE(world.IsValid(e1));

    world.DestroyEntity(e1);
    REQUIRE(world.IsValid(e1)); // still valid until flush
    world.FlushDeferred();
    REQUIRE_FALSE(world.IsValid(e1));

    Entity e2 = world.CreateEntity();
    REQUIRE(e2.IsValid());
    REQUIRE(e1.index == e2.index);
    REQUIRE(e1.generation != e2.generation);
}

TEST_CASE("World Add/Get/Remove Component", "[ECS]") {
    World world;
    Entity e = world.CreateEntity();
    
    world.AddComponent<TestComponentA>(e, {42});
    REQUIRE(world.HasComponent<TestComponentA>(e));
    
    TestComponentA* compA = world.GetComponent<TestComponentA>(e);
    REQUIRE(compA != nullptr);
    REQUIRE(compA->x == 42);

    REQUIRE_FALSE(world.HasComponent<TestComponentB>(e));
    REQUIRE(world.GetComponent<TestComponentB>(e) == nullptr);

    world.RemoveComponent<TestComponentA>(e);
    REQUIRE_FALSE(world.HasComponent<TestComponentA>(e));
}

TEST_CASE("World Each Iteration", "[ECS]") {
    World world;
    Entity e1 = world.CreateEntity();
    Entity e2 = world.CreateEntity();
    Entity e3 = world.CreateEntity();

    world.AddComponent<TestComponentA>(e1, {1});
    world.AddComponent<TestComponentB>(e1, {1.0f});

    world.AddComponent<TestComponentA>(e2, {2});

    world.AddComponent<TestComponentA>(e3, {3});
    world.AddComponent<TestComponentB>(e3, {3.0f});

    int countA = 0;
    world.Each<TestComponentA>([&](Entity e, TestComponentA& a) {
        countA++;
    });
    REQUIRE(countA == 3);

    int countAB = 0;
    world.Each<TestComponentA, TestComponentB>([&](Entity e, TestComponentA& a, TestComponentB& b) {
        countAB++;
    });
    REQUIRE(countAB == 2);
}

TEST_CASE("World Stale Handle Rejection", "[ECS]") {
    World world;
    Entity e = world.CreateEntity();
    world.AddComponent<TestComponentA>(e, {10});

    world.DestroyEntity(e);
    world.FlushDeferred();

    REQUIRE_FALSE(world.HasComponent<TestComponentA>(e));
    REQUIRE(world.GetComponent<TestComponentA>(e) == nullptr);

    world.AddComponent<TestComponentA>(e, {20});
    REQUIRE_FALSE(world.HasComponent<TestComponentA>(e));
}
