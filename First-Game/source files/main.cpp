#include <iostream>
#include <fstream>
#include <string.h>
#include <random>
#include <SFML/Graphics.hpp>

using namespace std;
using namespace sf;

int resolution_width = 1920;
int resolution_height = 1080;

enum class AppState {
    Menu,
    Playing,
    Paused,
    Finished
};

string words_list[10005];

void create_words_list() {

    std::ifstream file("words.txt");
    if (!file.is_open()) {
        cout << "Error opening file";
        return;
    }


    string word;
    int k = 0;
    while (file >> word) {
        if (word.size() < 5) continue;
        words_list[++k] = word;
    }

    file.close();
}

string get_random_word() {

    int totalWords = 10000;
    
    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_int_distribution<int> distribution(1, totalWords);

    int randomNumber = distribution(gen);

    return words_list[randomNumber];

}

int main()
{
    RenderWindow window(VideoMode(resolution_width, resolution_height), "Works!");

    /// ----------------------------------- APPEARANCE --------------------------------

    int gamespace_side = resolution_height * 70 / 100;
    int left_space = (resolution_width - gamespace_side) / 2;
    int up_space = (resolution_height * 15 / 100);

    RectangleShape GameSpaceRectangle(Vector2f(gamespace_side, gamespace_side));
    GameSpaceRectangle.setFillColor(Color::White);
    GameSpaceRectangle.setPosition(Vector2f(left_space, up_space));

    Texture backgroundTexture;
    if (!backgroundTexture.loadFromFile("assets/galaxy.jpg")) {
        cout << "Error loading";
        return 0;
    }
    IntRect imageRect(0, 0, 3.5*gamespace_side, 3.5*gamespace_side);

    GameSpaceRectangle.setTexture(&backgroundTexture);
    GameSpaceRectangle.setTextureRect(imageRect);

    int scorewidth = left_space * 70 / 100;
    int scoreheight = gamespace_side;

    RectangleShape ScoreDetailsRectangle(Vector2f(scorewidth, scoreheight));
    ScoreDetailsRectangle.setFillColor(Color::Red);
    ScoreDetailsRectangle.setPosition(Vector2f(left_space * 25 / 100, up_space));

    /// ----------------------------------- APPEARANCE --------------------------------


    /// --------------------------------- GENERATING WORDS -------------------------------------

    create_words_list();

    Font words_font;
    if (!words_font.loadFromFile("assets/Cocogoose-Pro-Bold-trial.ttf")) {
        cout << "Error loading";
        return 0;
    }





    /// --------------------------------- GENERATING WORDS -------------------------------------

    AppState currentState = AppState::Playing;
    // -> speed = words / minute
    int gameSpeed = 60;

    Clock clock;
    Time elapsedTime;
    vector<Text*> text_list;


    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();

        /// ----------------------- DRAW ----------------------------

        if (currentState == AppState::Playing) {
            window.draw(GameSpaceRectangle);
            window.draw(ScoreDetailsRectangle);

            Time frameTime = clock.restart();
            elapsedTime += frameTime;


            if (elapsedTime.asSeconds() >= 1.0f) {

                string rand_word = get_random_word();

                Text *new_text = new Text();
                (*new_text).setFont(words_font);
                (*new_text).setString(rand_word);
                (*new_text).setCharacterSize(20);
                (*new_text).setPosition(left_space, up_space); // Set the position of the text
                (*new_text).setFillColor(Color::White); // Set the fill color

                text_list.push_back(new_text);

                elapsedTime = Time::Zero;
            }

            
            for (auto it : text_list) {
                (*it).move(0, gameSpeed * frameTime.asSeconds());
                window.draw(*it);
            }

        }


        /// ----------------------- DRAW ----------------------------

        window.display();
    }

    return 0;
}