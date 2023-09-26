#include <SFML/Graphics.hpp>
#include "Hero.h";


// Creamos el tamaño de la ventana.
sf::Vector2f viewSize(1024, 768);

// Indicamos a la clase VideoMode el tamaño.
sf::VideoMode vm(viewSize.x, viewSize.y);

// Creamos la ventana. Tres parámetros (VideoMode, String title, Style)
sf::RenderWindow window(vm, "Hola soy el juego", sf::Style::Default);

//Creacion de Texturas.
sf::Texture skyTexture;
sf::Texture bgTexture;



//Creacion de sprites.
sf::Sprite skySprite;
sf::Sprite bgSprite;


Hero hero;

//Movimiento del jugador.
sf::Vector2f playerPosition;
bool playerMoving = false;



//Cargar sprite.
void init() {
    //Cargar texturas.
    skyTexture.loadFromFile("Assets/graphics/sky.png");
    bgTexture.loadFromFile("Assets/graphics/bg.png");
    

    //Settear y atach la textura al sprite.
    skySprite.setTexture(skyTexture);
    bgSprite.setTexture(bgTexture);
    hero.init("Assets/graphics/hero.png", sf::Vector2f(viewSize.x * 0.25f, viewSize.y * 0.25f), 200);
   

}


//Dibujar sprite.
void draw() {
    window.draw(skySprite);
    window.draw(bgSprite);
    window.draw(hero.getSprite());        
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
            if (event.key.code == sf::Keyboard::Escape || event.type == sf::Event::Closed)
            {
                window.close();
            }
        }
    }
}


void update(float dt) {
    hero.update(dt);
}

int main() {
   

    sf::Clock clock;
    init();

    while (window.isOpen()) {
        
        //Update input.
        updateInput();

        //Update Game.
        sf::Time dt = clock.restart();
        update(dt.asSeconds());

        // Aquí puedes agregar lógica de juego y actualización de objetos.
        window.clear(sf::Color::Red);
        draw();

        // Renderizar objetos de juego aquí.
        window.display();

    }

    return 0;
}
