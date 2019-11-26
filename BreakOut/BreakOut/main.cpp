#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>
#include <SFML/System.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/Main.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>
#include <SFML/System.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/Main.hpp>
#include <random>
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <list>

const float LENGTHOFTHEWINDOW = 800.f;
const float HEIGHTHTOFTHEWINDOW = LENGTHOFTHEWINDOW * 0.75;
const float SPEEDOFPADDLEPERSECOND = LENGTHOFTHEWINDOW;
const float SPEEDOFBALLPERSECOND = LENGTHOFTHEWINDOW / 2;
const float LENGTHOFTHEPADDLE = LENGTHOFTHEWINDOW / 8;
const float HEIGHTOFTHEPADDLE = LENGTHOFTHEPADDLE / 5;
const float WIDTHOFTHEINTERVAL = HEIGHTOFTHEPADDLE;
const float RADIUSOFTHEBALL = HEIGHTOFTHEPADDLE / 2;
const float WIDTHFORPRESENTSCOREANDLIFE = HEIGHTHTOFTHEWINDOW * 0.1;
const int FONTSIZEOFSCOREBOARD = LENGTHOFTHEWINDOW / 16;
const int FONTSIZEOFPOWERUP = LENGTHOFTHEWINDOW / 50;
const int WINSCORE = 5;
const int COLUMNNUMBEROFBRICK = 16;
const int ROWNUMBEROFBRICK = 12;
const int THICKNESSOFBRICK = 1;
const float LENGTHOFTHEBRICK = LENGTHOFTHEWINDOW / COLUMNNUMBEROFBRICK;
const float HEIGHTOFTHEBRICK = HEIGHTHTOFTHEWINDOW / (2 * ROWNUMBEROFBRICK);
const float LENGTHOFPOWERUPBOUND = LENGTHOFTHEBRICK * 0.64;
const int MAXHEALTHPOINT = 5;
const int POINTOFEACHBRICK = 25;
const float INTERVALBETWEENLIFEPADDLEANDPADDLE = LENGTHOFTHEWINDOW * 0.4 / MAXHEALTHPOINT - LENGTHOFTHEPADDLE * 0.4;
const float INTERVALBETWEENLIFEPADDLEANDWINDOWTOP = (WIDTHFORPRESENTSCOREANDLIFE - HEIGHTOFTHEPADDLE * 0.4) / 2;
const float POWERUPTIME = 5;
enum class AlignOfText { leftAlign, center, rightAlign };
enum class PowerUpType { longerPaddle, slowerBall, extraPoint};
enum class BrickType {normal,twice,burn,invincible,invisible};
sf::Sound soundOfTheBallHitThePalldle;
sf::SoundBuffer bufferOfTheBallHitThePalldle;
sf::Sound soundOfTheBallHitTheBound;
sf::SoundBuffer bufferOfTheBallHitTheBound;
sf::Sound soundOfTheBallHitTheBrick;
sf::SoundBuffer bufferOfTheBallHitTheBrick;
sf::Sound soundOfTheBallBreakTheBrick;
sf::SoundBuffer bufferOfTheBallBreakTheBrick;
sf::Sound soundOfLevelClear;
sf::SoundBuffer bufferOfLevelClear;
sf::Sound soundOfLostLife;
sf::SoundBuffer bufferOfLostLife;
sf::Sound soundOfBurnBrick;
sf::SoundBuffer bufferOfBurnBrick;
sf::Texture textureOfFireBrick;
sf::Texture textureOfNormalBrick;
sf::Texture textureOfInvinsibleBrick;
sf::Texture textureOfInvisibleBrick;
sf::Texture textureOfStoneBrick;
sf::Texture textureOfBrokenStoneBrick;
sf::Texture textureOfSlowBall;
sf::Texture textureOfLongerPaddle;
sf::Texture textureOfExtraPoint;

using namespace std;

class Ball :public sf::CircleShape {
public:
	float speedPerSecond = SPEEDOFBALLPERSECOND;
	float basicalSpeed = SPEEDOFPADDLEPERSECOND;
	float radius = RADIUSOFTHEBALL;
	void* pointerOfObjectLastHit = nullptr;
	int topBound = 1, leftBound = 1, rightBound = 1;
	sf::Vector2f velocityPerSecond;
	Ball(float radiusOfTheBall) {
		speedPerSecond = SPEEDOFBALLPERSECOND;
		radius = radiusOfTheBall;
		setRadius(radius);
		setOrigin(radius, radius);
		setFillColor(sf::Color::White);
		pointerOfObjectLastHit = nullptr;
	}
	void initializeSpeed() {
		speedPerSecond = basicalSpeed;
		velocityPerSecond.x = speedPerSecond * 0.8 * 0.05;
		velocityPerSecond.y = -sqrt(speedPerSecond * speedPerSecond - velocityPerSecond.x * velocityPerSecond.x);
	}
	bool boundBounceDectect() {
		if (pointerOfObjectLastHit != &topBound && velocityPerSecond.y < 0 && getPosition().y < radius) {
			velocityPerSecond.y = -velocityPerSecond.y;
			pointerOfObjectLastHit = &topBound;
			soundOfTheBallHitTheBound.play();
		}
		if ((pointerOfObjectLastHit != &leftBound && velocityPerSecond.x < 0 && getPosition().x < radius) || (pointerOfObjectLastHit != &rightBound && velocityPerSecond.x > 0 && getPosition().x > (LENGTHOFTHEWINDOW - radius))) {
			velocityPerSecond.x = - velocityPerSecond.x;
			pointerOfObjectLastHit = velocityPerSecond.x > 0 ? &leftBound : &rightBound;
			soundOfTheBallHitTheBound.play();
		}
		if (velocityPerSecond.y > 0 && getPosition().y > HEIGHTHTOFTHEWINDOW) {
			return true;
		}
		return false;
	}
};

class Paddle :public sf::RectangleShape {
public:
	sf::Texture textureOfPaddle;
	sf::Vector2f speedOfPaddle;
	float height = HEIGHTOFTHEPADDLE;
	float length = LENGTHOFTHEPADDLE;
	float basicLength = LENGTHOFTHEPADDLE;
	float extraLength = 0;
	float speedPerSecond =SPEEDOFPADDLEPERSECOND;
	Paddle(float lengthOfThePaddle, float heightOfThePaddle,sf::Vector2f position) {
		initializePaddle(lengthOfThePaddle, heightOfThePaddle, position);
	}
	Paddle() {};
	void initializePaddle(float lengthOfThePaddle, float heightOfThePaddle, sf::Vector2f position) {
		height = heightOfThePaddle;
		basicLength = lengthOfThePaddle;
		extraLength = 0;
		length = basicLength + extraLength;
		setFillColor(sf::Color::White);
		setSize(sf::Vector2f(length, height));
		speedOfPaddle = sf::Vector2f(0, 0);
		textureOfPaddle.loadFromFile("wood.jpg");
		setTexture(&textureOfPaddle);
		setPosition(position);
	}
	void controlPaddle(bool left, bool right, float timePerFrame) {
		length = basicLength + extraLength;
		float speedPerFrame = timePerFrame * speedPerSecond;
		if (left) {
			getPosition().x <= 0 ? speedOfPaddle = sf::Vector2f(0, 0) : speedOfPaddle = sf::Vector2f(-speedPerFrame,0);
			move(speedOfPaddle);
		}
		if (right) {
			getPosition().x >= LENGTHOFTHEWINDOW - length ? speedOfPaddle = sf::Vector2f(0, 0) : speedOfPaddle = sf::Vector2f(speedPerFrame, 0);
			move(speedOfPaddle);
		}
	}

	void carryTheBall(Ball& ball) {
		length = basicLength + extraLength;
		ball.speedPerSecond = 0;
		ball.velocityPerSecond.x = ball.velocityPerSecond.y = 0;
		ball.setPosition(getPosition().x + length * 0.55, getPosition().y - ball.getRadius());
	}

	bool collisionDetect(Ball& ball) {
		length = basicLength + extraLength;
		sf::Vector2f positionOfTheBall = ball.getPosition();
		float leftCheckingLine = getPosition().x - ball.getRadius();
		float rightCheckingLine = getPosition().x + getSize().x + ball.getRadius();
		float topCheckingLine = getPosition().y - ball.getRadius();
		float bottomCheckingLine = getPosition().y + getSize().y + ball.getRadius();
		if (ball.pointerOfObjectLastHit != this) {
			if (positionOfTheBall.x < rightCheckingLine && positionOfTheBall.x > leftCheckingLine&& positionOfTheBall.y < bottomCheckingLine && positionOfTheBall.y > topCheckingLine) {
				if (positionOfTheBall.x < getPosition().x + getSize().x && positionOfTheBall.x > getPosition().x) {
					ball.velocityPerSecond.x = (positionOfTheBall.x - (getPosition().x + length / 2)) / (length / 2) * ball.speedPerSecond * 0.8;
					ball.velocityPerSecond.y = -sqrt(ball.speedPerSecond * ball.speedPerSecond - ball.velocityPerSecond.x * ball.velocityPerSecond.x);
					ball.pointerOfObjectLastHit = this;
					return true;
				}
				else if (positionOfTheBall.y < getPosition().y + getSize().y && positionOfTheBall.y > getPosition().y) {
					ball.pointerOfObjectLastHit = this;
					ball.velocityPerSecond.x = -ball.velocityPerSecond.x;
					return true;
				}
				else {
					float horizontalDistance = abs(positionOfTheBall.x - getPosition().x - getSize().x / 2) - getSize().x / 2;
					float verticalDistance = abs(positionOfTheBall.y - getPosition().y - getSize().y / 2) - getSize().y / 2;
					if (verticalDistance * verticalDistance + horizontalDistance * horizontalDistance <= ball.getRadius() * ball.getRadius()) {
						ball.velocityPerSecond.y = -ball.velocityPerSecond.y;
						ball.velocityPerSecond.x = -ball.velocityPerSecond.x;
						ball.pointerOfObjectLastHit = this;
						return true;
					}
				}
			}
		}
		return false;
	}
};

class Brick :public sf::RectangleShape {
public:
	int positionOnRow = 0;
	int positionOnColumn = 0;
	int hitTimes = 0;
	sf::Color colorOfBrick;
	BrickType brickType;
	Brick(int rowNumber,int columnNumber,BrickType type) {
		initializeBrick(rowNumber, columnNumber, type);
	}
	Brick() { 
		brickType = BrickType::invisible; 
		colorOfBrick = sf::Color::Transparent;
	}
	void initializeBrick(int rowNumber, int columnNumber, BrickType type) {
		brickType = type;
		positionOnRow = rowNumber;
		positionOnColumn = columnNumber;
		switch (brickType)
		{
		case BrickType::normal: {
			hitTimes = 1;
			/*int i = rand() % 6 + 1;
			switch (i)
			{
			case 1:colorOfBrick = sf::Color::Blue; break;
			case 2:colorOfBrick = sf::Color::Cyan; break;
			case 3:colorOfBrick = sf::Color::Green; break;
			case 4:colorOfBrick = sf::Color::Red; break;
			case 5:colorOfBrick = sf::Color::Magenta; break;
			case 6:colorOfBrick = sf::Color::Yellow; break;
			default:
				break;
			}*/
			colorOfBrick = sf::Color::Color(60, 120, 255, 255);
			setTexture(&textureOfNormalBrick);
			break;
		}
		case BrickType::twice:
			hitTimes = 2;
			colorOfBrick = sf::Color::White;
			setTexture(&textureOfStoneBrick);
			break;
		case BrickType::invincible:
			hitTimes = 999;
			colorOfBrick = sf::Color::White;
			setTexture(&textureOfInvinsibleBrick);
			break;
		case BrickType::burn:
			hitTimes = 1;
			colorOfBrick = sf::Color::White;
			setTexture(&textureOfFireBrick);
			break;
		case BrickType::invisible:
			hitTimes = 2;
			colorOfBrick = sf::Color::Transparent;
			break;
		default:
			break;
		}
		setSize(sf::Vector2f(LENGTHOFTHEBRICK - 2 * THICKNESSOFBRICK, HEIGHTOFTHEBRICK - 2 * THICKNESSOFBRICK));
		setFillColor(colorOfBrick);
		setOutlineThickness(THICKNESSOFBRICK);
		setOutlineColor(sf::Color::Black);
		setPosition(sf::Vector2f(columnNumber * LENGTHOFTHEBRICK, rowNumber * HEIGHTOFTHEBRICK + WIDTHFORPRESENTSCOREANDLIFE));
	}
	bool upDateBrick(int* score,bool burning = false) {
		switch (brickType)
		{
		case BrickType::normal:burning ? *score += POINTOFEACHBRICK : *score += POINTOFEACHBRICK * 2; break;
		case BrickType::twice:
			colorOfBrick = sf::Color::Color(100, 100, 100, 255);
			burning ? *score += POINTOFEACHBRICK : *score += POINTOFEACHBRICK * 2 * (3 - hitTimes);
			setTexture(&textureOfBrokenStoneBrick);
			break;
		case BrickType::invincible:burning ? *score += POINTOFEACHBRICK : *score += 1; break;
		case BrickType::burn:
			*score += POINTOFEACHBRICK;
			hitTimes -= 1;
			return true;
			break;
		case BrickType::invisible:
			burning ? *score += POINTOFEACHBRICK : *score += POINTOFEACHBRICK * 2 * (3 - hitTimes);
			colorOfBrick = sf::Color::Color(255, 204, 204, 255);
			setTexture(&textureOfInvisibleBrick);
			break;
		default:
			break;
		}
		if (burning) {
			hitTimes = 0;
		}
		else {
			hitTimes -= 1;
			if (hitTimes == 0) {
				soundOfTheBallBreakTheBrick.play();
			}
			else {
				soundOfTheBallHitTheBrick.play();
			}
		}
		setFillColor(colorOfBrick);
		return false;
	}
	bool collisionDetect(Ball& ball) {
		sf::Vector2f positionOfTheBall = ball.getPosition();
		float leftCheckingLine = getPosition().x - ball.getRadius();
		float rightCheckingLine = getPosition().x + getSize().x + ball.getRadius();
		float topCheckingLine = getPosition().y - ball.getRadius();
		float bottomCheckingLine = getPosition().y + getSize().y + ball.getRadius();
		if (ball.pointerOfObjectLastHit != this) {
			if (positionOfTheBall.x < rightCheckingLine && positionOfTheBall.x > leftCheckingLine&& positionOfTheBall.y < bottomCheckingLine && positionOfTheBall.y > topCheckingLine) {
				if (positionOfTheBall.x < getPosition().x + getSize().x && positionOfTheBall.x > getPosition().x) {
					ball.velocityPerSecond.y = -ball.velocityPerSecond.y;
					ball.pointerOfObjectLastHit = this;
					return true;
				}
				else if (positionOfTheBall.y < getPosition().y + getSize().y && positionOfTheBall.y > getPosition().y) {
					ball.velocityPerSecond.x = -ball.velocityPerSecond.x;
					ball.pointerOfObjectLastHit = this;
					return true;
				}
				else {
					float horizontalDistance = abs(positionOfTheBall.x - getPosition().x - getSize().x / 2) - getSize().x / 2;
					float verticalDistance = abs(positionOfTheBall.y - getPosition().y - getSize().y / 2) - getSize().y / 2;
					if (verticalDistance * verticalDistance + horizontalDistance * horizontalDistance < ball.getRadius() * ball.getRadius()) {
						ball.velocityPerSecond.y = -ball.velocityPerSecond.y;
						ball.velocityPerSecond.x = -ball.velocityPerSecond.x;
						ball.pointerOfObjectLastHit = this;
						return true;
					}
				}
			}
		}
		return false;
	}	
};

class TextForPong :public sf::Text {
public:
	sf::Font font;
	TextForPong() {};
	TextForPong(const sf::String& contentOfTheText, int fontSizeOfTheText, sf::Color colorOfTheText, sf::Vector2f positionOfTheText, AlignOfText alignMethod) {
		initateTextForPong(contentOfTheText, fontSizeOfTheText, colorOfTheText, positionOfTheText, alignMethod);
	}
	void initateTextForPong(const sf::String& contentOfTheText, int fontSizeOfTheText, sf::Color colorOfTheText, sf::Vector2f positionOfTheText, AlignOfText alignMethod) {
		font.loadFromFile("arial.ttf");
		setFont(font);
		setString(contentOfTheText);
		setCharacterSize(fontSizeOfTheText);
		setFillColor(colorOfTheText);
		sf::FloatRect borderOfTheText = getLocalBounds();
		switch (alignMethod) {
		case AlignOfText::leftAlign:setPosition(positionOfTheText); break;
		case AlignOfText::center:setOrigin(sf::Vector2f(borderOfTheText.width / 2, 0)); setPosition(positionOfTheText); break;
		case AlignOfText::rightAlign:setOrigin(sf::Vector2f(borderOfTheText.width, 0)); setPosition(positionOfTheText); break;
		}
	}
};

class PowerUp:public sf::RectangleShape {
public:
	PowerUpType powerUpType;
	sf::Vector2f startPosition;
	float speedOfFalling = SPEEDOFPADDLEPERSECOND / 4;
	bool getByPlayer = false;
	sf::Clock buffTime;
	PowerUp(PowerUpType type,Brick brick) {
		startPosition = brick.getPosition() + sf::Vector2f(LENGTHOFTHEBRICK / 2, HEIGHTOFTHEBRICK / 2);
		powerUpType = type;
		switch (powerUpType)
		{
		case PowerUpType::longerPaddle:setFillColor(sf::Color::White); setTexture(&textureOfLongerPaddle); break;
		case PowerUpType::slowerBall:setFillColor(sf::Color::White); setTexture(&textureOfSlowBall); break;
		case PowerUpType::extraPoint:setFillColor(sf::Color::White); setTexture(&textureOfExtraPoint); break;
		default:
			break;
		}
		setOutlineThickness(3);
		setOutlineColor(sf::Color::Color(18, 150, 219, 255));
		setOrigin(sf::Vector2f(LENGTHOFPOWERUPBOUND / 2, 0));
		setPosition(startPosition);
		setSize(sf::Vector2f(LENGTHOFPOWERUPBOUND, LENGTHOFPOWERUPBOUND));
		getByPlayer = false;
		buffTime.restart();
	}
	void PowerUpDectect(Paddle paddle,int *score) {
		if (getGlobalBounds().intersects(paddle.getGlobalBounds())) {
			if (getByPlayer == false) {
				if (powerUpType == PowerUpType::extraPoint) {
					*score += 100;
				}
				buffTime.restart();
				*score += 75;
			}
			getByPlayer = true;			
		}
	}
};

void loadLevelSetting(std::string levelFilePath, std::list<Brick> &brick) {
	brick.clear();
	std::ifstream levelFile;
	levelFile.open(levelFilePath, std::ios::in);
	if (!levelFile.is_open()) {
		std::cout << "Can not find " << levelFilePath << std::endl;
	}
	std::string buffer;
	int rowNumber = 0;
	while (getline(levelFile, buffer)&&rowNumber<ROWNUMBEROFBRICK) {
		int columnNumber = 0;
		for (int i=0;i<buffer.length()&&columnNumber<COLUMNNUMBEROFBRICK;i++){
			if ((buffer[i] >= '1') && (buffer[i] <= '5')) {
				Brick newBrick(rowNumber, columnNumber, BrickType(int(buffer[i]-'0')-1));
				brick.push_back(newBrick);
			}
			columnNumber++;
		}
		rowNumber++;
	}
	levelFile.close();
}

void brickCollectionDetect(std::list<Brick> &listOfBrick, std::list<sf::Vector2i> &listOfBurningBrick,std::list<PowerUp> &listOfPowerUp,Ball &ball,sf::Clock &clockForBurning,int* score,int* accumulateNumber) {
	std::list<Brick>::iterator i;
	for (i = listOfBrick.begin(); i != listOfBrick.end();) {
		if (i->collisionDetect(ball)) {
			if (i->upDateBrick(score)) {
				listOfBurningBrick.push_back(sf::Vector2i(i->positionOnRow, i->positionOnColumn));
				clockForBurning.restart();
			}
		}
		if (i->hitTimes <= 0) {
			if (*accumulateNumber > rand()%10 && *accumulateNumber > 3) {
				PowerUp newPowerUp(PowerUpType(rand() % 3), *i);
				listOfPowerUp.push_back(newPowerUp);
				*accumulateNumber = 0;
			}
			else {
				(*accumulateNumber)++;
			}
			listOfBrick.erase(i++);
		}
		else {
			i++;
		}
	}
}

void burnBrick(std::list<Brick>& listOfBrick, std::list<sf::Vector2i>& listOfBurningBrick, std::list<PowerUp>& listOfPowerUp, sf::Clock& clockForBurning, int* score, int* accumulateNumber) {
	sf::Vector2i positionOfBurningBrick = *listOfBurningBrick.begin();
	std::list<Brick>::iterator i;
	for (i = listOfBrick.begin(); i != listOfBrick.end();) {
		if (i->positionOnColumn == int(positionOfBurningBrick.y) && abs(i->positionOnRow - int(positionOfBurningBrick.x)) == 1 || i->positionOnRow == int(positionOfBurningBrick.x) && abs(i->positionOnColumn - int(positionOfBurningBrick.y)) == 1) {
			if (i->upDateBrick(score,true)) {
				listOfBurningBrick.push_back(sf::Vector2i(i->positionOnRow, i->positionOnColumn));
			}
		}
		if (i->hitTimes <= 0) {
			if (*accumulateNumber > rand() % 20 && *accumulateNumber > 5) {
				PowerUp newPowerUp(PowerUpType(rand() % 3), *i);
				listOfPowerUp.push_back(newPowerUp);
				*accumulateNumber = 0;
			}
			else {
				(*accumulateNumber)++;
			}
			listOfBrick.erase(i++);
		}
		else {
			i++;
		}
	}
	listOfBurningBrick.pop_front();
	clockForBurning.restart();
}

void powerUpCheck(std::list<PowerUp>& listOfPowerUp, int* score,Paddle &paddle,float timePerFrame) {
	std::list<PowerUp>::iterator i;
	for (i = listOfPowerUp.begin(); i != listOfPowerUp.end();) {
		i->PowerUpDectect(paddle,score);
		bool deleteSelf = false;
		if (i->getByPlayer) {
			if (i->powerUpType == PowerUpType::extraPoint || i->buffTime.getElapsedTime().asSeconds() > 10) {
				deleteSelf = true;
			}
		}
		else {
			i->move(0, i->speedOfFalling * timePerFrame);
			if (i->getPosition().y >= HEIGHTHTOFTHEWINDOW) {
				deleteSelf = true;
			}
		}
		if (deleteSelf) {
			listOfPowerUp.erase(i++);
		}
		else {
			i++;
		}
	}
}

int main()
{
	std::list<Brick> listOfBrick;
	std::list<sf::Vector2i> listOfBurningBrick;
	std::list<PowerUp> listOfPowerUp;
	sf::RenderWindow window(sf::VideoMode(LENGTHOFTHEWINDOW, HEIGHTHTOFTHEWINDOW), "Breakout");
	Paddle paddle(LENGTHOFTHEPADDLE,HEIGHTOFTHEPADDLE, sf::Vector2f(LENGTHOFTHEWINDOW / 2 - LENGTHOFTHEPADDLE / 2, HEIGHTHTOFTHEWINDOW - WIDTHOFTHEINTERVAL - HEIGHTOFTHEPADDLE));
	Ball ball(RADIUSOFTHEBALL);
	Paddle lifePaddle[MAXHEALTHPOINT];
	for (int i = 0; i < MAXHEALTHPOINT; i++) {
		lifePaddle[i].initializePaddle(LENGTHOFTHEPADDLE * 0.4, HEIGHTOFTHEPADDLE * 0.4, sf::Vector2f(LENGTHOFTHEWINDOW * 0.6 + i * (LENGTHOFTHEWINDOW * 0.4) / MAXHEALTHPOINT + INTERVALBETWEENLIFEPADDLEANDPADDLE / 2, INTERVALBETWEENLIFEPADDLEANDWINDOWTOP));
	};
	int healthPoint = 3;
	int levelNumber = 0;
	bool ballStick = true;
	bool gameStart = false;
	bool newLifeStart = false;
	bool gamePaused = false;
	bool mouseControl = false;
	bool loadNewLevel = true;
	bool normalBrickExist = true;
	bool burnOut = true;
	int accumulateNumber = 0;
	float ballSpeedCoefficient = 0;
	sf::Vector2i mousePosition;
	float timePerFrame = 0;
	int score = 0;
	int currentScore = 0;
	textureOfFireBrick.loadFromFile("fire.jpg");
	textureOfBrokenStoneBrick.loadFromFile("brokenStone.jpg");
	textureOfInvinsibleBrick.loadFromFile("steel.jpg");
	textureOfInvisibleBrick.loadFromFile("glass.jpg");
	textureOfNormalBrick.loadFromFile("brick.jpg");
	textureOfStoneBrick.loadFromFile("stone.jpg");
	textureOfSlowBall.loadFromFile("slow.png");
	textureOfExtraPoint.loadFromFile("100.png");
	textureOfLongerPaddle.loadFromFile("longer.png");
	bufferOfTheBallHitThePalldle.loadFromFile("ballHitPaddle.wav");
	soundOfTheBallHitThePalldle.setBuffer(bufferOfTheBallHitThePalldle);
	bufferOfLevelClear.loadFromFile("levelClear.wav");
	soundOfLevelClear.setBuffer(bufferOfLevelClear);
	bufferOfLostLife.loadFromFile("lostLife.wav");
	soundOfLostLife.setBuffer(bufferOfLostLife);
	bufferOfTheBallHitTheBrick.loadFromFile("ballHitBrick.wav");
	soundOfTheBallHitTheBrick.setBuffer(bufferOfTheBallHitTheBrick);
	bufferOfTheBallHitTheBound.loadFromFile("ballHitWall.wav");
	soundOfTheBallHitTheBound.setBuffer(bufferOfTheBallHitTheBound);
	bufferOfTheBallBreakTheBrick.loadFromFile("brickBroken.wav");
	soundOfTheBallBreakTheBrick.setBuffer(bufferOfTheBallBreakTheBrick);
	bufferOfBurnBrick.loadFromFile("burnBrick.wav");
	soundOfBurnBrick.setBuffer(bufferOfBurnBrick);


	sf::Clock clock;
	sf::Clock clockForBurning;
	
	while (window.isOpen())
	{
		if (loadNewLevel) {
			std::string levelPath = "level" + std::to_string(levelNumber%3+1) + ".txt";
			loadLevelSetting(levelPath, listOfBrick);
			ball.basicalSpeed = SPEEDOFBALLPERSECOND * (1 + 0.2 * levelNumber);
			levelNumber++;
			loadNewLevel = false;
		}

		if (currentScore < score) {
			currentScore += (score-currentScore)/100 +1;
			//make score increase continuously 
		}
		TextForPong scoreBoard(std::to_string(currentScore), FONTSIZEOFSCOREBOARD, sf::Color::Color(60,138,255,255), sf::Vector2f(0,0), AlignOfText::leftAlign);
		if (gameStart) {
			timePerFrame = clock.restart().asSeconds();
			sf::Event event;
			while (window.pollEvent(event))
			{
				if (event.type == sf::Event::Closed)
					window.close();
				if (event.type == sf::Event::MouseButtonReleased) {
					if (event.mouseButton.button == sf::Mouse::Left) {
						if (!newLifeStart && gameStart) {
							newLifeStart = true;
							ballStick = false;
							ball.initializeSpeed();
							clock.restart();
						}
						gamePaused ? gamePaused = false : NULL;
					}
				}
				if (event.type == sf::Event::KeyReleased) {
					if (event.key.code == sf::Keyboard::Space) {
						if (!newLifeStart && gameStart) {
							newLifeStart = true;
							ballStick = false;
							ball.initializeSpeed();
							clock.restart();
						}
						gamePaused ? gamePaused = false : NULL;
					}
					if (event.key.code == sf::Keyboard::Left || event.key.code == sf::Keyboard::Right) {
						mouseControl = false;
						mousePosition = sf::Mouse::getPosition(window);
					}
				}
				if (event.type == sf::Event::LostFocus)
					gamePaused = true;
			}
			if (!gamePaused) {
				if (sf::Mouse::getPosition(window) != mousePosition) {
					mouseControl = true;
				}
				if (mouseControl) {
					if (int(paddle.getPosition().x + paddle.length / 2 + RADIUSOFTHEBALL) < sf::Mouse::getPosition(window).x) {
						paddle.controlPaddle(false, true, timePerFrame);
					}
					else if (int(paddle.getPosition().x + paddle.length / 2 - RADIUSOFTHEBALL) > sf::Mouse::getPosition(window).x) {
						paddle.controlPaddle(true, false, timePerFrame);
					}
				}
				paddle.controlPaddle(sf::Keyboard::isKeyPressed(sf::Keyboard::Left), sf::Keyboard::isKeyPressed(sf::Keyboard::Right), timePerFrame);
				if (ballStick) {
					paddle.carryTheBall(ball);
				}
				
				if (paddle.collisionDetect(ball)) {
					soundOfTheBallHitThePalldle.play();
				}

				if (ball.boundBounceDectect()) {
					healthPoint -= 1;
					newLifeStart = false;
					ballStick = true;
					listOfPowerUp.clear();
					accumulateNumber = 0;
					ballSpeedCoefficient = 0;
					paddle.extraLength = 0;
					if (healthPoint <= 0) {
						gameStart = false;
					}
					soundOfLostLife.play();
				}
				brickCollectionDetect(listOfBrick, listOfBurningBrick, listOfPowerUp,ball, clockForBurning,&score,&accumulateNumber);
				if (listOfBurningBrick.empty()) {
					burnOut = true;
				}
				else {
					burnOut = false;
					if (clockForBurning.getElapsedTime().asSeconds() >= 0.02) {
						burnBrick(listOfBrick, listOfBurningBrick, listOfPowerUp,clockForBurning, &score,&accumulateNumber);
						soundOfBurnBrick.play();
					}
				}
				powerUpCheck(listOfPowerUp,&score,paddle,timePerFrame);
				window.clear();
				normalBrickExist = false;
				std::list<Brick>::iterator i;
				for (i = listOfBrick.begin(); i != listOfBrick.end(); i++) {
					if (i->brickType == BrickType::burn || i->brickType == BrickType::normal || i->brickType == BrickType::twice || (i->brickType == BrickType::invisible && i->hitTimes == 1)) {
						normalBrickExist = true;
					}
					window.draw(*i);
				}
				if (!normalBrickExist && burnOut) {
					soundOfLevelClear.play();
					loadNewLevel = true;
					newLifeStart = false;
					ballStick = true;
					listOfPowerUp.clear();
					ball.pointerOfObjectLastHit = nullptr;
					sf::Clock temclock;
					while (temclock.getElapsedTime().asSeconds() < 1) {

					}
				}
				std::list<PowerUp>::iterator j;
				paddle.extraLength = 0;
				ballSpeedCoefficient = 0;
				for (j = listOfPowerUp.begin(); j != listOfPowerUp.end(); j++) {
					if (!j->getByPlayer) {
						window.draw(*j);
					}
					else {
						if (j->powerUpType == PowerUpType::longerPaddle) {
							paddle.extraLength < 200 ? paddle.extraLength += 50 : NULL;
						}
						if (j->powerUpType == PowerUpType::slowerBall) {
							ballSpeedCoefficient < 0.6 ? ballSpeedCoefficient += 0.2 : NULL;
						}
					}
				}
				paddle.setSize(sf::Vector2f(paddle.basicLength + paddle.extraLength, paddle.height));
				ball.move(ball.velocityPerSecond.x* timePerFrame* (1 - ballSpeedCoefficient), ball.velocityPerSecond.y* timePerFrame* (1 - ballSpeedCoefficient));
	
				for (int i = 0; i < healthPoint-1; i++) {
					window.draw(lifePaddle[i]);
				}
				window.draw(scoreBoard);
				window.draw(paddle);
				window.draw(ball);
				window.display();
			}
		}

		else {
			TextForPong startGame(std::string("Start"), FONTSIZEOFSCOREBOARD, sf::Color::White, sf::Vector2f(LENGTHOFTHEWINDOW / 2, HEIGHTHTOFTHEWINDOW - 12 * WIDTHOFTHEINTERVAL), AlignOfText::center);
			TextForPong clickToStart(std::string("Click or press Space to start"), FONTSIZEOFPOWERUP, sf::Color::White, sf::Vector2f(LENGTHOFTHEWINDOW / 2, HEIGHTHTOFTHEWINDOW - 7 * WIDTHOFTHEINTERVAL), AlignOfText::center);
			TextForPong scoreOfLastGame(std::string("Score of last Game:")+std::to_string(score), 1.2*FONTSIZEOFSCOREBOARD, sf::Color::Color(60, 138, 255, 255), sf::Vector2f(LENGTHOFTHEWINDOW/2, HEIGHTHTOFTHEWINDOW/4), AlignOfText::center);
			window.clear();
			window.draw(startGame);
			window.draw(clickToStart);
			if (score != 0) {
				window.draw(scoreOfLastGame);
			}
			window.display();
			sf::Event event;
			while (window.pollEvent(event))
			{
				if (event.type == sf::Event::Closed)
					window.close();
				if (event.type == sf::Event::MouseButtonReleased) {
					if (event.mouseButton.button == sf::Mouse::Left) {
						if (startGame.getGlobalBounds().contains(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y)) {
							gameStart = true;
							currentScore = 0;
							score = 0;
							levelNumber = 0;
							loadNewLevel = true;
							ball.basicalSpeed = SPEEDOFBALLPERSECOND;
							mousePosition = sf::Mouse::getPosition(window);
						}
					}
				}
				if (event.type == sf::Event::KeyReleased) {
					if (event.key.code == sf::Keyboard::Space) {
						gameStart = true;
						currentScore = 0;
						score = 0;
						levelNumber = 0;
						loadNewLevel = true;
						ball.basicalSpeed = SPEEDOFBALLPERSECOND;
						mousePosition = sf::Mouse::getPosition(window);
					}
				}
			}
		}
	}

	return 0;

}

