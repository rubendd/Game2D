#include <SFML/Graphics.hpp>
#include "Hero.h";
#include "Enemy.h";
#include "Rocket.h";
#include <SFML/Audio.hpp>



// Creamos el tamaño de la ventana.
sf::Vector2f viewSize(1024, 768);

// Indicamos a la clase VideoMode el tamaño.
sf::VideoMode vm(viewSize.x, viewSize.y);

// Creamos la ventana. Tres parámetros (VideoMode, String title, Style)
sf::RenderWindow window(vm, "Hola soy el juego", sf::Style::Default);

//Audio
sf::Music bgMusic;

sf::SoundBuffer fireBuffer, hitBuffer;

sf::Sound fireSound(fireBuffer); 
sf::Sound hitSound(hitBuffer);

//Texto.
sf::Font fuenteCabecera;
sf::Text textoCabecera;

sf::Font scoreFont;
sf::Text scoreText;

sf::Text tutorialText;



//Creacion de Texturas.
sf::Texture skyTexture;
sf::Texture bgTexture;



//Creacion de sprites.
sf::Sprite skySprite;
sf::Sprite bgSprite;


int puntuacion = 0;
bool gameover = true;

//Resetear el juego.
void reset();

//Declaro el héroe.
Hero hero;

//Enemigos.
std::vector<Enemy*> enemies;
void spawnEnemy();

//Cohetes.
std::vector<Rocket*> rockets;
void shoot();

//Collision detection.
bool checkCollision(sf::Sprite sprite1, sf::Sprite sprite2);


float currentTime;
float prevTime = 0.0f;

//Movimiento del jugador.
sf::Vector2f playerPosition;
bool playerMoving = false;






//Cargar sprite.
void init() {

	//Audio
	bgMusic.openFromFile("Assets/audio/himno-del-cordoba.ogg");
	bgMusic.play();
	bgMusic.setLoop(true);

	hitBuffer.loadFromFile("Assets/audio/hit.ogg");
	fireBuffer.loadFromFile("Assets/audio/fire.ogg");
	
	
	//Cargar texturas.
	skyTexture.loadFromFile("Assets/graphics/sky.png");
	bgTexture.loadFromFile("Assets/graphics/bg.png");

	//Cargar fuente.
	fuenteCabecera.loadFromFile("Assets/fonts/SnackerComic.ttf");
	scoreFont.loadFromFile("Assets/fonts/pixelfont.ttf");

	//Setear texto.
	textoCabecera.setFont(fuenteCabecera);
	textoCabecera.setString("Tiny bazooka :D");
	textoCabecera.setCharacterSize(84);
	textoCabecera.setFillColor(sf::Color::Red);

	//Setear score texto.
	scoreText.setFont(scoreFont);
	scoreText.setString("Puntuacion: 0");
	scoreText.setCharacterSize(45);
	scoreText.setFillColor(sf::Color::Red);

	sf::FloatRect scoreBounds = scoreText.getLocalBounds();
	scoreText.setOrigin(scoreBounds.width / 2, scoreBounds.height / 2);
	scoreText.setPosition(sf::Vector2f(viewSize.x * 0.5f, viewSize.y * 0.10f));

	//Establecemos el rectangulo de la cabecera.
	sf::FloatRect cabeceraBound = textoCabecera.getLocalBounds();
	textoCabecera.setOrigin(cabeceraBound.width / 2, cabeceraBound.height / 2);
	textoCabecera.setPosition(sf::Vector2f(viewSize.x * 0.5f, viewSize.y * 0.10f));

	//texto tutorial.
	tutorialText.setFont(scoreFont);
	tutorialText.setString("Pulsa espacio para para atacar y \n flecha de arriba o W para saltar");
	tutorialText.setCharacterSize(35);
	tutorialText.setFillColor(sf::Color::Red);

	sf::FloatRect tutorialBounds = tutorialText.getLocalBounds();
	tutorialText.setOrigin(tutorialBounds.width / 2, tutorialBounds.height / 2);
	tutorialText.setPosition(sf::Vector2f(viewSize.x * 0.5f, viewSize.y * 0.25f));
	

	//Settear y atach la textura al sprite.
	skySprite.setTexture(skyTexture);
	bgSprite.setTexture(bgTexture);

	//Iniciar personajes.
	hero.init("Assets/graphics/hero.png", sf::Vector2f(viewSize.x * 0.25f, viewSize.y * 0.25f), 200);
	srand((int)time(0));

}


//Dibujar sprite.
void draw() {
	window.draw(skySprite);
	window.draw(bgSprite);
	window.draw(hero.getSprite());

	for (Enemy* enemy : enemies)
	{
		window.draw(enemy->getSprite());
	}
	for (Rocket *rocket : rockets) {
		window.draw(rocket->getSprite());
	}

	if (gameover)
	{
		window.draw(textoCabecera);
		window.draw(tutorialText);
	}
	else {
		window.draw(scoreText);
	}
	
}

//Creamos una funcion para usar los pollEvents y así checkear los eventos que puedan
//estar ocurriendo.
void updateInput() {
	sf::Event event;

	while (window.pollEvent(event)) {
		if (event.type == sf::Event::KeyPressed)
		{
			if (event.key.code == sf::Keyboard::Up || event.key.code == sf::Keyboard::W)
			{
				hero.jump(700.0f);
			}
			if (event.key.code == sf::Keyboard::Space)
			{
				if (gameover)
				{
					gameover = false;
					reset();
				}
				else {
					shoot();
				}
			}			
			if (event.key.code == sf::Keyboard::Escape || event.type == sf::Event::Closed)
			{
				window.close();
			}
		}
	}
}


void update(float dt) {
	hero.update(dt);
	currentTime += dt;

	//Spawn enemies.
	if (currentTime >= prevTime + 1.125f) {

		spawnEnemy();
		prevTime = currentTime;

	}

	//Actualizar enemigos.
	for (int i = 0; i < enemies.size(); i++)
	{
		Enemy *enemy = enemies[i];

		enemy->update(dt);

		if (enemy->getSprite().getPosition().x < 0)
		{
			enemies.erase(enemies.begin() + i);
			delete(enemy);
			gameover = true;

		}
	}

	//Actualizar cohetes.
	for (int i = 0; i < rockets.size(); i++)
	{
		Rocket* rocket = rockets[i];

		rocket->update(dt);

		if (rocket->getSprite().getPosition().x > viewSize.x)
		{
			rockets.erase(rockets.begin() + i);
			delete(rocket);
		}
	}

	//Check collision entre cohetes y enemigos.
	for (int i = 0; i < rockets.size(); i++)
	{
		for (int j = 0; j < enemies.size(); j++)
		{
			Rocket* rocket = rockets[i];
			Enemy* enemy = enemies[j];

			if (checkCollision(rocket->getSprite(), enemy->getSprite()))
			{
				hitSound.play();
				puntuacion++;
				scoreText.setString("Puntuacion: " + std::to_string(puntuacion));


				sf::FloatRect scoreBounds = scoreText.getLocalBounds();
				scoreText.setOrigin(scoreBounds.width / 2, scoreBounds.height / 2);
				scoreText.setPosition(sf::Vector2f(viewSize.x * 0.5f, viewSize.y * 0.10f));


				rockets.erase(rockets.begin() + i);
				enemies.erase(enemies.begin() + j);

				
				delete(rocket);
				delete(enemy);

				printf("cohete impactado \n");
			}
		}
	}


}



void spawnEnemy() {
	int randLoc = rand() % 3;

	sf::Vector2f enemyPos;

	float speed;

	switch (randLoc)
	{
	case 0: enemyPos = sf::Vector2f(viewSize.x, viewSize.y * 0.75f);
		speed = -400;
		break;
	case 1: enemyPos = sf::Vector2f(viewSize.x, viewSize.y * 0.60f);
		speed = -550;
		break;
	case 2: enemyPos = sf::Vector2f(viewSize.x, viewSize.y * 0.40f);
		speed = -650;
		break;


	default:
		printf("Incorrect y value \n");
		return;
	}

	Enemy* enemy = new Enemy();
	enemy->init("Assets/graphics/enemy.png", enemyPos, speed);

	enemies.push_back(enemy);
}


void shoot() {
	Rocket* rocket = new Rocket();

	rocket->init("Assets/graphics/rocket.png", hero.getSprite().getPosition(), 400.0f);
	fireSound.play();
	rockets.push_back(rocket);
	
}

bool checkCollision(sf::Sprite sprite, sf::Sprite sprite2) {

	//Esto esta bastante guapo porque crea un rectangulo invisible al rededor del sprite.
	sf::FloatRect shape1 = sprite.getGlobalBounds();
	sf::FloatRect shape2 = sprite2.getGlobalBounds();

	//Si el rectangulo de la forma 1 toca el rectangulo de la forma 2 pues bum.
	if (shape1.intersects(shape2))
	{
		return true;
	}
	else {
		return false;
	}
}


void reset() {
	puntuacion = 0;
	currentTime = 0.0f;
	prevTime = 0.0f;
	scoreText.setString("Score: 0");

	for (Enemy *enemy : enemies)
	{
		delete(enemy);
	}
	for (Rocket* rocket : rockets)
	{
		delete(rocket);
	}

	enemies.clear();
	rockets.clear();
	
}

int main() {


	sf::Clock clock;
	init();

	while (window.isOpen()) {

		//Update input.
		updateInput();

		//Update Game.
		sf::Time dt = clock.restart();
		if (!gameover)
		{
			update(dt.asSeconds());
		}
		

		//agregar lógica de juego y actualización de objetos.
		window.clear(sf::Color::Red);
		draw();

		// Renderizar objetos de juego aquí.
		window.display();

	}

	return 0;
}
