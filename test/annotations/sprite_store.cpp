#include "../fixtures/util.hpp"
#include "../fixtures/fixture_log_observer.hpp"

#include <mbgl/annotation/sprite_store.hpp>

using namespace mbgl;

TEST(Annotations, SpriteStore) {
    FixtureLog log;

    const auto sprite1 = std::make_shared<SpriteImage>(8, 8, 2, std::string(16 * 16 * 4, '\0'));
    const auto sprite2 = std::make_shared<SpriteImage>(8, 8, 2, std::string(16 * 16 * 4, '\0'));
    const auto sprite3 = std::make_shared<SpriteImage>(8, 8, 2, std::string(16 * 16 * 4, '\0'));

    using Sprites = std::map<std::string, std::shared_ptr<const SpriteImage>>;
    SpriteStore store;

    // Adding single
    store.setSprite("one", sprite1);
    EXPECT_EQ(Sprites({
                  { "one", sprite1 },
              }),
              store.getDirty());
    EXPECT_EQ(Sprites(), store.getDirty());

    // Adding multiple
    store.setSprite("two", sprite2);
    store.setSprite("three", sprite3);
    EXPECT_EQ(Sprites({
                  { "two", sprite2 }, { "three", sprite3 },
              }),
              store.getDirty());
    EXPECT_EQ(Sprites(), store.getDirty());

    // Removing
    store.removeSprite("one");
    store.removeSprite("two");
    EXPECT_EQ(Sprites({
                  { "one", nullptr }, { "two", nullptr },
              }),
              store.getDirty());
    EXPECT_EQ(Sprites(), store.getDirty());

    // Accessing
    EXPECT_EQ(sprite3, store.getSprite("three"));

    EXPECT_TRUE(log.empty());

    EXPECT_EQ(nullptr, store.getSprite("two"));
    EXPECT_EQ(nullptr, store.getSprite("four"));

    EXPECT_EQ(1u, log.count({
                      EventSeverity::Info,
                      Event::Sprite,
                      int64_t(-1),
                      "Can't find sprite named 'two'",
                  }));
    EXPECT_EQ(1u, log.count({
                      EventSeverity::Info,
                      Event::Sprite,
                      int64_t(-1),
                      "Can't find sprite named 'four'",
                  }));

    // Overwriting
    store.setSprite("three", sprite1);
    EXPECT_EQ(Sprites({
                  { "three", sprite1 },
              }),
              store.getDirty());
    EXPECT_EQ(Sprites(), store.getDirty());
}

TEST(Annotations, SpriteStoreOtherPixelRatio) {
    FixtureLog log;

    const auto sprite1 = std::make_shared<SpriteImage>(8, 8, 1, std::string(8 * 8 * 4, '\0'));

    using Sprites = std::map<std::string, std::shared_ptr<const SpriteImage>>;
    SpriteStore store;

    // Adding mismatched sprite image
    store.setSprite("one", sprite1);
    EXPECT_EQ(Sprites({ { "one", sprite1 } }), store.getDirty());
}

TEST(Annotations, SpriteStoreMultiple) {
    const auto sprite1 = std::make_shared<SpriteImage>(8, 8, 2, std::string(16 * 16 * 4, '\0'));
    const auto sprite2 = std::make_shared<SpriteImage>(8, 8, 2, std::string(16 * 16 * 4, '\0'));

    using Sprites = std::map<std::string, std::shared_ptr<const SpriteImage>>;
    SpriteStore store;

    store.setSprites({
        { "one", sprite1 }, { "two", sprite2 },
    });
    EXPECT_EQ(Sprites({
                  { "one", sprite1 }, { "two", sprite2 },
              }),
              store.getDirty());
    EXPECT_EQ(Sprites(), store.getDirty());
}

TEST(Annotations, SpriteStoreReplace) {
    FixtureLog log;

    const auto sprite1 = std::make_shared<SpriteImage>(8, 8, 2, std::string(16 * 16 * 4, '\0'));
    const auto sprite2 = std::make_shared<SpriteImage>(8, 8, 2, std::string(16 * 16 * 4, '\0'));

    using Sprites = std::map<std::string, std::shared_ptr<const SpriteImage>>;
    SpriteStore store;

    store.setSprite("sprite", sprite1);
    EXPECT_EQ(sprite1, store.getSprite("sprite"));
    store.setSprite("sprite", sprite2);
    EXPECT_EQ(sprite2, store.getSprite("sprite"));

    EXPECT_EQ(Sprites({ { "sprite", sprite2 } }), store.getDirty());
}

TEST(Annotations, SpriteStoreReplaceWithDifferentDimensions) {
    FixtureLog log;

    const auto sprite1 = std::make_shared<SpriteImage>(8, 8, 2, std::string(16 * 16 * 4, '\0'));
    const auto sprite2 = std::make_shared<SpriteImage>(9, 9, 2, std::string(18 * 18 * 4, '\0'));

    using Sprites = std::map<std::string, std::shared_ptr<const SpriteImage>>;
    SpriteStore store;

    store.setSprite("sprite", sprite1);
    store.setSprite("sprite", sprite2);

    EXPECT_EQ(1u, log.count({
                      EventSeverity::Warning,
                      Event::Sprite,
                      int64_t(-1),
                      "Can't change sprite dimensions for 'sprite'",
                  }));

    EXPECT_EQ(sprite1, store.getSprite("sprite"));

    EXPECT_EQ(Sprites({ { "sprite", sprite1 } }), store.getDirty());
}
