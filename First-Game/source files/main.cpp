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
    PlayingSurvival,
    Paused,
    Finished
};
/// I'll want -> Survival Mode + Difficulties for game modes

/// ---------------------------- GLOBALS ----------------------------------
int gamespace_side;
int left_space;
int up_space;

int scorewidth;
int scoreheight;

RectangleShape GameSpaceRectangle;
RectangleShape ScoreDetailsRectangle;

string words_list[10005];

vector<Text*> text_list;


/// ---------------------------- GLOBALS ----------------------------------



void create_words_list() {

    std::ifstream file("words.txt");
    if (!file.is_open()) {
        cout << "Error opening file";
        return;
    }


    string word;
    int k = 0;
    while (file >> word) {
        // if (word.size() < 4) continue;
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

bool isTextFullyInside(const sf::Text& text, const sf::RectangleShape& rectangle) {
    sf::FloatRect textBounds = text.getGlobalBounds();
    sf::FloatRect rectangleBounds = rectangle.getGlobalBounds();

    return textBounds.left >= rectangleBounds.left &&
        textBounds.top >= rectangleBounds.top &&
        textBounds.left + textBounds.width <= rectangleBounds.left + rectangleBounds.width &&
        textBounds.top + textBounds.height <= rectangleBounds.top + rectangleBounds.height;
}

void generate_and_push_word(Font& font,int font_size,Color color) {

    string rand_word = get_random_word();

    Text* new_text = new Text();
    (*new_text).setFont(font);
    (*new_text).setString(rand_word);
    (*new_text).setCharacterSize(20); /// CHANGE FONT SIZE LATER MAYBE
    (*new_text).setFillColor(Color::White);

    // left_space -> left_space + gamespace_side
    // -> generate number from 0 -> gamespace_side

    int number = left_space + (rand() % gamespace_side);


    (*new_text).setPosition(number, up_space + 1);

    while (!isTextFullyInside((*new_text), GameSpaceRectangle)) {
        number = left_space + (rand() % gamespace_side);
        cout << number << '\n';
        (*new_text).setPosition(number, up_space + 1);
    }


    text_list.push_back(new_text);

}


int main()
{

    /// ----------------------------------- GAME APPEARANCE --------------------------------

    gamespace_side = resolution_height * 70 / 100;
    left_space = (resolution_width - gamespace_side) / 2;
    up_space = (resolution_height * 15 / 100);

    GameSpaceRectangle.setSize(Vector2f(gamespace_side, gamespace_side));
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

    scorewidth = left_space * 70 / 100;
    scoreheight = gamespace_side;

    ScoreDetailsRectangle.setSize(Vector2f(scorewidth, scoreheight));
    ScoreDetailsRectangle.setFillColor(Color::Red);
    ScoreDetailsRectangle.setPosition(Vector2f(left_space * 25 / 100, up_space));

    /// ----------------------------------- GAME APPEARANCE --------------------------------


    /// --------------------------------- GENERATING WORDS -------------------------------------

    create_words_list();

    Font words_font;

    if (!words_font.loadFromFile("assets/Cocogoose-Pro-Bold-trial.ttf")) {
        cout << "Error loading";
        return 0;
    }

    /// --------------------------------- GENERATING WORDS -------------------------------------


    /// ----------------------------------- PAUSED APPEARANCE --------------------------------

    Text PAUSED;
    PAUSED.setFont(words_font);
    PAUSED.setCharacterSize(145);
    PAUSED.setFillColor(Color::White);
    PAUSED.setString("P A U S E D");
    FloatRect pausedBounds = PAUSED.getLocalBounds();
    PAUSED.setOrigin(pausedBounds.left + pausedBounds.width / 2.0f, pausedBounds.top + pausedBounds.height / 2.0f);
    PAUSED.setPosition(resolution_width / 2.0f, resolution_height * 10 / 100);

    int button_width = resolution_width * 30 / 100;
    int button_height = resolution_height * 15 / 100;

    RectangleShape ContinueButton(Vector2f(button_width, button_height));
    RectangleShape RetryButton(Vector2f(button_width, button_height));
    RectangleShape ExitButton(Vector2f(button_width, button_height));

    Text CONTINUE,RETRY,EXIT;
    CONTINUE.setFont(words_font);
    RETRY.setFont(words_font);
    EXIT.setFont(words_font);

    CONTINUE.setCharacterSize(70);
    CONTINUE.setFillColor(Color::White);
    CONTINUE.setString("CONTINUE");
    RETRY.setCharacterSize(70);
    RETRY.setFillColor(Color::White);
    RETRY.setString("RETRY");
    EXIT.setCharacterSize(70);
    EXIT.setFillColor(Color::White);
    EXIT.setString("EXIT");

    ContinueButton.setOutlineThickness(2);
    ContinueButton.setOutlineColor(Color::White);
    ContinueButton.setFillColor(Color::Black);
    ContinueButton.setPosition(resolution_width * 35 / 100, resolution_height * 25 / 100);


    RetryButton.setOutlineThickness(2);
    RetryButton.setOutlineColor(Color::White);
    RetryButton.setFillColor(Color::Black);
    RetryButton.setPosition(resolution_width * 35 / 100, resolution_height * 47 / 100);


    ExitButton.setOutlineThickness(2);
    ExitButton.setOutlineColor(Color::White);
    ExitButton.setFillColor(Color::Black);
    ExitButton.setPosition(resolution_width * 35 / 100, resolution_height * 69 / 100);

    sf::Vector2f ContinueCenter(ContinueButton.getPosition().x + ContinueButton.getSize().x / 2.0f,
                                ContinueButton.getPosition().y + ContinueButton.getSize().y / 2.0f);

    sf::Vector2f RetryCenter(RetryButton.getPosition().x + RetryButton.getSize().x / 2.0f,
                             RetryButton.getPosition().y + RetryButton.getSize().y / 2.0f);

    sf::Vector2f ExitCenter(ExitButton.getPosition().x + ExitButton.getSize().x / 2.0f,
                            ExitButton.getPosition().y + ExitButton.getSize().y / 2.0f);

    sf::FloatRect continueBounds = CONTINUE.getLocalBounds();
    CONTINUE.setOrigin(continueBounds.left + continueBounds.width / 2.0f, continueBounds.top + continueBounds.height / 2.0f);
    sf::FloatRect retryBounds = RETRY.getLocalBounds();
    RETRY.setOrigin(retryBounds.left + retryBounds.width / 2.0f, retryBounds.top + retryBounds.height / 2.0f);
    sf::FloatRect exitBounds = EXIT.getLocalBounds();
    EXIT.setOrigin(exitBounds.left + exitBounds.width / 2.0f, exitBounds.top + exitBounds.height / 2.0f);

    CONTINUE.setPosition(ContinueCenter);
    RETRY.setPosition(RetryCenter);
    EXIT.setPosition(ExitCenter);

    

    /// ----------------------------------- PAUSED APPEARANCE --------------------------------


    AppState currentState = AppState::PlayingSurvival;
    // -> speed = words / minute
    int gameSpeed = 160;

    Clock clock;
    Time elapsedTime;

    RenderWindow window(VideoMode(resolution_width, resolution_height), "Works!");
    string currentInput;

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Escape) {

                    if (currentState == AppState::PlayingSurvival) {
                        currentState = AppState::Paused;
                        continue;
                    }
                    if (currentState == AppState::Paused) {
                        currentState = AppState::PlayingSurvival;
                        continue;
                    }

                }
            }

        }

        window.clear();

        /// ----------------------- DRAW ----------------------------

        if (currentState == AppState::PlayingSurvival) {

            window.draw(GameSpaceRectangle);
            window.draw(ScoreDetailsRectangle);

            Time frameTime = clock.restart();
            elapsedTime += frameTime;


            if (elapsedTime.asSeconds() >= 1.0f) { /// Every second we generate a new word to print

                generate_and_push_word(words_font,20,Color::White);

                elapsedTime = Time::Zero;
            }

            /// Drawing the falling words :

            for (auto it = text_list.begin(); it != text_list.end();) {
                (*it)->move(0, gameSpeed * frameTime.asSeconds());

                // if ((*it)->getPosition().y > up_space + gamespace_side) {
                if ( !isTextFullyInside((**it), GameSpaceRectangle) ) {
                    delete* it;
                    it = text_list.erase(it);
                }
                else {
                    window.draw(**it);
                    ++it;
                }
            }

            text_list.shrink_to_fit();

        }

        if (currentState == AppState::Paused) { /// CAN PAUSE PRESSING ESC

            clock.restart();
            window.draw(ContinueButton);
            window.draw(RetryButton);
            window.draw(ExitButton);
            window.draw(PAUSED);
            window.draw(CONTINUE);
            window.draw(RETRY);
            window.draw(EXIT);
        }

        /// ----------------------- DRAW ----------------------------

        window.display();
    }

    return 0;
}