// Jong Hoon Kim
// CS32 - Project 3

#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include <string>
#include <vector>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class Actor;
class Level;
class Penelope;

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    virtual ~StudentWorld();
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    
    bool makeLevel(int level);
    bool canIMove(Actor* thisActor, double destX, double destY) const;
    bool getOverlaps(std::vector<Actor*> &actors, Actor *currentActor) const;
    bool getCitizen(std::vector<Actor*> &citizens) const;
    bool getZombie(std::vector<Actor*> &zombies) const;
    int  getCitizenCount() const;
    bool getPenelope(Penelope*& player) const;
    
    void addActor(Actor *newActor);
    void decreaseCitizen();
    void makeStatus();
private:
    Penelope *m_player;
    std::vector<Actor*> m_actors;
    std::vector<std::string> m_leveltxt;

    int m_citizenCount;
};

#endif // STUDENTWORLD_H_
