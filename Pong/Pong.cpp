#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <cmath>

//Random angleBall
float randomAngleBall() {
	srand(time(NULL));
	short quarter = rand() % 4 + 1;
	switch (quarter) {
	case 1:
		return -10.f;
		break;
	case 2:
		return -170.f;
		break;
	case 3:
		return 10.f;
		break;
	case 4:
		return 170.f;
		break;
	}
}

int main() {	
	//Define some constants
	const int SCREEN_WIDTH = 800;
	const int SCREEN_HEIGHT = 500;
	const sf::Vector2f PADDLE_SIZE(12, 80);
	const float BALL_RADIUS = 10.f;
	const float ROCK_RADIUS = 15.f;
	const float BUBBLE_RADIUS = 18.f;
	const int NUM_ROCKS = 4;
	const int NUM_BUBBLE = 4;
	const float OUTLINE_THICKNESS = 3.f;
	const float PI = 3.14159f;

	//Create the window of application
	sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Ping Pong", sf::Style::Titlebar | sf::Style::Close);

	//Create the left paddle
	sf::RectangleShape leftPaddle;
	leftPaddle.setSize(PADDLE_SIZE);
	leftPaddle.setFillColor(sf::Color::Blue);
	leftPaddle.setOutlineThickness(OUTLINE_THICKNESS);
	leftPaddle.setOutlineColor(sf::Color(0, 0, 0));
	leftPaddle.setPosition(PADDLE_SIZE.x / 2.f + OUTLINE_THICKNESS, SCREEN_HEIGHT / 2);
	leftPaddle.setOrigin(PADDLE_SIZE / 2.f);

	//Create the right paddle
	sf::RectangleShape rightPaddle;
	rightPaddle.setSize(PADDLE_SIZE);
	rightPaddle.setFillColor(sf::Color::Red);
	rightPaddle.setOutlineThickness(OUTLINE_THICKNESS);
	rightPaddle.setOutlineColor(sf::Color(0, 0, 0));
	rightPaddle.setPosition(SCREEN_WIDTH - (PADDLE_SIZE.x / 2.f + OUTLINE_THICKNESS), SCREEN_HEIGHT / 2);
	rightPaddle.setOrigin(PADDLE_SIZE / 2.f);

	//Create the ball
	sf::CircleShape ball;
	ball.setRadius(BALL_RADIUS);
	ball.setFillColor(sf::Color::Yellow);
	ball.setOutlineThickness(OUTLINE_THICKNESS);
	ball.setOutlineColor(sf::Color::Black);
	ball.setPosition(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
	ball.setOrigin(BALL_RADIUS, BALL_RADIUS);

	//Creat rocks
	sf::CircleShape rocks[NUM_ROCKS];
	for (int i = 0; i < NUM_ROCKS; ++i) {
		rocks[i].setRadius(ROCK_RADIUS);
		rocks[i].setOutlineThickness(0);
		rocks[i].setFillColor(sf::Color(130, 135, 132));
		rocks[i].setOrigin(ROCK_RADIUS, ROCK_RADIUS);
	}
	rocks[0].setPosition(SCREEN_WIDTH / 8.f, SCREEN_HEIGHT / 8.f);
	rocks[1].setPosition(SCREEN_WIDTH / 8.f, SCREEN_HEIGHT * 7.f / 8.f);
	rocks[2].setPosition(SCREEN_WIDTH * 7.f / 8.f, SCREEN_HEIGHT / 8.f);
	rocks[3].setPosition(SCREEN_WIDTH * 7.f / 8.f, SCREEN_HEIGHT * 7.f / 8.f);

	//Create bubble
	sf::CircleShape bubbles[NUM_BUBBLE];
	for (int i = 0; i < NUM_BUBBLE; ++i) {
		bubbles[i].setRadius(BUBBLE_RADIUS);
		bubbles[i].setOutlineThickness(0);
		bubbles[i].setOrigin(BUBBLE_RADIUS, BUBBLE_RADIUS);
	}
	bubbles[0].setFillColor(sf::Color(102, 63, 117));
	bubbles[0].setPosition(SCREEN_WIDTH / 2.f, SCREEN_HEIGHT / 2.f - 100);
	bubbles[1].setFillColor(sf::Color(224, 163, 56));
	bubbles[1].setPosition(SCREEN_WIDTH / 2.f + 100, SCREEN_HEIGHT / 2.f);
	bubbles[2].setFillColor(sf::Color(212, 121, 175));
	bubbles[2].setPosition(SCREEN_WIDTH / 2.f, SCREEN_HEIGHT / 2.f + 100);
	bubbles[3].setFillColor(sf::Color(121, 212, 154));
	bubbles[3].setPosition(SCREEN_WIDTH / 2.f - 100, SCREEN_HEIGHT / 2.f);

	//Bot
	float botTarget = SCREEN_HEIGHT / 2.f;
	float botPeriod = 0.3f;
	float botTimer = 0.f;

	//Timer
	sf::Clock clock;
	float deltaTime = 0.f;

	//Sound
	sf::SoundBuffer ballBuffer;
	ballBuffer.loadFromFile("../Sound/ping-pong.wav");
	sf::Sound ballSFX;
	ballSFX.setBuffer(ballBuffer);

	sf::SoundBuffer endBuffer;
	endBuffer.loadFromFile("../Sound/End-Game.wav");
	sf::Sound endSFX;
	endSFX.setBuffer(endBuffer);

	sf::SoundBuffer bubble1Buffer;
	bubble1Buffer.loadFromFile("../Sound/Bubble1.wav");
	sf::Sound bubble1SFX;
	bubble1SFX.setBuffer(bubble1Buffer);

	sf::SoundBuffer bubble2Buffer;
	bubble2Buffer.loadFromFile("../Sound/Bubble2.wav");
	sf::Sound bubble2SFX;
	bubble2SFX.setBuffer(bubble2Buffer);

	//Music
	sf::Music backgroundMusic;
	backgroundMusic.openFromFile("../Music/rise-and-shine.wav");
	backgroundMusic.setLoop(true);
	backgroundMusic.setVolume(40.f);
	backgroundMusic.play();

	// Load font
	sf::Font font;
	if (!font.loadFromFile("../Font/DebugFreeTrial-MVdYB.otf")) {
		return EXIT_FAILURE;
	}

	//Init pause Messenger
	sf::Text pauseMessenger;
	pauseMessenger.setFont(font);
	pauseMessenger.setPosition(SCREEN_WIDTH / 2 - 180, SCREEN_HEIGHT / 2 - 50);
	pauseMessenger.setFillColor(sf::Color::Green);
	pauseMessenger.setCharacterSize(50);
	pauseMessenger.setString("Press space to play");

	//Define paddle propreties
	float paddleSpeed = 200.f;
	const float PADDLE_DELTA_X = PADDLE_SIZE.x / 2.f + OUTLINE_THICKNESS;
	const float PADDLE_DELTA_Y = PADDLE_SIZE.y / 2.f + OUTLINE_THICKNESS;

	//Define the ball proprties
	float ballSpeed = 500.f;
	float angleBall = randomAngleBall();
	const float DELTA_BALL = BALL_RADIUS * sqrt(2.f) / 2.f;

	//Game state
	bool isPlaying = false;

	//Game loop
	while (window.isOpen()) {
		deltaTime = clock.restart().asSeconds();

		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) {
				window.close();
			}
		}

		if (isPlaying == false && sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
			isPlaying = true;
		}

		if (isPlaying == true) {
			// Control Left Paddle
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
				sf::Vector2f pos = leftPaddle.getPosition();
				if ((pos.y + PADDLE_SIZE.y / 2.f + deltaTime * paddleSpeed) >= SCREEN_HEIGHT) {
					leftPaddle.setPosition(pos.x, SCREEN_HEIGHT - PADDLE_DELTA_Y);
				}
				leftPaddle.move(0, paddleSpeed * deltaTime);
			}
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
				sf::Vector2f pos = leftPaddle.getPosition();
				if ((pos.y - PADDLE_SIZE.y / 2.f - deltaTime * paddleSpeed) <= 0) {
					leftPaddle.setPosition(pos.x, 0 + PADDLE_DELTA_Y);
				}
				leftPaddle.move(0, - paddleSpeed * deltaTime);
			}

			//Control Right Paddle (without bot)
			/*if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
				sf::Vector2f pos = rightPaddle.getPosition();
				if ((pos.y + PADDLE_SIZE.y / 2.f + deltaTime * paddleSpeed) >= SCREEN_HEIGHT) {
					rightPaddle.setPosition(pos.x, SCREEN_HEIGHT - PADDLE_DELTA_Y);
				}
				rightPaddle.move(0, paddleSpeed * deltaTime);
			}
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
				sf::Vector2f pos = rightPaddle.getPosition();
				if ((pos.y - PADDLE_SIZE.y / 2.f - deltaTime * paddleSpeed) <= 0) {
					rightPaddle.setPosition(pos.x, 0 + PADDLE_DELTA_Y);
				}
				rightPaddle.move(0, -paddleSpeed * deltaTime);
			}*/

			//Control Right Paddle (with bot)
			//Update botTarget
			botTimer += deltaTime;
			if (botTimer >= botPeriod && cos(angleBall * PI / 180.f) > 0.f) {
				botTarget = ball.getPosition().y;
				botTimer = 0.f;
			}
			if (rightPaddle.getPosition().y + PADDLE_SIZE.y / 8 < botTarget) {
				sf::Vector2f pos = rightPaddle.getPosition();
				if ((pos.y + PADDLE_SIZE.y / 2.f + deltaTime * paddleSpeed) >= SCREEN_HEIGHT) {
					rightPaddle.setPosition(pos.x, SCREEN_HEIGHT - PADDLE_DELTA_Y);
				}
				rightPaddle.move(0, paddleSpeed * deltaTime);
			}
			else if (rightPaddle.getPosition().y - PADDLE_SIZE.y / 8 > botTarget) {
				sf::Vector2f pos = rightPaddle.getPosition();
				if ((pos.y - PADDLE_SIZE.y / 2.f - deltaTime * paddleSpeed) <= 0) {
					rightPaddle.setPosition(pos.x, 0 + PADDLE_DELTA_Y);
				}
				rightPaddle.move(0, -paddleSpeed * deltaTime);
			}


			// Move ball
			float factor = ballSpeed * deltaTime;
			ball.move(cos(angleBall * PI / 180) * factor, sin(angleBall * PI / 180) * factor);

			//check collision top_down wall
			if ((ball.getPosition().y - BALL_RADIUS) <= 0 || (ball.getPosition().y + BALL_RADIUS) >= SCREEN_HEIGHT) {
				ballSFX.play();
				angleBall = angleBall - angleBall * 2;
			}


			//Check collision with right paddle
			if ((ball.getPosition().x + BALL_RADIUS) >= (rightPaddle.getPosition().x - PADDLE_DELTA_X)) {
				if (ball.getPosition().y > (rightPaddle.getPosition().y - PADDLE_DELTA_Y - DELTA_BALL)
					&& ball.getPosition().y < (rightPaddle.getPosition().y + PADDLE_DELTA_Y + DELTA_BALL)) {
					ballSFX.play();
					angleBall = 180.f - angleBall;
				}
				else if (ball.getPosition().y == (rightPaddle.getPosition().y - PADDLE_DELTA_Y - DELTA_BALL)
						|| ball.getPosition().y == (rightPaddle.getPosition().y + PADDLE_DELTA_Y + DELTA_BALL)) {
					ballSFX.play();
					if (cos(angleBall * PI / 180.f) == sqrt(2.f) / 2.f) {
						angleBall = 180.f + angleBall;
					}
					else if (cos(angleBall * PI / 180.f) > sqrt(2.f) / 2.f) {
						angleBall = 180.f - angleBall;
					}
				}
				else {
					if ((ball.getPosition().y + BALL_RADIUS) >= (rightPaddle.getPosition().y - PADDLE_DELTA_Y)
						&& sin(angleBall * PI / 180.f) > 0) {
						ballSFX.play();
						angleBall = angleBall - angleBall * 2;
					}
					else if ((ball.getPosition().y - BALL_RADIUS) <= (rightPaddle.getPosition().y + PADDLE_DELTA_Y)
						&& sin(angleBall * PI /180.f) < 0) {
						ballSFX.play();
						angleBall = angleBall - angleBall * 2;
					}
				}
			}
			//Check collision with left paddle
			else if ((ball.getPosition().x - BALL_RADIUS) <= (leftPaddle.getPosition().x + PADDLE_DELTA_X)) {
				if (ball.getPosition().y > (leftPaddle.getPosition().y - PADDLE_DELTA_Y - DELTA_BALL)
					&& ball.getPosition().y < (leftPaddle.getPosition().y + PADDLE_DELTA_Y + DELTA_BALL)) {
					ballSFX.play();
					angleBall = 180.f - angleBall;
				}
				else if (ball.getPosition().y == (leftPaddle.getPosition().y - PADDLE_DELTA_Y - DELTA_BALL)
					|| ball.getPosition().y == (leftPaddle.getPosition().y + PADDLE_DELTA_Y + DELTA_BALL)) {
					ballSFX.play();
					if (cos(angleBall * PI / 180.f) == -sqrt(2.f) / 2.f) {
						angleBall = 180 + angleBall;
					}
					else if (cos(angleBall * PI / 180.f) < -sqrt(2.f) / 2.f) {
						angleBall = 180.f - sqrt(2.f) / 2.f;
					}
					else {
						angleBall = angleBall - angleBall * 2;
					}
				}
				else {
					if ((ball.getPosition().y + BALL_RADIUS) >= (leftPaddle.getPosition().y - PADDLE_DELTA_Y)
						&& sin(angleBall * PI / 180.f) > 0) {
						ballSFX.play();
						angleBall = angleBall - angleBall * 2;
					}
					else if ((ball.getPosition().y - BALL_RADIUS) <= (leftPaddle.getPosition().y + PADDLE_DELTA_Y)
						&& sin(angleBall * PI / 180.f) < 0) {
						ballSFX.play();
						angleBall = angleBall - angleBall * 2;
					}
				}
			}

			//Check collision with rocks
			float distance;
			for (int i = 0; i < NUM_ROCKS; ++i) {
				distance = sqrt((ball.getPosition().x - rocks[i].getPosition().x) * (ball.getPosition().x - rocks[i].getPosition().x) 
							+ (ball.getPosition().y - rocks[i].getPosition().y) * (ball.getPosition().y - rocks[i].getPosition().y));
				if (distance <= (BALL_RADIUS + ROCK_RADIUS)) {
					ballSFX.play();
					angleBall = 180.f - angleBall;
				}
			}

			//Check collision with Bubbles
			for (int i = 0; i < NUM_BUBBLE; ++i) {
				distance = sqrt((ball.getPosition().x - bubbles[i].getPosition().x) * (ball.getPosition().x - bubbles[i].getPosition().x)
					+ (ball.getPosition().y - bubbles[i].getPosition().y) * (ball.getPosition().y - bubbles[i].getPosition().y));
				if (distance <= (BUBBLE_RADIUS - BALL_RADIUS)) {
					bubble1SFX.play();
					angleBall = 330.f - angleBall;
				}
				else if (distance > (BUBBLE_RADIUS - BALL_RADIUS) && distance <= (BUBBLE_RADIUS + BALL_RADIUS)) {
					bubble2SFX.play();
				}
			}

			//End Game Handle
			if ((ball.getPosition().x + BALL_RADIUS) >= SCREEN_WIDTH) {
				endSFX.play();
				pauseMessenger.setString("   Blue is winner\nPress space to play");
				ball.setPosition(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
				leftPaddle.setPosition(PADDLE_SIZE.x / 2.f + OUTLINE_THICKNESS, SCREEN_HEIGHT / 2);
				rightPaddle.setPosition(SCREEN_WIDTH - PADDLE_SIZE.x / 2.f - OUTLINE_THICKNESS, SCREEN_HEIGHT / 2);
				angleBall = randomAngleBall();
				isPlaying = false;
			}
			else if ((ball.getPosition().x - BALL_RADIUS) <= 0) {
				endSFX.play();
				pauseMessenger.setString("     Red is winner\nPress spcace to play");
				ball.setPosition(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
				leftPaddle.setPosition(PADDLE_SIZE.x / 2.f + OUTLINE_THICKNESS, SCREEN_HEIGHT / 2);
				rightPaddle.setPosition(SCREEN_WIDTH - PADDLE_SIZE.x / 2.f - OUTLINE_THICKNESS, SCREEN_HEIGHT / 2);
				angleBall = randomAngleBall();
				isPlaying = false;
			}
		}

		//Display object
		window.clear(sf::Color::White);
		if (isPlaying == true) {
			window.draw(leftPaddle);
			window.draw(rightPaddle);
			window.draw(ball);
			for (int i = 0; i < NUM_ROCKS; ++i) {
				window.draw(rocks[i]);
			}
			for (int i = 0; i < NUM_BUBBLE; ++i) {
				window.draw(bubbles[i]);
			}
		}
		else {
			window.draw(pauseMessenger);
		}
		window.display();
	}

	return 0;
}