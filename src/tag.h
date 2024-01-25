#pragma once


enum Tag {
	Player,
	Bullet,
	Enemy,
	StaticCollision,
	Particle,
};

static std::unordered_map<Tag, std::string> name_tags = {
	{ Tag::Player, "Player" },
	{ Tag::Bullet, "Bullet" },
	{ Tag::Enemy, "Enemy" },
	{ Tag::StaticCollision, "Collision" },
	{ Tag::Particle, "Particle" },
};

static std::unordered_map<std::string, Tag> tag_names = {
	{ "Player", Tag::Player },
	{ "Bullet", Tag::Bullet },
	{ "Enemy", Tag::Enemy },
	{ "Collision", Tag::StaticCollision },
	{ "Particle", Tag::Particle },
};