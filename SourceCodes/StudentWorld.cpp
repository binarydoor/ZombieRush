// Jong Hoon Kim
// CS32 - Project 3

#include "StudentWorld.h"
#include "GameConstants.h"
#include "Level.h"
#include "Actor.h"
#include <cstdlib>
#include <string>
#include <iomanip>
#include <sstream>
using namespace std;

int LEVEL_COUNT = 6;

GameWorld* createStudentWorld(string assetPath)
{
    return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
:GameWorld(assetPath),
m_player(nullptr),
m_citizenCount(0)
{
    // Store the name of level text files in vector up to LEVEL_COUNT
    string temp;
    for (int i = 1; i <= LEVEL_COUNT; ++i) {
        temp = "level";
        temp += char(i / 10 + 48);
        temp += char(i + 48);
        temp += ".txt";
        m_leveltxt.push_back(temp);
    }
}

StudentWorld::~StudentWorld()
{
    cleanUp();
}

int StudentWorld::init()
{
    int currentLevel = getLevel();
    if (currentLevel > LEVEL_COUNT || currentLevel > 99)
        return GWSTATUS_PLAYER_WON;
    else if (makeLevel(currentLevel)) {
        makeStatus();
        return GWSTATUS_CONTINUE_GAME;
    }
    else return GWSTATUS_LEVEL_ERROR;
}

int StudentWorld::move()
{
    if (m_player->doSomething()) {
        playSound(SOUND_PLAYER_DIE);
        decLives();
        return GWSTATUS_PLAYER_DIED;
    }
    auto it = m_actors.begin();
    for(; it != m_actors.end(); ++it) {
        if ((*it)->doSomething()) {
            if ((*it)->isExit()) {
                if (m_citizenCount == 0) {
                    playSound(SOUND_LEVEL_FINISHED);
                    return GWSTATUS_FINISHED_LEVEL;
                }
            }
            else {
                playSound(SOUND_PLAYER_DIE);
                decLives();
                return GWSTATUS_PLAYER_DIED;
            }
        }
    }
    it = m_actors.begin();
    while (it != m_actors.end()) {
        if (!(*it)->isAlive()) {
            delete (*it);
            it = m_actors.erase(it);
        }
        else ++it;
    }
    // This code is here merely to allow the game to build, run, and terminate after you hit enter.
    // Notice that the return value GWSTATUS_PLAYER_DIED will cause our framework to end the current level.
    //decLives();
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    delete m_player;
    m_player = nullptr;
    for (auto it = m_actors.begin(); it != m_actors.end(); ++it)
        delete (*it);
    m_actors.clear();
}


bool StudentWorld::makeLevel(int level)
{
    Level newLevel(assetPath());
    Level::LoadResult result = newLevel.loadLevel(m_leveltxt[level - 1]);
    if (result == Level::load_fail_file_not_found) {
        cerr << "Cannot find " << m_leveltxt[level - 1] << " data file \n";
        return false;
    }
    else if (result == Level::load_fail_bad_format) {
        cerr << "Your level was improperly formatted\n";
        return false;
    }
    else if (result == Level::load_success) {
        for (int x = 0; x < VIEW_WIDTH; ++x) {
            for (int y = 0; y < VIEW_HEIGHT; ++y) {
                Level::MazeEntry newCharacter = newLevel.getContentsOf(x, y);
                switch (newCharacter) {
                    case Level::empty:
                        // do nothing
                        break;
                    case Level::smart_zombie:
                        m_actors.push_back(new SmartZombie(this, x * SPRITE_WIDTH, y * SPRITE_HEIGHT));
                        break;
                    case Level::dumb_zombie:
                        m_actors.push_back(new DumbZombie(this, x * SPRITE_WIDTH, y * SPRITE_HEIGHT));
                        break;
                    case Level::player:
                        m_player = new Penelope(this, x * SPRITE_WIDTH, y * SPRITE_HEIGHT);
                        break;
                    case Level::exit:
                        m_actors.push_back(new Exit(this, x * SPRITE_WIDTH, y * SPRITE_HEIGHT));
                        break;
                    case Level::wall:
                        m_actors.push_back(new Wall(this, x * SPRITE_WIDTH, y * SPRITE_HEIGHT));
                        break;
                    case Level::pit:
                        m_actors.push_back(new Pit(this, x * SPRITE_WIDTH, y * SPRITE_HEIGHT));
                        break;
                    case Level::citizen:
                        ++m_citizenCount;
                        m_actors.push_back(new Citizen(this, x * SPRITE_WIDTH, y * SPRITE_HEIGHT));
                        break;
                    case Level::vaccine_goodie:
                        m_actors.push_back(new VaccineGoodie(this, x * SPRITE_WIDTH, y * SPRITE_HEIGHT));
                        break;
                    case Level::gas_can_goodie:
                        m_actors.push_back(new GasCanGoodie(this, x * SPRITE_WIDTH, y * SPRITE_HEIGHT));
                        break;
                    case Level::landmine_goodie:
                        m_actors.push_back(new LandmineGoodie(this, x * SPRITE_WIDTH, y * SPRITE_HEIGHT));
                        break;
                }
            }
        }
    }
    
    return true;
}



bool StudentWorld::canIMove(Actor* thisActor, double destX, double destY) const
{
    if (thisActor != m_player && abs(destX - m_player->getX()) <= 15 && abs(destY - m_player->getY()) <= 15)
        return false;
    for (auto it = m_actors.begin(); it != m_actors.end(); ++it)
        if ((*it) != thisActor && abs((*it)->getX() - destX) <= 15 && abs((*it)->getY() - destY) <= 15)
            if (!(*it)->isPassable())
                return false;
    
    
    return true;
}


bool StudentWorld::getOverlaps(std::vector<Actor*> &actors, Actor *currentActor) const
{
    if (currentActor != m_player && currentActor->triggerOverlap(m_player))
        actors.push_back(m_player);
    for (auto it = m_actors.begin(); it != m_actors.end(); ++it)
        if (currentActor != m_player && currentActor->triggerOverlap(*it))
            actors.push_back(*it);
    return actors.size() > 0;
}


bool StudentWorld::getCitizen(std::vector<Actor*> &citizens) const
{
    if (m_citizenCount == 0) return false;
    else {
        for (auto it = m_actors.begin(); it != m_actors.end(); ++it) {
            if ((*it)->isCitizen())
                citizens.push_back(*it);
        }
        return true;
    }
}


bool StudentWorld::getZombie(std::vector<Actor*> &zombies) const
{
    for (auto it = m_actors.begin(); it != m_actors.end(); ++it)
        if ((*it)->scareHuman())
            zombies.push_back(*it);
    return zombies.size() != 0;
}


int StudentWorld::getCitizenCount() const
{ return m_citizenCount; }



bool StudentWorld::getPenelope(Penelope*& player) const
{
    if (m_player != nullptr) {
        player = m_player;
        return true;
    }
    return false;
}



void StudentWorld::addActor(Actor *newActor)
{ m_actors.push_back(newActor); }


void StudentWorld::decreaseCitizen()
{ --m_citizenCount; }


void StudentWorld::makeStatus()
{
    ostringstream oss;
    oss << "Score: " << setw(6) << setfill('0') << getScore()
        << "  Level: "    << getLevel()
        << "  Lives: "    << getLives()
        << "  Vaccines: " << m_player->getVaccines()
        << "  Flames: "   << m_player->getFlamethrowers()
        << "  Mines: "    << m_player->getLandmines()
        << "  Infected: " << m_player->getInfectionCount();
    
    setGameStatText(oss.str());
}
