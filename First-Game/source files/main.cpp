#include <iostream>
#include <fstream>
#include <string.h>
#include <random>
#include <thread>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <SFML/Graphics.hpp>

using namespace std;
using namespace sf;

int resolution_width = 1920;
int resolution_height = 1080;

enum class AppState {
    Menu,
    PlayingSurvival,
    PlayingEasy,
    PlayingMedium,
    PlayingHard,
    PlayingExpert,
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

void generate_and_push_word(Font& font, int font_size, Color color) {

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

void centerTextInRectangle(Text& text, RectangleShape rectangle) {
    sf::Vector2f textCenter(rectangle.getPosition().x + rectangle.getSize().x / 2.0f,
        rectangle.getPosition().y + rectangle.getSize().y / 2.0f);
    sf::FloatRect textBounds = text.getLocalBounds();
    text.setOrigin(textBounds.left + textBounds.width / 2.0f, textBounds.top + textBounds.height / 2.0f);
    text.setPosition(textCenter);
}

void set_SizeColorString_of_Text(Text& text, int fontsize, Color color, string str) {
    text.setCharacterSize(fontsize);
    text.setFillColor(color);
    text.setString(str);
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
    IntRect imageRect(0, 0, 3.5 * gamespace_side, 3.5 * gamespace_side);

    GameSpaceRectangle.setTexture(&backgroundTexture);
    GameSpaceRectangle.setTextureRect(imageRect);

    scorewidth = left_space * 70 / 100;
    scoreheight = gamespace_side;

    ScoreDetailsRectangle.setSize(Vector2f(scorewidth, scoreheight));
    ScoreDetailsRectangle.setFillColor(Color::Black);
    ScoreDetailsRectangle.setOutlineColor(Color::White);
    ScoreDetailsRectangle.setOutlineThickness(2);
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
    PAUSED.setCharacterSize(150);
    PAUSED.setFillColor(Color::White);
    PAUSED.setString("P A U S E D");
    FloatRect pausedBounds = PAUSED.getLocalBounds();
    PAUSED.setOrigin(pausedBounds.left + pausedBounds.width / 2.0f, pausedBounds.top + pausedBounds.height / 2.0f);
    PAUSED.setPosition(resolution_width / 2.0f, resolution_height * 10 / 100);

    int paused_button_width = resolution_width * 30 / 100;
    int paused_button_height = resolution_height * 15 / 100;

    RectangleShape ContinueButton(Vector2f(paused_button_width, paused_button_height));
    RectangleShape RetryButton(Vector2f(paused_button_width, paused_button_height));
    RectangleShape ExitButton(Vector2f(paused_button_width, paused_button_height));

    Text CONTINUE, RETRY, EXIT;
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

    /// I'm aware it could have been done in an easier manner;

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


    /// ----------------------------------- MENU APPEARANCE --------------------------------

    Text WELCOME;
    WELCOME.setFont(words_font);
    WELCOME.setCharacterSize(150);
    WELCOME.setFillColor(Color::White);
    WELCOME.setString("WELCOME");
    FloatRect welcomeBounds = WELCOME.getLocalBounds();
    WELCOME.setOrigin(welcomeBounds.left + welcomeBounds.width / 2.0f, welcomeBounds.top + welcomeBounds.height / 2.0f);
    WELCOME.setPosition(resolution_width / 2.0f, resolution_height * 10 / 100);

    int menu_button_width = resolution_width * 25 / 100;
    int menu_button_height = resolution_height * 10 / 100;

    RectangleShape MenuButtonShape(Vector2f(menu_button_width, menu_button_height));
    MenuButtonShape.setFillColor(Color::Black);
    MenuButtonShape.setOutlineColor(Color::White);
    MenuButtonShape.setOutlineThickness(2);


    RectangleShape SurvivalButton = MenuButtonShape;
    RectangleShape EasyButton = MenuButtonShape;
    RectangleShape MediumButton = MenuButtonShape;
    RectangleShape HardButton = MenuButtonShape;
    RectangleShape ExpertButton = MenuButtonShape;

    SurvivalButton.setPosition(resolution_width * 5 / 100, resolution_height * 25 / 100);
    EasyButton.setPosition(resolution_width * 5 / 100, resolution_height * 40 / 100);
    MediumButton.setPosition(resolution_width * 5 / 100, resolution_height * 55 / 100);
    HardButton.setPosition(resolution_width * 5 / 100, resolution_height * 70 / 100);
    ExpertButton.setPosition(resolution_width * 5 / 100, resolution_height * 85 / 100);

    sf::Color orangeColor(255, 165, 0);

    SurvivalButton.setOutlineColor(Color::Cyan);
    EasyButton.setOutlineColor(Color::Green);
    MediumButton.setOutlineColor(Color::Yellow);
    HardButton.setOutlineColor(orangeColor);
    ExpertButton.setOutlineColor(Color::Red);

    Text GameModesText, SURVIVAL, EASY, MEDIUM, HARD, EXPERT;

    SURVIVAL.setFont(words_font);
    set_SizeColorString_of_Text(SURVIVAL, 50, Color::Cyan, "SURVIVAL");
    centerTextInRectangle(SURVIVAL, SurvivalButton);

    EASY.setFont(words_font);
    set_SizeColorString_of_Text(EASY, 50, Color::Green, "EASY");
    centerTextInRectangle(EASY, EasyButton);

    MEDIUM.setFont(words_font);
    set_SizeColorString_of_Text(MEDIUM, 50, Color::Yellow, "MEDIUM");
    centerTextInRectangle(MEDIUM, MediumButton);

    HARD.setFont(words_font);
    set_SizeColorString_of_Text(HARD, 50, orangeColor, "HARD");
    centerTextInRectangle(HARD, HardButton);

    EXPERT.setFont(words_font);
    set_SizeColorString_of_Text(EXPERT, 50, Color::Red, "EXPERT");
    centerTextInRectangle(EXPERT, ExpertButton);


    Text HighScoreSurvival, HighScoreEasy, HighScoreMedium, HighScoreHard, HighScoreExpert;
    HighScoreSurvival.setFont(words_font);
    set_SizeColorString_of_Text(HighScoreSurvival, 50, Color::White, "-> HIGH SCORE:");
    HighScoreEasy = HighScoreSurvival;
    HighScoreMedium = HighScoreSurvival;
    HighScoreHard = HighScoreSurvival;
    HighScoreExpert = HighScoreSurvival;


    sf::FloatRect highscoreBounds = HighScoreSurvival.getLocalBounds();
    HighScoreSurvival.setOrigin(highscoreBounds.left + highscoreBounds.width / 2.0f, highscoreBounds.top + highscoreBounds.height / 2.0f);
    HighScoreEasy.setOrigin(highscoreBounds.left + highscoreBounds.width / 2.0f, highscoreBounds.top + highscoreBounds.height / 2.0f);
    HighScoreMedium.setOrigin(highscoreBounds.left + highscoreBounds.width / 2.0f, highscoreBounds.top + highscoreBounds.height / 2.0f);
    HighScoreHard.setOrigin(highscoreBounds.left + highscoreBounds.width / 2.0f, highscoreBounds.top + highscoreBounds.height / 2.0f);
    HighScoreExpert.setOrigin(highscoreBounds.left + highscoreBounds.width / 2.0f, highscoreBounds.top + highscoreBounds.height / 2.0f);

    HighScoreSurvival.setPosition(SURVIVAL.getPosition().x + resolution_width * 25 / 100, SURVIVAL.getPosition().y);
    HighScoreEasy.setPosition(EASY.getPosition().x + resolution_width * 25 / 100, EASY.getPosition().y);
    HighScoreMedium.setPosition(MEDIUM.getPosition().x + resolution_width * 25 / 100, MEDIUM.getPosition().y);
    HighScoreHard.setPosition(HARD.getPosition().x + resolution_width * 25 / 100, HARD.getPosition().y);
    HighScoreExpert.setPosition(EXPERT.getPosition().x + resolution_width * 25 / 100, EXPERT.getPosition().y);


    /// ----------------------------------- MENU APPEARANCE --------------------------------

    /// ---------------------------------- SCOREBOARD INFO ----------------------------


    int gameSpeed = 160;
    int currentGameScore = 0;
    int currentGameSpeed = 0;
    float currentGameTimeSpent = 0;
    float totalGameTimeSpent = 0;
    float gameSpawnRate = 1.0;
    float finalGameTime = 0;
    int currentGameLifes = 3;

    Clock gameClock;
    Clock clock;
    Time elapsedTime;
    Time secondCountertime;
    Time gameClockPauseTime;

    Text secondsPassedText;
    Font numbersFont;
    if (!numbersFont.loadFromFile("assets/Nexa-Heavy.ttf")) {
        cout << "Erorr loading numbers font!";
        return 0;
    }
    secondsPassedText.setFont(numbersFont);
    // secondsPassedText.setPosition(0, 0);
    secondsPassedText.setFillColor(Color::White);
    secondsPassedText.setString("0.00s");
    secondsPassedText.setCharacterSize(35);

    centerTextInRectangle(secondsPassedText, ScoreDetailsRectangle);
    secondsPassedText.setPosition(secondsPassedText.getPosition().x, ScoreDetailsRectangle.getPosition().y + ScoreDetailsRectangle.getSize().y * 15 / 100);

    /// ---------------------------------- SCOREBOARD INFO ----------------------------

    /// ---------------------------------- SCOREBOARD APPEARANCE ----------------------------

    Text SCOREBOARD;
    SCOREBOARD.setFont(words_font);
    SCOREBOARD.setCharacterSize(35);
    SCOREBOARD.setFillColor(Color::White);
    SCOREBOARD.setString("SCOREBOARD");
    centerTextInRectangle(SCOREBOARD, ScoreDetailsRectangle);
    SCOREBOARD.setPosition(SCOREBOARD.getPosition().x, ScoreDetailsRectangle.getPosition().y + ScoreDetailsRectangle.getSize().y * 5 / 100);

    Text WordsPerMinute = SCOREBOARD;
    WordsPerMinute.setString("WORDS PER MINUTE");
    centerTextInRectangle(WordsPerMinute, ScoreDetailsRectangle);
    WordsPerMinute.setPosition(WordsPerMinute.getPosition().x, ScoreDetailsRectangle.getPosition().y + ScoreDetailsRectangle.getSize().y * 25 / 100);

    Text ACCURACY = SCOREBOARD;
    ACCURACY.setString("ACCURACY");
    centerTextInRectangle(ACCURACY, ScoreDetailsRectangle);
    ACCURACY.setPosition(ACCURACY.getPosition().x, ScoreDetailsRectangle.getPosition().y + ScoreDetailsRectangle.getSize().y * 45 / 100);

    Text RANK = SCOREBOARD;
    RANK.setString("RANK");
    centerTextInRectangle(RANK, ScoreDetailsRectangle);
    RANK.setPosition(RANK.getPosition().x, ScoreDetailsRectangle.getPosition().y + ScoreDetailsRectangle.getSize().y * 65 / 100);

    Text WPM;
    WPM.setFont(numbersFont);
    WPM.setCharacterSize(35);
    WPM.setFillColor(Color::White);
    WPM.setString("0 WPM");
    centerTextInRectangle(WPM, ScoreDetailsRectangle);
    WPM.setPosition(WPM.getPosition().x, ScoreDetailsRectangle.getPosition().y + ScoreDetailsRectangle.getSize().y * 35 / 100);


    /// ---------------------------------- SCOREBOARD APPEARANCE ----------------------------

    /// ----------------------------------- HEARTS -------------------------------------
    
    int heartSize = 50;

    // RectangleShape heart1(Vector2f(heartSize, heartSize));
    
    Texture heartTexture;
    heartTexture.loadFromFile("assets/heart2.png");
    Sprite heart1(heartTexture);
    Sprite heart2(heartTexture);
    Sprite heart3(heartTexture);

    heart1.setScale(2, 2);
    heart2.setScale(2, 2);
    heart3.setScale(2, 2);


    // up_space
    int heart1xPos = GameSpaceRectangle.getPosition().x + GameSpaceRectangle.getSize().x;
    int heart2xPos = heart1xPos + heartTexture.getSize().x;
    int heart3xPos = heart2xPos + heartTexture.getSize().x;
    heart1.setPosition(heart1xPos, up_space);
    heart2.setPosition(heart2xPos, up_space);
    heart3.setPosition(heart3xPos, up_space);

    Texture deadHeartTexture;
    deadHeartTexture.loadFromFile("assets/deadHeart.png");
    Sprite deadHeart1(deadHeartTexture);
    Sprite deadHeart2(deadHeartTexture);
    Sprite deadHeart3(deadHeartTexture);

    deadHeart1.setScale(2, 2);
    deadHeart2.setScale(2, 2);
    deadHeart3.setScale(2, 2);

    deadHeart1.setPosition(heart1.getPosition().x, heart1.getPosition().y);
    deadHeart2.setPosition(heart2.getPosition().x, heart2.getPosition().y);
    deadHeart3.setPosition(heart3.getPosition().x, heart3.getPosition().y);

    /// ----------------------------------- HEARTS -------------------------------------

    AppState currentState = AppState::Menu;
    AppState lastState = AppState::Menu;
    // -> speed = words / minute


    RenderWindow window(VideoMode(resolution_width, resolution_height), "Falling Words");
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
                        lastState = AppState::PlayingSurvival;
                        continue;
                    }
                    if (currentState == AppState::PlayingEasy) {
                        currentState = AppState::Paused;
                        lastState = AppState::PlayingEasy;
                        continue;
                    }
                    if (currentState == AppState::PlayingMedium) {
                        currentState = AppState::Paused;
                        lastState = AppState::PlayingMedium;
                        continue;
                    }
                    if (currentState == AppState::PlayingHard) {
                        currentState = AppState::Paused;
                        lastState = AppState::PlayingHard;
                        continue;
                    }
                    if (currentState == AppState::PlayingExpert) {
                        currentState = AppState::Paused;
                        lastState = AppState::PlayingExpert;
                        continue;
                    }
                    if (currentState == AppState::Paused) {
                        currentState = lastState;
                        continue;
                    }
                    if (currentState == AppState::Finished) {
                        currentState = AppState::Menu;
                        continue;
                    }
                }
            }

            if (event.type == sf::Event::MouseButtonPressed and event.mouseButton.button == sf::Mouse::Left) {

                if (currentState == AppState::Paused) {

                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                    sf::FloatRect continueBounds = ContinueButton.getGlobalBounds();
                    sf::FloatRect retryBounds = RetryButton.getGlobalBounds();
                    sf::FloatRect exitBounds = ExitButton.getGlobalBounds();

                    if (continueBounds.contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)))
                    {
                        currentState = lastState;
                        continue;
                    }
                    if (retryBounds.contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)))
                    {
                        for (auto it = text_list.begin(); it != text_list.end();) {
                            delete* it;
                            it = text_list.erase(it);
                        }
                        text_list.clear();
                        currentState = lastState; /// return to the last game mode You've been playing !
                        currentGameTimeSpent = 0;
                        totalGameTimeSpent = 0;
                        gameSpawnRate = 1.0;
                        currentGameLifes = 3;
                        continue;
                    }
                    if (exitBounds.contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)))
                    {
                        for (auto it = text_list.begin(); it != text_list.end();) {
                            delete* it;
                            it = text_list.erase(it);
                        }
                        text_list.clear();
                        currentState = AppState::Menu;
                        continue;
                    }

                    continue; /// !!! CAREFUL ON THIS 
                }

                if (currentState == AppState::Menu) {

                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                    sf::FloatRect survivalBounds = SurvivalButton.getGlobalBounds();
                    sf::FloatRect easyBounds = EasyButton.getGlobalBounds();
                    sf::FloatRect mediumBounds = MediumButton.getGlobalBounds();
                    sf::FloatRect hardBounds = HardButton.getGlobalBounds();
                    sf::FloatRect expertBounds = ExpertButton.getGlobalBounds();

                    if (survivalBounds.contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                        currentState = AppState::PlayingSurvival;
                        gameClock.restart();
                        continue;
                    }
                    if (easyBounds.contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                        currentState = AppState::PlayingEasy;
                        continue;
                    }
                    if (mediumBounds.contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                        currentState = AppState::PlayingMedium;
                        continue;
                    }
                    if (hardBounds.contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                        currentState = AppState::PlayingHard;
                        continue;
                    }
                    if (expertBounds.contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                        currentState = AppState::PlayingExpert;
                        continue;
                    }
                }

            }

            /// managing letters typed;


        }

        window.clear();

        /// ----------------------- DRAW ----------------------------

        
         /// scoreboard 

        if (currentState == AppState::PlayingSurvival or currentState == AppState::PlayingEasy
            or currentState == AppState::PlayingMedium or currentState == AppState::PlayingHard
            or currentState == AppState::PlayingExpert) {

            /// I want to display the scoreboard with it's stats:

            window.draw(GameSpaceRectangle);
            window.draw(ScoreDetailsRectangle);

            window.draw(SCOREBOARD);
            window.draw(WordsPerMinute);
            window.draw(ACCURACY);
            window.draw(RANK);

            /// the red rectangle is the location of all the information
            /// ScoreDetailsRectangle
        }

        /// scoreboard


        if (currentState == AppState::PlayingSurvival) {

            window.draw(secondsPassedText);
            window.draw(WPM);

            if (currentGameLifes == 3) {
                window.draw(heart1);
                window.draw(heart2);
                window.draw(heart3);
            }
            if (currentGameLifes == 2) {
                window.draw(heart1);
                window.draw(heart2);
                window.draw(deadHeart3);
            }
            if (currentGameLifes == 1) {
                window.draw(heart1);
                window.draw(deadHeart2);
                window.draw(deadHeart3);
            }
            if (currentGameLifes == 0) {
                window.draw(deadHeart1);
                window.draw(deadHeart2);
                window.draw(deadHeart3);
            }

            Time frameTime = clock.restart();
            elapsedTime += frameTime;
            secondCountertime += frameTime;

            Time elapsed = gameClock.getElapsedTime();
            float seconds = elapsed.asSeconds();

            currentGameTimeSpent = seconds;

            std::ostringstream ss;
            ss << std::fixed << std::setprecision(2) << totalGameTimeSpent + currentGameTimeSpent;

            secondsPassedText.setString(ss.str() + "s");

            // --------------------------------------------

            int wpm_nr = 60.0 / gameSpawnRate;
            WPM.setString(to_string(wpm_nr) + " WPM");


            if (secondCountertime.asSeconds() >= 1.0) {

                /// -0.005 is the good time;
                gameSpawnRate -= 0.005;
                secondCountertime = Time::Zero;
            }

            if (elapsedTime.asSeconds() >= gameSpawnRate) { /// Every second we generate a new word to print

                generate_and_push_word(words_font, 20, Color::White);

                elapsedTime = Time::Zero;
            }

            /// Drawing the falling words :

            for (auto it = text_list.begin(); it != text_list.end();) {
                (*it)->move(0, gameSpeed * frameTime.asSeconds());

                // if ((*it)->getPosition().y > up_space + gamespace_side) {
                if (!isTextFullyInside((**it), GameSpaceRectangle)) {
                    delete* it;
                    it = text_list.erase(it);
                    currentGameLifes--;
                }
                else {
                    window.draw(**it);
                    ++it;
                }
            }

            text_list.shrink_to_fit();

            if (currentGameLifes <= 0) {
                finalGameTime = totalGameTimeSpent + currentGameTimeSpent;
                currentState = AppState::Finished;
                lastState = AppState::PlayingSurvival;
            }

        }

       

        if (currentState == AppState::Paused) { /// CAN PAUSE PRESSING ESC

            totalGameTimeSpent += currentGameTimeSpent;
            currentGameTimeSpent = 0;
            clock.restart();
            gameClock.restart();
            window.draw(ContinueButton);
            window.draw(RetryButton);
            window.draw(ExitButton);
            window.draw(PAUSED);
            window.draw(CONTINUE);
            window.draw(RETRY);
            window.draw(EXIT);
        } 

        if (currentState == AppState::Menu) {

            totalGameTimeSpent = 0;
            gameSpawnRate = 1.0;
            currentGameLifes = 3;


            gameClock.restart();
            // gameClockPauseTime = gameClock.getElapsedTime();
            clock.restart();

            currentGameScore = 0;
            currentGameSpeed = 0;
            currentGameTimeSpent = 0;

            window.draw(WELCOME);
            window.draw(SurvivalButton);
            window.draw(EasyButton);
            window.draw(MediumButton);
            window.draw(HardButton);
            window.draw(ExpertButton);

            window.draw(SURVIVAL);
            window.draw(EASY);
            window.draw(MEDIUM);
            window.draw(HARD);
            window.draw(EXPERT);

            window.draw(HighScoreSurvival);
            window.draw(HighScoreEasy);
            window.draw(HighScoreMedium);
            window.draw(HighScoreHard);
            window.draw(HighScoreExpert);
        }

        if (currentState == AppState::Finished) {
            /// I will display the scores + STATS;
        }

        /// ----------------------- DRAW ----------------------------

        window.display();
    }

    return 0;
}