// Jong Hoon Kim
// CS32 - Project 3

#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
class StudentWorld;
// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp
// base class for all actor
class Actor: public GraphObject {
    
public:
    Actor(StudentWorld *thisWorld, int imageID, double startX, double startY, bool infectable = false, bool passable = false, bool killable = true, bool flamePassable = true, Direction dir = right, int depth = 0, double size = 1.0);
    virtual ~Actor();
    
    
    virtual bool doSomething() = 0;
    bool isAlive() const;
    virtual void kill();
    StudentWorld* getWorld() const;
    void tryMoving(int keystroke);
    bool isPassable() const;
    bool isInfectable() const;
    virtual bool isCitizen() const;
    virtual bool isPenelope() const;
    virtual bool isExit() const;
    virtual bool isMovingObject() const;
    virtual bool scareHuman() const;
    double getDistance(Actor *other) const;
    bool triggerOverlap(Actor *other) const;
    virtual bool burn();
    bool isFlamePassable() const;
    virtual bool infect();
    
    
private:
    StudentWorld *thisWorld;
    bool m_alive;
    bool m_infectable;
    bool m_passable;
    bool m_killable;
    bool m_flamePassable;
};



class MovingActor: public Actor {
public:
    MovingActor(StudentWorld *thisWorld, int imageID, double startX, double startY, bool infectable = false, Direction dir = right);
    virtual ~MovingActor();
    virtual bool doSomething();
    virtual bool isMovingObject() const;
    virtual bool isParalyzed() const;
    virtual bool moveX(double dist);
    virtual bool moveY(double dist);
private:
    bool m_paralyzed;
};


// Human Class for Penelope and Citizen
class Human: public MovingActor {
    
public:
    Human(StudentWorld *thisWorld, int imageID, double startX, double startY, Direction dir = right);
    virtual ~Human();
    virtual bool doSomething();
    virtual void becomeZombie() = 0;
    bool changeInfection(int rate);
    int getInfectionCount() const;
    virtual bool infect();
    void cure();
    
private:
    bool m_infectionStatus;
    int m_infectionCount;
};



// Penelope class
class Penelope: public Human {
    
public:
    Penelope(StudentWorld *thisWorld, double startX, double startY, Direction dir = right);
    virtual ~Penelope();
    
    
    virtual bool doSomething();
    virtual void shootFlames() const;
    virtual int getLandmines() const;
    virtual int getFlamethrowers() const;
    virtual int getVaccines() const;
    virtual bool isPenelope() const;
    virtual void becomeZombie();

    virtual void increaseLandmines(int count);
    virtual void increaseFlamethrowers(int count);
    virtual void increaseVaccines(int count);
    
private:
    int m_landmines;
    int m_flamethrowers;
    int m_vaccines;
};



// Wall class for now
class Wall: public Actor {
    
public:
    Wall(StudentWorld *thisWorld, double startX, double startY);
    virtual ~Wall();
    
    virtual bool doSomething();
    
private:
    // nothing so far
};



class Item: public Actor {
    
public:
    Item(StudentWorld *thisWorld, int imageID, double startX, double startY, bool killable = true, bool flamePassable = true, Direction dir = right, int depth = 1);
    virtual ~Item();
    virtual bool doSomething();
private:
    virtual bool applyEffect(Actor *target) = 0;
};


class Exit: public Item {
    
public:
    Exit(StudentWorld *thisWorld, double startX, double startY);
    virtual ~Exit();
    virtual bool isExit() const;
private:
    virtual bool applyEffect(Actor *target);
};


class Goodie: public Item {
public:
    Goodie(StudentWorld *thisWorld, int imageID, double startX, double startY, Direction dir = right);
    virtual ~Goodie();
private:
    virtual bool applyEffect(Actor *target);
    virtual void increaseGoodieCounter(Penelope *player) = 0;
};



class VaccineGoodie: public Goodie {
public:
    VaccineGoodie(StudentWorld *thisWorld, double startX, double startY, Direction dir = right);
    virtual ~VaccineGoodie();
private:
    virtual void increaseGoodieCounter(Penelope *player);
};



class GasCanGoodie: public Goodie {
public:
    GasCanGoodie(StudentWorld *thisWorld, double startX, double startY, Direction dir = right);
    virtual ~GasCanGoodie();
private:
    virtual void increaseGoodieCounter(Penelope *player);
};



class LandmineGoodie: public Goodie {
public:
    LandmineGoodie(StudentWorld *thisWorld, double startX, double startY, Direction dir = right);
    virtual ~LandmineGoodie();
private:
    virtual void increaseGoodieCounter(Penelope *player);
};

class DeadlyObject: public Item {
public:
    DeadlyObject(StudentWorld *thisWorld, int imageID, double startX, double startY, bool flamePassable, Direction dir);
    virtual ~DeadlyObject();
private:
    virtual bool applyEffect(Actor *target);
};


class Projectile: public DeadlyObject {
public:
    Projectile(StudentWorld *thisWorld, int imageID, double startX, double startY, bool flamePassable, Direction dir);
    virtual ~Projectile();
    virtual bool doSomething();
private:
    int lifeCount;
};


class Flame: public Projectile {
public:
    Flame(StudentWorld *thisWorld, double startX, double startY, Direction dir);
    virtual ~Flame();
};


class Pit: public DeadlyObject {
public:
    Pit(StudentWorld *thisWorld, double startX, double startY);
    virtual ~Pit();
};

class Vomit: public Projectile {
public:
    Vomit(StudentWorld *thisWorld, double startX, double startY, Direction dir);
    virtual ~Vomit();
private:
    virtual bool applyEffect(Actor *target);
};


class Landmine: public Item {
public:
    Landmine(StudentWorld *thisWorld, double startX, double startY);
    virtual ~Landmine();
    virtual bool doSomething();
    virtual void kill();
    virtual void explode() const;
private:
    virtual bool applyEffect(Actor *target);
    bool active;
    int safetyTick;
};


class Zombie: public MovingActor {
public:
    Zombie(StudentWorld *thisWorld, double startX, double startY, Direction dir = 0);
    virtual ~Zombie();
    virtual bool doSomething();
    virtual bool scareHuman() const;
    virtual void kill();
    virtual void makePlan();
    virtual void makeDirection();
    virtual bool canVomit() const;
    virtual int getMovement() const;
    virtual void moveFoward();
    virtual int killPoints() const = 0;
    
private:
    int m_movement;
};


class DumbZombie: public Zombie {
public:
    DumbZombie(StudentWorld *thisWorld, double startX, double startY, Direction dir = 0);
    virtual ~DumbZombie();
    virtual void kill();
    virtual int killPoints() const;
};


class SmartZombie: public Zombie {
public:
    SmartZombie(StudentWorld *thisWorld, double startX, double startY, Direction dir = 0);
    virtual ~SmartZombie();
    virtual void kill();
    virtual void makeDirection();
    virtual int killPoints() const;
};

class Citizen: public Human {
public:
    Citizen(StudentWorld *thisWorld, double startX, double startY, Direction dir = right);
    virtual ~Citizen();
    virtual bool doSomething();
    virtual void kill();
    virtual void save();
    virtual bool isCitizen() const;
private:
    virtual void citizenDied(int sound = SOUND_CITIZEN_DIE, int score = -1000);
    virtual void becomeZombie();
    virtual void optimalRun(Actor *zombie);
    virtual void optimalFollow(Actor *player);
};

#endif // ACTOR_H_
