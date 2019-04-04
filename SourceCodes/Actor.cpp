// Jong Hoon Kim
// CS32 - Project 3

#include "Actor.h"
#include "StudentWorld.h"
#include <cstdlib>
#include <cmath>

using namespace std;

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp

// Actor Class
Actor::Actor(StudentWorld *thisWorld, int imageID, double startX, double startY, bool infectable, bool passable, bool killable, bool flamePassable, Direction dir, int depth, double size)
:GraphObject(imageID, startX, startY, dir, depth, size),
thisWorld(thisWorld),
m_alive(true),
m_infectable(infectable),
m_passable(passable),
m_killable(killable),
m_flamePassable(flamePassable)
{
    
}


Actor::~Actor()
{
    
}



bool Actor::isAlive() const
{ return m_alive; }



void Actor::kill()
{
    if (m_alive) m_alive = false;
}



void Actor::tryMoving(Direction dir)
{
    setDirection(dir);
    int temptedX = getX();
    int temptedY = getY();
    if (dir == GraphObject::right ) {
        temptedX += 4;
        if (temptedX > VIEW_WIDTH - 4)
            temptedX = VIEW_WIDTH - 4;
    }
    else if (dir == GraphObject::left) {
        temptedX -= 4;
        if (temptedX < 0)
            temptedX = 0;
    }
    else if (dir == GraphObject::up) {
        temptedY += 4;
        if (temptedY > VIEW_HEIGHT - 4)
            temptedY = VIEW_HEIGHT - 4;
    }
    else {
        temptedY -= 4;
        if (temptedY < 0)
            temptedY = 0;
    }
    if (thisWorld->canIMove(this, temptedX, temptedY))
        moveTo(temptedX, temptedY);
    
    
}


bool Actor::isPassable() const
{ return m_passable; }


bool Actor::isInfectable() const
{ return m_infectable; }


bool Actor::isCitizen() const
{ return false; }


bool Actor::isPenelope() const
{ return false; }


bool Actor::isExit() const
{ return false; }


bool Actor::isMovingObject() const
{ return false; }


bool Actor::scareHuman() const
{ return false; }


StudentWorld* Actor::getWorld() const
{ return thisWorld; }



double Actor::getDistance(Actor *other) const
{
    double xdiff = getX() - other->getX();
    double ydiff = getY() - other->getY();
    return sqrt((xdiff * xdiff) + (ydiff * ydiff));
}


bool Actor::triggerOverlap(Actor *other) const
{
    return (getDistance(other) <= 10);
}




bool Actor::burn()
{
    if (m_killable) kill();
    return false;
}



bool Actor::isFlamePassable() const
{ return m_flamePassable; }



bool Actor::infect()
{ return false; }


MovingActor::MovingActor(StudentWorld *thisWorld, int imageID, double startX, double startY, bool infectable, Direction dir)
: Actor(thisWorld, imageID, startX, startY, infectable, false, true, true, dir),
m_paralyzed(false)
{
    
}


MovingActor::~MovingActor()
{
    
}



bool MovingActor::doSomething()
{
    if (m_paralyzed) m_paralyzed = false;
    else m_paralyzed = true;
    return false;
}

bool MovingActor::isMovingObject() const
{ return true; }




bool MovingActor::isParalyzed() const
{ return m_paralyzed; }

bool MovingActor::moveX(double dist)
{
    if (getWorld()->canIMove(this, getX() + dist, getY())) {
        Direction dir = dist < 0 ? left : right;
        setDirection(dir);
        moveTo(getX() + dist, getY());
        return true;
    }
    return false;
}

bool MovingActor::moveY(double dist)
{
    if (getWorld()->canIMove(this, getX(), getY() + dist)) {
        Direction dir = dist < 0 ? down : up;
        setDirection(dir);
        moveTo(getX(), getY() + dist);
        return true;
    }
    return false;
}




// Human Class
Human::Human(StudentWorld *thisWorld, int imageID, double startX, double startY, Direction dir)
:MovingActor(thisWorld, imageID, startX, startY, true, dir),
m_infectionStatus(false),
m_infectionCount(0)
{
    
}



Human::~Human()
{
    
}


bool Human::doSomething()
{
    if (m_infectionStatus) {
        if (changeInfection(1)) {
            becomeZombie();
            return true;
        }
        getWorld()->makeStatus();
    }
    else if (m_infectionCount > 0) {
        changeInfection(-1);
        getWorld()->makeStatus();
    }
    return false;
}






bool Human::changeInfection(int rate)
{
    // change the infection count by passed rate
    m_infectionCount += rate;
    
    // if the human reaches 500 infection count, return true as dead
    if (m_infectionCount >= 500) return true;
    
    // if the human reaches negative infection count, set it to 0
    if (m_infectionCount < 0) m_infectionCount = 0;
    
    // return false as not being dead
    return false;
}


int Human::getInfectionCount() const
{ return m_infectionCount; }



bool Human::infect()
{
    if (!m_infectionStatus) {
        m_infectionStatus = true;
        return true;
    }
    return false;
}


void Human::cure()
{ m_infectionStatus = false; }




// Penelope
Penelope::Penelope(StudentWorld *thisWorld, double startX, double startY, Direction dir)
:Human(thisWorld, IID_PLAYER, startX, startY, dir),
m_landmines(0),
m_flamethrowers(0),
m_vaccines(0)
{
    
}



Penelope::~Penelope()
{
    
}



bool Penelope::doSomething()
{
    // if player is dead, do nothing
    if (isAlive()) {
        if (Human::doSomething()) return true;
        
        int keystroke;
        if (getWorld()->getKey(keystroke)) {
            switch (keystroke) {
                case KEY_PRESS_SPACE:
                    if (m_flamethrowers > 0) {
                        --m_flamethrowers;
                        getWorld()->makeStatus();
                        getWorld()->playSound(SOUND_PLAYER_FIRE);
                        shootFlames();
                    }
                    break;
                case KEY_PRESS_TAB:
                    if (m_landmines > 0) {
                        --m_landmines;
                        getWorld()->makeStatus();
                        getWorld()->addActor(new Landmine(getWorld(), getX(), getY()));
                    }
                    break;
                case KEY_PRESS_ENTER:
                    if (m_vaccines > 0) {
                        --m_vaccines;
                        getWorld()->makeStatus();
                        cure();
                        changeInfection(-1);
                    }
                    break;
                case KEY_PRESS_UP:
                    tryMoving(GraphObject::up);
                    break;
                case KEY_PRESS_DOWN:
                    tryMoving(GraphObject::down);
                    break;
                case KEY_PRESS_LEFT:
                    tryMoving(GraphObject::left);
                    break;
                case KEY_PRESS_RIGHT:
                    tryMoving(GraphObject::right);
                    break;
                    
                default:
                    break;
            }
        }
        return false;
    }
    else return false;
}


void Penelope::shootFlames() const
{
    double x = getX();
    double y = getY();
    Direction dir = getDirection();
    StudentWorld *thisWorld = getWorld();
    vector<Actor*> overlaps;
    Flame *newFlame;
    for (int i = 1; i <= 3; ++i) {
        if (dir == up)        newFlame = new Flame(thisWorld, x, y + (i * SPRITE_HEIGHT), dir);
        else if (dir == down) newFlame = new Flame(thisWorld, x, y - (i * SPRITE_HEIGHT), dir);
        else if (dir == left) newFlame = new Flame(thisWorld, x - (i * SPRITE_WIDTH), y, dir);
        else                  newFlame = new Flame(thisWorld, x + (i * SPRITE_WIDTH), y, dir);
        if (thisWorld->getOverlaps(overlaps, newFlame)) {
            for (auto it = overlaps.begin(); it != overlaps.end(); ++it) {
                if (!(*it)->isFlamePassable()) {
                    delete newFlame;
                    return ;
                }
            }
        }
        thisWorld->addActor(newFlame);
    }
}


int Penelope::getLandmines() const
{ return m_landmines; }




int Penelope::getFlamethrowers() const
{ return m_flamethrowers; }




int Penelope::getVaccines() const
{ return m_vaccines; }


bool Penelope::isPenelope() const
{ return true; }


void Penelope::becomeZombie()
{
    kill();
}


void Penelope::increaseLandmines(int count)
{
    m_landmines += ((m_landmines + count) > 0 ? count : 0);
}



void Penelope::increaseFlamethrowers(int count)
{
    m_flamethrowers += ((m_flamethrowers + count) > 0 ? count : 0);
}



void Penelope::increaseVaccines(int count)
{
    m_vaccines += ((m_vaccines + count) > 0 ? count : 0);
}



Wall::Wall(StudentWorld *thisWorld, double startX, double startY)
:Actor(thisWorld, IID_WALL, startX, startY, false, false, false, false)
{
    
}


Wall::~Wall()
{
    
}


bool Wall::doSomething()
{
    return false;
}



// Item

Item::Item(StudentWorld *thisWorld, int imageID, double startX, double startY, bool killable, bool flamePassable, Direction dir, int depth)
:Actor(thisWorld, imageID, startX, startY, false, true, killable, flamePassable, dir, depth)
{
    
}

Item::~Item()
{
    // nothing for now
}


bool Item::doSomething()
{
    if (!isAlive()) return false;
    vector<Actor*> overlappingActors;
    if (getWorld()->getOverlaps(overlappingActors, this))
        for (auto it = overlappingActors.begin(); isAlive() && it != overlappingActors.end(); ++it)
            if (applyEffect(*it))
                return true;
    
    return false;
}


Exit::Exit(StudentWorld *thisWorld, double startX, double startY)
:Item(thisWorld, IID_EXIT, startX, startY, false, false)
{
    // nothing for now
}

Exit::~Exit()
{
    // nothing for now
}

bool Exit::isExit() const
{ return true; }


bool Exit::applyEffect(Actor *target)
{
    StudentWorld *thisWorld = getWorld();
    Penelope *player;
    thisWorld->getPenelope(player);
    if (target->isCitizen() && target->isAlive()) {
        Citizen *citizen = static_cast<Citizen*>(target);
        citizen->save();
    }
    else if (target == player && thisWorld->getCitizenCount() == 0) {
        target->kill();
        return true;
    }
    return false;
}


Goodie::Goodie(StudentWorld *thisWorld, int imageID, double startX, double startY, Direction dir)
: Item(thisWorld, imageID, startX, startY, true, true, dir)
{
    
}

Goodie::~Goodie()
{
    
}

bool Goodie::applyEffect(Actor *target)
{
    if (target->isPenelope()) {
        StudentWorld *thisWorld = getWorld();
        Penelope *player;
        thisWorld->getPenelope(player);
        thisWorld->playSound(SOUND_GOT_GOODIE);
        thisWorld->increaseScore(50);
        increaseGoodieCounter(player);
        thisWorld->makeStatus();
        kill();
    }
    return false;
}


VaccineGoodie::VaccineGoodie(StudentWorld *thisWorld, double startX, double startY, Direction dir)
: Goodie(thisWorld, IID_VACCINE_GOODIE, startX, startY, dir)
{
    
}

VaccineGoodie::~VaccineGoodie()
{
    
}


void VaccineGoodie::increaseGoodieCounter(Penelope *player)
{ player->increaseVaccines(1); }



GasCanGoodie::GasCanGoodie(StudentWorld *thisWorld, double startX, double startY, Direction dir)
: Goodie(thisWorld, IID_GAS_CAN_GOODIE, startX, startY, dir)
{
    
}


GasCanGoodie::~GasCanGoodie()
{
    
}


void GasCanGoodie::increaseGoodieCounter(Penelope *player)
{ player->increaseFlamethrowers(5); }




LandmineGoodie::LandmineGoodie(StudentWorld *thisWorld, double startX, double startY, Direction dir)
: Goodie(thisWorld, IID_LANDMINE_GOODIE, startX, startY, dir)
{
    
}


LandmineGoodie::~LandmineGoodie()
{
    
}


void LandmineGoodie::increaseGoodieCounter(Penelope *player)
{ player->increaseLandmines(2); }


DeadlyObject::DeadlyObject(StudentWorld *thisWorld, int imageID, double startX, double startY, bool flamePassable, Direction dir)
:Item(thisWorld, imageID, startX, startY, false, flamePassable, dir, 0)
{
    
}

DeadlyObject::~DeadlyObject()
{
    
}

bool DeadlyObject::applyEffect(Actor *target)
{ return target->burn(); }


Projectile::Projectile(StudentWorld *thisWorld, int imageID, double startX, double startY, bool flamePassable, Direction dir)
:DeadlyObject(thisWorld, imageID, startX, startY, flamePassable, dir),
lifeCount(2)
{
    
}

Projectile::~Projectile()
{
    
}

bool Projectile::doSomething()
{
    if (lifeCount == 0) kill();
    else --lifeCount;
    
    return Item::doSomething();
}

Flame::Flame(StudentWorld *thisWorld, double startX, double startY, Direction dir)
:Projectile(thisWorld, IID_FLAME, startX, startY, false, dir)
{
    
}


Flame::~Flame()
{
    
}



Pit::Pit(StudentWorld *thisWorld, double startX, double startY)
:DeadlyObject(thisWorld, IID_PIT, startX, startY, true, right)
{
    
}

Pit::~Pit()
{
    
}


Vomit::Vomit(StudentWorld *thisWorld, double startX, double startY, Direction dir)
:Projectile(thisWorld, IID_VOMIT, startX, startY, true, dir)
{
    
}


Vomit::~Vomit()
{
    
}


bool Vomit::applyEffect(Actor *target)
{
    if (target->isInfectable()) {
        if (target->infect()) {
            Penelope *player;
            getWorld()->getPenelope(player);
            if (target != player)
                getWorld()->playSound(SOUND_CITIZEN_INFECTED);
        }
    }
    return false;
}

Landmine::Landmine(StudentWorld *thisWorld, double startX, double startY)
:Item(thisWorld, IID_LANDMINE, startX, startY),
active(false),
safetyTick(30)
{
    
}

Landmine::~Landmine()
{
    
}

bool Landmine::doSomething()
{
    if (isAlive()) {
        if (active) return Item::doSomething();
        --safetyTick;
        if (safetyTick == 0) active = true;
    }
    return false;
}


void Landmine::kill()
{
    if (isAlive()) {
        getWorld()->playSound(SOUND_LANDMINE_EXPLODE);
        Actor::kill();
        explode();
    }
}


void Landmine::explode() const
{
    double x = getX() - SPRITE_WIDTH;
    double y = getY() - SPRITE_HEIGHT;
    StudentWorld *thisWorld = getWorld();
    vector<Actor*> overlaps;
    bool valid;
    Flame *newFlame;
    thisWorld->addActor(new Pit(thisWorld, getX(), getY()));
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            overlaps.clear();
            newFlame = new Flame(thisWorld, x + (i * SPRITE_WIDTH), y + (j * SPRITE_HEIGHT), up);
            valid = true;
            if (thisWorld->getOverlaps(overlaps, newFlame))
                for (auto it = overlaps.begin(); it != overlaps.end(); ++it)
                    if (!(*it)->isFlamePassable())
                        valid = false;
            if (valid) thisWorld->addActor(newFlame);
            else delete newFlame;
        }
    }
}


bool Landmine::applyEffect(Actor *target)
{
    if (target->isMovingObject()) kill();
    return false;
}


Zombie::Zombie(StudentWorld *thisWorld, double startX, double startY, Direction dir)
:MovingActor(thisWorld, IID_ZOMBIE, startX, startY, false, dir),
m_movement(0)
{

}

Zombie::~Zombie()
{
    
}

bool Zombie::doSomething()
{
    if (isAlive()) {
        if (!isParalyzed()) {
            if (randInt(1, 3) == 3) canVomit();
            makePlan();
            makeDirection();
            moveFoward();
        }
        
        MovingActor::doSomething();
    }
    return false;
}

bool Zombie::scareHuman() const
{ return true; }


void Zombie::kill()
{
    getWorld()->playSound(SOUND_ZOMBIE_DIE);
    getWorld()->increaseScore(killPoints());
    getWorld()->makeStatus();
    Actor::kill();
}


void Zombie::makePlan()
{ m_movement = randInt(3, 10); }

void Zombie::makeDirection()
{
    int randomNumber = randInt(1, 4);
    if (randomNumber == 1)      setDirection(up);
    else if (randomNumber == 2) setDirection(down);
    else if (randomNumber == 3) setDirection(left);
    else                        setDirection(right);
}


bool Zombie::canVomit() const
{
    double x = getX();
    double y = getY();
    Direction dir = getDirection();
    if      (dir == up)   y += SPRITE_HEIGHT;
    else if (dir == down) y -= SPRITE_HEIGHT;
    else if (dir == left) x -= SPRITE_WIDTH;
    else                  x += SPRITE_WIDTH;
    Vomit *newVomit = new Vomit(getWorld(), x, y, dir);
    Penelope *player;
    StudentWorld *thisWorld = getWorld();
    vector<Actor*> citizens;
    if (thisWorld->getPenelope(player)) {
        if (newVomit->triggerOverlap(player)) {
            thisWorld->playSound(SOUND_ZOMBIE_VOMIT);
            thisWorld->addActor(newVomit);
            return true;
        }
    }
    if (getWorld()->getCitizen(citizens)) {
        for (auto it = citizens.begin(); it != citizens.end(); ++it) {
            if (newVomit->triggerOverlap(*it)) {
                thisWorld->playSound(SOUND_ZOMBIE_VOMIT);
                thisWorld->addActor(newVomit);
                return true;
            }
        }
    }
    delete newVomit;
    return false;
}


int Zombie::getMovement() const
{ return m_movement; }


void Zombie::moveFoward()
{
    double x = getX();
    double y = getY();
    Direction dir = getDirection();
    if      (dir == up)   y += 1;
    else if (dir == down) y -= 1;
    else if (dir == left) x -= 1;
    else                  x += 1;
    if (getWorld()->canIMove(this, x, y))
        moveTo(x, y);
    else m_movement = 0;
}


DumbZombie::DumbZombie(StudentWorld *thisWorld, double startX, double startY, Direction dir)
:Zombie(thisWorld, startX, startY, dir)
{
    
}

DumbZombie::~DumbZombie()
{
    
}


void DumbZombie::kill()
{
    if (isAlive()) {
        if (randInt(1, 10) == 7) {
            int randomNum = randInt(1,4);
            double x = getX();
            double y = getY();
            StudentWorld *thisWorld = getWorld();
            if      (randomNum == 1) y += SPRITE_HEIGHT;
            else if (randomNum == 2) y -= SPRITE_HEIGHT;
            else if (randomNum == 3) x += SPRITE_WIDTH;
            else                     x -= SPRITE_WIDTH;
            VaccineGoodie *newVaccine = new VaccineGoodie(thisWorld, x, y);
            vector<Actor*> temp;
            if (thisWorld->getOverlaps(temp, newVaccine))
                delete newVaccine;
            else
                thisWorld->addActor(newVaccine);
        }
        Zombie::kill();
    }
}



int DumbZombie::killPoints() const
{ return 1000; }



SmartZombie::SmartZombie(StudentWorld *thisWorld, double startX, double startY, Direction dir)
:Zombie(thisWorld, startX, startY, dir)
{
    
}

SmartZombie::~SmartZombie()
{
    
}


void SmartZombie::kill()
{
    if (isAlive()) Zombie::kill();
}


void SmartZombie::makeDirection()
{
    vector<Actor*> citizens;
    Penelope *player;
    Actor *closeActor;
    double distance;
    StudentWorld *thisWorld = getWorld();
    thisWorld->getPenelope(player);
    closeActor = player;
    distance = getDistance(player);
    if (thisWorld->getCitizen(citizens)) {
        double newDistance;
        for (auto it = citizens.begin(); it != citizens.end(); ++it) {
            newDistance = getDistance(*it);
            if (newDistance < distance) {
                distance = newDistance;
                closeActor = (*it);
            }
        }
    }
    if (distance <= 80) {
        double xDiff = getX() - closeActor->getX();
        double yDiff = getY() - closeActor->getY();
        Direction xDir = xDiff < 0 ? right : left;
        Direction yDir = yDiff < 0 ? up : down;
        
        if (xDiff == 0)      setDirection(yDir);
        else if (yDiff == 0) setDirection(xDir);
        else {
            if (randInt(1, 2) == 1) setDirection(xDir);
            else                    setDirection(yDir);
        }
    }
    else Zombie::makeDirection();
}


int SmartZombie::killPoints() const
{ return 2000; }


Citizen::Citizen(StudentWorld *thisWorld, double startX, double startY, Direction dir)
:Human(thisWorld, IID_CITIZEN, startX, startY, dir)
{
    
}

Citizen::~Citizen()
{
    
}

bool Citizen::doSomething()
{
    if (isAlive()) {
        if (Human::doSomething()) {
            Zombie *newZombie;
            if (randInt(1, 10) <= 7)
                newZombie = new DumbZombie(getWorld(), getX(), getY(), getDirection());
            else
                newZombie = new SmartZombie(getWorld(), getX(), getY(), getDirection());
            getWorld()->addActor(newZombie);
            becomeZombie();
        }
        else {
            if (!isParalyzed()) {
                Penelope *player;
                if (getWorld()->getPenelope(player)) {
                    double dist_p = getDistance(player);
                    double dist_z = dist_p + 1;
                    Actor *targetZombie = nullptr;
                    vector<Actor*> zombies;
                    if (getWorld()->getZombie(zombies)) {
                        double tempDist;
                        for (auto it = zombies.begin(); it != zombies.end(); ++it) {
                            tempDist = getDistance(*it);
                            if (tempDist < dist_z) {
                                dist_z = tempDist;
                                targetZombie = *it;
                            }
                        }
                    }
                    if (dist_p <= 80 || dist_z <= 80) {
                        if (dist_p < dist_z)
                            optimalFollow(player);
                        else
                            optimalRun(targetZombie);
                    }
                }
            }
            MovingActor::doSomething();
        }
    }
    return false;
}


void Citizen::kill()
{
    citizenDied(SOUND_CITIZEN_DIE);
}

void Citizen::save()
{
    citizenDied(SOUND_CITIZEN_SAVED, 500);
}

void Citizen::becomeZombie()
{
    citizenDied(SOUND_ZOMBIE_BORN);
}

void Citizen::citizenDied(int sound, int score)
{
    getWorld()->playSound(sound);
    getWorld()->increaseScore(score);
    getWorld()->decreaseCitizen();
    getWorld()->makeStatus();
    Actor::kill();
}


void Citizen::optimalRun(Actor *zombie)
{
    double x = getX() - zombie->getX();
    double y = getY() - zombie->getY();
    if (x == 0 && y == 0) return;
    else if (x == 0) {
        if (y < 0) {
            if (moveY(-2)) return;
        }
        else {
            if (moveY(2)) return;
        }
    }
    else if (y == 0) {
        if (x < 0) {
            if (moveX(-2)) return;
        }
        else {
            if (moveX(2)) return;
        }
    }
    else if (abs(x) > abs(y)) {
        if (x < 0) {
            if (moveX(-2)) return;
        }
        else {
            if (moveX(2)) return;
        }
        if (y < 0) {
            if (moveY(-2)) return;
        }
        else {
            if (moveY(2)) return;
        }
    }
    else {
        if (y < 0) {
            if (moveY(-2)) return;
        }
        else {
            if (moveY(2)) return;
        }
        if (x < 0) {
            if (moveX(-2)) return;
        }
        else {
            if (moveX(2)) return;
        }
    }
}



void Citizen::optimalFollow(Actor *player)
{
    double x = getX() - player->getX();
    double y = getY() - player->getY();
    if (x == 0) {
        if (y < 0) {
            if (moveY(2)) return;
        }
        else {
            if (moveY(-2)) return;
        }
    }
    else if (y == 0) {
        if (x < 0) {
            if (moveX(2)) return;
        }
        else {
            if (moveX(-2)) return;
        }
    }
    else {
        if (randInt(0, 1) == 1) {
            if (y < 0) {
                if (moveY(2)) return;
            }
            else {
                if (moveY(-2)) return;
            }
            if (x < 0) {
                if (moveX(2)) return;
            }
            else {
                if (moveX(-2)) return;
            }
        }
        else {
            if (x < 0) {
                if (moveX(2)) return;
            }
            else {
                if (moveX(-2)) return;
            }
            if (y < 0) {
                if (moveY(2)) return;
            }
            else {
                if (moveY(-2)) return;
            }
        }
    }
}


bool Citizen::isCitizen() const
{
    return true;
}
