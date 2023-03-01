#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include <algorithm>
#include <stdlib.h>

const int r = 2; //Multiplier par deux l'affichage
const int largeur = 224 * r, hauteur = 256 * r;
const int rowSize = 8 * r, colSize = 16 * r;
const int rowCount = 16, colCount = 14;
bool alldead = false;

void initGame();
void resetGame(bool gameOver);
void disableShot();
std::string getStructuredScore(int& integer);

struct ennemyShot
{
    int posX = 0, posY = 0, style = 0;
    bool display = false;
}es;

struct ennemy
{
    const int sizeX = 12 * r, sizeY = 8 * r;
    int posX = 0, posY = 0;
    bool dead = false;
    int sprite = 0;
    ennemyShot es;
}e[55];

struct player
{
    const int sizeX = 16 * r, sizeY = 8 * r;
    int posX = 32;
    const int posY = 14 * r * rowSize;
    int life = 3;
    bool dead = false;
}p;

struct shot
{
    const int sizeX = 1, sizeY = 8 * r;
    int posX = 0, posY = 0;
    bool display = false;
}s;

struct blocs
{
    const int sizeX = 16, sizeY = 16;
    int posX = 1 * colSize + 1, posY = 11 * r * rowSize; //posX + 1 pour que le shot touche le bloc
    int life = 3;
}b;

struct guards
{
    blocs b[12]; //sizeX = 32 et sizeY = 48;
}g[4];

struct score
{
    int playerScore = 0;
    int bestScore = 0;
}sc;

int main()
{
    int frame = 0;

    sf::RenderWindow window(sf::VideoMode(largeur, hauteur), "Space Invaders", sf::Style::Titlebar);
    window.setFramerateLimit(60);

    sf::Image icon;
    if (!icon.loadFromFile("res/icon/icon.png"))
        std::cout << "Erreur de chargement de l'icone du jeu" << std::endl;

    window.setIcon(64, 64, icon.getPixelsPtr());

    //Gestion de la font et des textes
    sf::Font font;
    if (!font.loadFromFile("res/fonts/space_invaders.ttf"))
        std::cout << "Erreur lors du chargement de la font" << std::endl;

    sf::Text scoreText, highScoreText, playerScore, highScore;
    scoreText.setFont(font);
    highScoreText.setFont(font);
    playerScore.setFont(font);
    highScore.setFont(font);

    scoreText.setString("SCORE< 1 >");
    scoreText.setCharacterSize(8 * r);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition(sf::Vector2f(colSize, 2 * r));

    highScoreText.setString("HI-SCORE");
    highScoreText.setCharacterSize(8 * r);
    highScoreText.setFillColor(sf::Color::White);
    highScoreText.setPosition(sf::Vector2f(colSize * 5.5, 2 * r));

    playerScore.setString(getStructuredScore(sc.playerScore));
    playerScore.setCharacterSize(8 * r);
    playerScore.setFillColor(sf::Color::White);
    playerScore.setPosition(sf::Vector2f(colSize * 1.75, rowSize * 2));

    highScore.setString(getStructuredScore(sc.bestScore));
    highScore.setCharacterSize(8 * r);
    highScore.setFillColor(sf::Color::White);
    highScore.setPosition(sf::Vector2f(colSize * 6.15, rowSize * 2));

    sf::RectangleShape limitShape(sf::Vector2f(largeur, 1 * r));
    limitShape.setFillColor(sf::Color::White);
    limitShape.setPosition(sf::Vector2f(0, 15 * rowSize * r));

    //******
    //Joueur
    //******
    sf::Texture playerTexture;
    if (!playerTexture.loadFromFile("res/player/playerTexture.png"))
        std::cout << "Erreur du chargement de la texture du joueur" << std::endl;

    sf::Sprite playerSprite;
    playerSprite.setTexture(playerTexture);
    playerSprite.setTextureRect(sf::IntRect(0, 0, 32, 16));

    sf::Sprite playerLives;
    playerLives.setTexture(playerTexture);
    playerLives.setTextureRect(sf::IntRect(0, 0, 32, 16));

    //***
    //Tir
    //***
    sf::Texture shotTexture;
    if (!shotTexture.loadFromFile("res/player/shot.png"))
        std::cout << "Erreur lors du chargement de la texture du tir" << std::endl;

    sf::Sprite shotSprite;
    shotSprite.setTexture(shotTexture);
    shotSprite.setTextureRect(sf::IntRect(0, 0, 2, 16));

    //*******
    //Ennemis
    //*******
    bool go_down = false, go_right = true;
    int killed = 0, newKilled = 0;

    sf::Texture ennemiesTexture;
    if (!ennemiesTexture.loadFromFile("res/ennemies/ennemies.png"))
        std::cout << "Erreur lors du chargement de la texture des ennemis" << std::endl;

    sf::Sprite ennemiesSprite;
    ennemiesSprite.setTexture(ennemiesTexture);

    sf::RectangleShape esShape(sf::Vector2f(2, 16));
    esShape.setFillColor(sf::Color::White);

    //*******************
    //Blocs de protection
    //*******************
    sf::Texture bloksTexture;
    if (!bloksTexture.loadFromFile("res/guards/guards.png"))
        std::cout << "Erreur lors du chargement de la texture des protections" << std::endl;

    sf::Sprite bloksSprite;
    bloksSprite.setTexture(bloksTexture);
    bloksSprite.setTextureRect(sf::IntRect(0, 64, 16, 16));
    bloksSprite.setPosition(b.posX, b.posY);

    //*********************
    //Initialisation du jeu
    //*********************
    initGame();

    //*************************
    //Début de la boucle du jeu
    //*************************
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::KeyReleased)
                if (event.key.code == sf::Keyboard::Escape)
                    window.close();
        }

        //if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) && p.posX + p.sizeX < largeur - (largeur / colCount) && p.life > 0)
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) && (p.posX + p.sizeX) / colSize < colCount && p.life > 0)
        {
            p.posX += 1 * r;
            std::cout << "Right - posX = " << p.posX << std::endl;
        }
        //if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && p.posX > largeur / colCount)
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && p.posX > 1)
        {
            p.posX -= 1 * r;
            std::cout << "Left - posX = " << p.posX << std::endl;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && !s.display && p.life > 0)
        {
            s.display = true;

            s.posX = p.posX + p.sizeX / 2;
            s.posY = p.posY;

            shotSprite.setPosition(sf::Vector2f(s.posX, s.posY));
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::R))
        {
            resetGame(true);
            highScore.setString(getStructuredScore(sc.bestScore));
            playerScore.setString(getStructuredScore(sc.playerScore));
        }
        /*if (sf::Keyboard::isKeyPressed(sf::Keyboard::K))
        {
            //p.life = 0;
            //for (int i = 0; i < 54; i++)
                //e[i].dead = true;
        }*/

        playerSprite.setPosition(p.posX, p.posY);

        window.clear();

        window.draw(scoreText);
        window.draw(highScoreText);
        window.draw(playerScore);
        window.draw(highScore);
        window.draw(limitShape);

        //Si le joueur a encore au moins une vie
        if (!p.dead && p.life > 0)
        {
            if (alldead)
            {
                resetGame(false);
                go_down = false;
                go_right = true;
            }

            alldead = true;

            for (int l = 0; l < p.life - 1; l++)
            {
                playerLives.setPosition(sf::Vector2f(l * colSize, 15 * rowSize * r + 8));
                window.draw(playerLives);
            }

            for (int a = 0; a < 4; a++)
            {
                for (int i = 0; i < 12; i++)
                {
                    if (g[a].b[i].life > 0 && s.posX > g[a].b[i].posX && s.posX < g[a].b[i].posX + g[a].b[i].sizeX && s.posY > g[a].b[i].posY && s.posY < g[a].b[i].posY + g[a].b[i].sizeY)
                    {
                        g[a].b[i].life -= 1;
                        disableShot();
                    }

                    if (g[a].b[i].life > 0)
                    {
                        bloksSprite.setPosition(g[a].b[i].posX, g[a].b[i].posY);

                        if (i == 0)
                            bloksSprite.setTextureRect(sf::IntRect((g[a].b[i].life - 1) * (colSize / 2), 0, 16, 16));
                        else if (i == 3)
                            bloksSprite.setTextureRect(sf::IntRect((g[a].b[i].life - 1) * (colSize / 2), 16, 16, 16));
                        else if (i == 9)
                            bloksSprite.setTextureRect(sf::IntRect((g[a].b[i].life - 1) * (colSize / 2), 32, 16, 16));
                        else if (i == 10)
                            bloksSprite.setTextureRect(sf::IntRect((g[a].b[i].life - 1) * (colSize / 2), 48, 16, 16));
                        else
                            bloksSprite.setTextureRect(sf::IntRect((g[a].b[i].life - 1) * (colSize / 2), 64, 16, 16));

                        window.draw(bloksSprite);
                    }
                }
            }

            for (int i = 0; i < 55; i++)
            {
                if (!e[i].dead)
                {
                    alldead = false;

                    if (s.posX > e[i].posX && s.posX < e[i].posX + e[i].sizeX && s.posY > e[i].posY && s.posY < e[i].posY + e[i].sizeY) //Check des collisions avec les tirs du joueur
                    {
                        e[i].dead = true;
                        switch (e[i].sprite)
                        {
                            case 0:
                                sc.playerScore += 30;
                                break;
                            case 2:
                                sc.playerScore += 20;
                                break;
                            case 4:
                                sc.playerScore += 10;
                                break;
                            default:
                                break;
                        }

                        playerScore.setString(getStructuredScore(sc.playerScore));

                        disableShot();
                    }

                    if (e[i].posX < 2 && !go_right) //if (e[i].posX / colSize < 1 && !go_right)
                    {
                        if (e[i].posY < 10 * r * rowSize && !go_down)
                        {
                            go_down = true;
                        }
                        go_right = true;
                    }
                    else if ((e[i].posX + e[i].sizeX) / colSize > colCount - 1) //else if ((e[i].posX + e[i].sizeX) / colSize > colCount - 2)
                    {
                        if (go_right && e[i].posY < 10 * r * rowSize && !go_down)
                        {
                            go_down = true;
                        }
                        go_right = false;
                    }

                    if (e[i].es.display)
                    {
                        esShape.setPosition(e[i].es.posX + (e[i].sizeX / 2), e[i].es.posY);
                        window.draw(esShape);
                        e[i].es.posY += 1 * r;
                        if (e[i].es.posX + 16 > p.posX && e[i].es.posX + 16 < (p.posX + p.sizeX) && e[i].es.posY > p.posY && e[i].es.posY < (p.posY + p.sizeY))
                        {
                            std::cout << e[i].es.posX << " - " << p.posX << std::endl;
                            std::cout << "Touche joueur" << std::endl; //Faire une fonction pour tuer le joueur, enlever une vie et repositionner le vaisseau
                            p.life -= 1;
                            e[i].es.display = false;
                        }

                        for (int a = 0; a < 4; a++)
                        {
                            for (int y = 0; y < 12; y++)
                            {
                                if (g[a].b[y].life > 0 && e[i].es.posX + (e[i].sizeX / 2) > g[a].b[y].posX && e[i].es.posX + (e[i].sizeX / 2) < (g[a].b[y].posX + g[a].b[y].sizeX) && e[i].es.posY > g[a].b[y].posY && e[i].es.posY < (g[a].b[y].posY + g[a].b[y].sizeY))
                                {
                                    g[a].b[y].life -= 1;
                                    std::cout << "Touche bouclier" << std::endl;
                                    e[i].es.display = false;
                                }
                            }
                        }

                        if (e[i].es.posY > hauteur - (3 * rowSize))
                            e[i].es.display = false;
                    }
                    else
                    {
                        e[i].es.posX = e[i].posX;
                        e[i].es.posY = e[i].posY;
                    }

                    if (frame % 30 == 0 &&(rand() % 2048) % 192 == 0 && !e[i].es.display)
                    {
                        std::cout << "Tir" << std::endl;

                        e[i].es.display = true;
                        e[i].es.posX = e[i].posX;
                        e[i].es.posY = e[i].posY;
                    }

                    if (frame % (28 - (killed / 2)) == 0)
                    {
                        if (go_down)
                            e[i].posY += rowSize;
                        else if (go_right)
                            e[i].posX += 1 * r;
                        else
                            e[i].posX -= 1 * r;
                    }

                    ennemiesSprite.setTextureRect(sf::IntRect(e[i].sizeX * (e[i].sprite + (e[i].posX / 2 % 2)), 0, e[i].sizeX, e[i].sizeY));
                    ennemiesSprite.setPosition(e[i].posX, e[i].posY);

                    if (e[i].dead)
                        ennemiesSprite.setTextureRect(sf::IntRect(144, 0, e[i].sizeX, e[i].sizeY));

                    window.draw(ennemiesSprite);
                }
                else
                {
                    newKilled += 1;
                }
            }

            if (s.display)
            {
                if (s.posY > rowSize * 3)
                {
                    s.posY -= 4 * r;
                    shotSprite.setPosition(sf::Vector2f(s.posX, s.posY));
                    window.draw(shotSprite);
                }
                else
                {
                    disableShot();
                }
            }

            window.draw(playerSprite);
        }
        else
        {
            p.dead = true;

            if (frame == 2)
            {
                playerSprite.setTextureRect(sf::IntRect(32, 0, 32, 16));
            }
            else if (frame == 30)
            {
                playerSprite.setTextureRect(sf::IntRect(64, 0, 32, 16));
            }

            if (frame == 60)
                p.life += 1;

            if (p.life == 3)
            {
                p.dead = false;
                playerSprite.setTextureRect(sf::IntRect(0, 0, 32 ,16));
                resetGame(true);
                go_right = true;
                go_down = false;
            }

            window.draw(playerSprite);
        }

        window.display();

        if (frame % (28 - (killed / 2)) == 0)
            go_down = false;

        if (frame == 60) //Utiliser les frames pour le déplacement !!
        {
            frame = 0;
        }
        frame++;
        killed = newKilled;
        newKilled = 0;
    }

    return 0;
}

void disableShot()
{
    s.display = false;
    s.posX = p.posX;
    s.posY = p.posY;
}

void initGame()
{
    p.posX = colSize; //Placement du joueur au début de la deuxième colonne

    for (int a = 0; a < 4; a++) //Placement des boucliers
    {
        for (int i = 0; i < 12; i++)
        {
            if (a == 0)
                g[a].b[i].posX += (colSize / 2) * (i % 4) + colSize * (a * 2.5);
            else if (a == 1)
                g[a].b[i].posX += (colSize / 2) * (i % 4) + colSize * (a * 3);
            else if (a == 2)
                g[a].b[i].posX += (colSize / 2) * (i % 4) + colSize * (a * 3.25);
            else if (a == 3)
                g[a].b[i].posX += (colSize / 2) * (i % 4) + colSize * (a * 3.25);

            g[a].b[i].posY += rowSize * (i / 4);
        }
    }

    for (int i = 0; i < 55; i++) //Placement initial des ennemis
    {
        e[i].posX = colSize * (i % 11);
        e[i].posY = rowSize * (5 + (i / 11 * r));
        if (i / 11 == 0)
            e[i].sprite = 0;
        else if (i / 11 == 1 || i / 11 == 2)
            e[i].sprite = 2;
        else
            e[i].sprite = 4;
    }
}

void resetGame(bool gameOver) //En cas de game over
{
    p.posX = colSize; //Placement du joueur au début de la deuxième colonne

    for (int a = 0; a < 4; a++) //Placement des boucliers
    {
        for (int i = 0; i < 12; i++)
        {
            g[a].b[i].life = 3;
        }
    }

    for (int i = 0; i < 55; i++) //Placement initial des ennemis
    {
        e[i].posX = colSize * (i % 11);
        e[i].posY = rowSize * (5 + (i / 11 * r));

        if (i / 11 == 0)
            e[i].sprite = 0;
        else if (i / 11 == 1 || i / 11 == 2)
            e[i].sprite = 2;
        else
            e[i].sprite = 4;

        e[i].es.display = false;

        e[i].dead = false;
    }

    if (gameOver)
    {
        if (sc.playerScore > sc.bestScore)
            sc.bestScore = sc.playerScore;

        sc.playerScore = 0;
    }


}

std::string getStructuredScore(int& integer)
{
    std::string value = std::to_string(integer);

    int sze = value.size();

    int precision = 4 - std::min(4, sze);
    std::string s = std::string(precision, '0').append(value);

    return s;
}
