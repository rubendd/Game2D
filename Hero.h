

#pragma once


#include <SFML/Graphics.hpp>
#include <stdlib.h>
class Hero
{
public:
	Hero();
	~Hero();

	void init(std::string textureName, sf::Vector2f posicion, float mass);

	void update(float dt);

	void jump(float velocity);

	sf::Sprite getSprite();

private:
	sf::Texture m_texture;
	sf::Sprite m_sprite;
	sf::Vector2f m_position;

	const float m_gravity = 9.80f;
	int jumpCount = 0;
	float m_mass;
	float m_velocity;
	float m_force;
	bool m_grounded;

};

