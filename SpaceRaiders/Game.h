// SpaceRaiders.cpp : Defines the entry point for the console application.
//
#include "Vector2D.h"
#include "Random.h"
#include <random>


struct GameConfig;
class PlayField;
class MessageLog;
struct AlienWave;
struct WallInfo;
struct BossInfo;
struct Event;


class Game
{
public:

	Game(PlayField& world, const GameConfig& config, std::default_random_engine& rGen);

	enum class State
	{
		start,
		intro,
		running,
		paused,
		over,
		victory,
		quit
	};

	int GetScore(int playerIndex) const;
	int GetNumPlayers() const;
	State GetState() const { return state; }
	void Update(float dt, MessageLog& messageLog);

private:

	enum class Mode
	{
		p1,
		cpu1,
		p1p2,
		p1cpu2,
		cpu1cpu2
	};

	// States
	void StartMenu();
	void Intro(float dt);
	void Run(float dt, MessageLog& messageLog);
	void VictoryMenu(float dt);
	void GameOverMenu();
	void PauseMenu();

	void Start(Mode mode);
	void CreatePlayers(Mode mode);
	void ProcessEvent(const Event& event, MessageLog& messageLog);
	void SpawnAlienWave(const AlienWave& wave);
	void SpawnWall(const WallInfo& wall);
	void SpawnBoss(const BossInfo& boss);
	void NextLevel(MessageLog& messageLog);

private:

	static constexpr int maxPlayers = 2;

	PlayField&                  world;
	const GameConfig&           config;
	std::default_random_engine& rGen;

	State state;
	int   score[maxPlayers];
	int   eventIndex;
	int   wallRow;
	int   numPlayers;
	int   level;
	float accumTime;
};
