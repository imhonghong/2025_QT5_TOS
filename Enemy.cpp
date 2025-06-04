#include "Enemy.h"

// 建構子
Enemy::Enemy(int id,
             Attribute attr,
             int maxHP,
             const QString &iconPath,
             int cooldownDefault)
    : Character(id, attr, maxHP, 1, iconPath),
      cooldownCounter(cooldownDefault),
      cooldownDefault(cooldownDefault)
{
}

// ★ 在這裡才真正實作一次 virtual destructor ★
Enemy::~Enemy()
{

}

void Enemy::onNewTurn(Character &target)
{
    cooldownCounter--;
    if (cooldownCounter <= 0) {
        performAttack(target);
        cooldownCounter = cooldownDefault;
    }
}

void Enemy::performAttack(Character &target)
{
    if (target.isAlive()) {
        target.takeDamage(getAttackPower());
    }
}

int Enemy::getCooldownCounter() const
{
    return cooldownCounter;
}

int Enemy::getCooldownDefault() const
{
    return cooldownDefault;
}

void Enemy::reset()
{
    Character::reset();
    cooldownCounter = cooldownDefault;
}
