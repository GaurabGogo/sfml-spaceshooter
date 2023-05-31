#include <iostream>
#include "SFML\Graphics.hpp"
#include "SFML\Window.hpp"
#include "SFML\System.hpp"
#include "SFML\Audio.hpp"
#include "SFML\Network.hpp"
#include <cstdlib>
#include <vector>
#include <math.h>

using namespace sf;
using  namespace std;

class Bullet {
public:
    Sprite shape;
    Bullet(Texture* texture, Vector2f pos)
    {
        this->shape.setTexture(*texture);
        this->shape.setPosition(pos);
        this->shape.setScale(0.06f, 0.06f);
    }
    ~Bullet() {} 
};

class Player {
public:
    Sprite shape;
    Texture *texture;

    int HP;
    int HPMax;

    vector<Bullet> bullets;

    Player(Texture *texture)
    {
        this->HPMax = 10;
        this->HP = this->HPMax;

        this->texture = texture;
        this->shape.setTexture(*texture);

        this->shape.setScale(0.1f, 0.1f);
    }
    ~Player(){}
};

class Enemy {
public:
    Sprite shape;

    int HP;
    int HPMax;

    Enemy(Texture* texture, Vector2u windowSize)
    {
        this->HPMax = rand() % 3 + 1;
        this->HP = this->HPMax;

        this->shape.setTexture(*texture);
        this->shape.setScale(0.2f, 0.2f);
        this->shape.setPosition(windowSize.x - this->shape.getGlobalBounds().width, rand() %(int)( windowSize.y-this->shape.getGlobalBounds().height));

    }
    ~Enemy(){}
};





int main()
{

    srand(time(NULL));
    //Draw Window
    RenderWindow window(VideoMode(800, 600), "Rosu Doge",Style::Default);
    window.setFramerateLimit(60);

    //Font
    Font font;
    font.loadFromFile("Fonts/Arial Black.ttf");

    //Init Textures
    Texture playerTex;
    playerTex.loadFromFile("Textures/SpacePlayer.png");

    Texture enemyTex;
    enemyTex.loadFromFile("Textures/SpaceEnemy.png");

    Texture bulletTex;
    bulletTex.loadFromFile("Textures/MissilesRight.png");

    //UI init
    Text scoreText;
    scoreText.setFont(font);
    scoreText.setCharacterSize(20);
    scoreText.setFillColor(Color::White);
    scoreText.setPosition(10.f, 10.f);

    Text gameOverText;
    gameOverText.setFont(font);
    gameOverText.setCharacterSize(30);
    gameOverText.setFillColor(Color::Red);
    gameOverText.setString("GAME OVER!");
    gameOverText.setPosition(window.getSize().x/2-(gameOverText.getGlobalBounds().width/2), window.getSize().y / 2 - (gameOverText.getGlobalBounds().height/2));


    //Player Init
    int score = 0;
    Player player(&playerTex);
    int shootTimer = 20;
    Text hpText;
    hpText.setFont(font);
    hpText.setCharacterSize(12);
    hpText.setFillColor(Color::White);

    //Enemy init
    int enemySpawnTimer = 0;
    vector<Enemy> enemies;
    Text ehpText;
    ehpText.setFont(font);
    ehpText.setCharacterSize(12);
    ehpText.setFillColor(Color::White);
    

    
    //Loop
        while (window.isOpen())
        {
            Event event;
            while (window.pollEvent(event))
            {
                if (event.type == Event::Closed) {
                    window.close();
                }
                if (event.type == Event::KeyPressed && event.key.code == Keyboard::Escape){
                    window.close();
                }

            }
            if (player.HP > 0)
            {
                //Player
                if (Keyboard::isKeyPressed(Keyboard::W))
                    player.shape.move(0.f, -10.f);
                if (Keyboard::isKeyPressed(Keyboard::S))
                    player.shape.move(0.f, 10.f);
                if (Keyboard::isKeyPressed(Keyboard::A))
                    player.shape.move(-10.f, 0);
                if (Keyboard::isKeyPressed(Keyboard::D))
                    player.shape.move(10.f, 0);

                hpText.setPosition(player.shape.getPosition().x, player.shape.getPosition().y - hpText.getGlobalBounds().height);
                hpText.setString(to_string(player.HP) + "/" + to_string(player.HPMax));

                //Collision with Window
                if (player.shape.getPosition().x <= 0)//left
                    player.shape.setPosition(0, player.shape.getPosition().y);
                if (player.shape.getPosition().x > window.getSize().x - player.shape.getGlobalBounds().width)//right
                    player.shape.setPosition(window.getSize().x - player.shape.getGlobalBounds().width, player.shape.getPosition().y);
                if (player.shape.getPosition().y <= 0)//top
                    player.shape.setPosition(player.shape.getPosition().x, 0);
                if (player.shape.getPosition().y > window.getSize().y - player.shape.getGlobalBounds().height)//bottom
                    player.shape.setPosition(player.shape.getPosition().x, window.getSize().y - player.shape.getGlobalBounds().height);

                //Update COntrols

                if (shootTimer < 15)
                    shootTimer++;
                if (Mouse::isButtonPressed(Mouse::Left) && shootTimer >= 15)
                {
                    player.bullets.push_back(Bullet(&bulletTex, player.shape.getPosition()));
                    shootTimer = 0; //Reset timer
                }

                //Bullets
                for (size_t i = 0; i < player.bullets.size(); i++)
                {
                    //move bullets
                    player.bullets[i].shape.move(20.f, 0);

                    //Out of Bound Bullets
                    if (player.bullets[i].shape.getPosition().x > window.getSize().x)
                    {
                        player.bullets.erase(player.bullets.begin() + i);
                        break;
                    }

                    //Enemies Collision
                    for (size_t k = 0; k < enemies.size(); k++)
                    {
                        if (player.bullets[i].shape.getGlobalBounds().intersects(enemies[k].shape.getGlobalBounds()))
                        {
                            if (enemies[k].HP <= 1)
                            {
                                score += enemies[k].HPMax;
                                enemies.erase(enemies.begin() + k);
                            }
                            else
                                enemies[k].HP--; //Enemies take damage

                            player.bullets.erase(player.bullets.begin() + i);
                            break;
                        }
                    }
                }
                //Enemy
                if (enemySpawnTimer < 25)
                    enemySpawnTimer++;
                if (enemySpawnTimer >= 25) {
                    enemies.push_back(Enemy(&enemyTex, window.getSize()));
                    enemySpawnTimer = 0; //reset enemy
                }


                for (size_t i = 0; i < enemies.size(); i++)
                {
                    enemies[i].shape.move(-5.f, 0);
                    if (enemies[i].shape.getPosition().x <= 0 - enemies[i].shape.getGlobalBounds().width)
                    {
                        enemies.erase(enemies.begin() + i);
                        break;
                    }
                    if (enemies[i].shape.getGlobalBounds().intersects(player.shape.getGlobalBounds()))
                    {
                        enemies.erase(enemies.begin() + i);
                        player.HP--; //Player take damage
                        break;
                    }

                }
                //UI Update
                scoreText.setString("Score :" + std::to_string(score));
            }

            //Draw
            window.clear();
            window.draw(player.shape);
            for (size_t i = 0; i < player.bullets.size(); i++)
            {
                window.draw(player.bullets[i].shape);
            }
            for (size_t i = 0; i < enemies.size(); i++)
            {
                ehpText.setString(std::to_string(enemies[i].HP) + "/" + std::to_string(enemies[i].HPMax));
                ehpText.setPosition(enemies[i].shape.getPosition().x, enemies[i].shape.getPosition().y - ehpText.getGlobalBounds().height);
                window.draw(ehpText);
                window.draw(enemies[i].shape);
            }
            //UI
            window.draw(hpText);
            window.draw(scoreText);
            if (player.HP <= 0)
                window.draw(gameOverText);

            //Display
            window.display();
        }
    return 0;
}
