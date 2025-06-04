// Enemy.cpp
#include "Enemy.h"

Enemy::Enemy(int id,
             Attribute attr,
             int maxHP,
             const QString &iconPath,
             int cooldownDefault)
    : Character(id, attr,
                /* totalPlayerHP = */ maxHP,  /* numSelectedChars = */ 1,
                iconPath),
      cooldownCounter(cooldownDefault),
      cooldownDefault(cooldownDefault)
{
    // 因為 Character 的建構子把 maxHP 計算為 totalPlayerHP/1，所以就等於傳入的 maxHP
    // currentHP 由 Character 自動設為 maxHP
}

void Enemy::onNewTurn(Character &target)
{
    cooldownCounter--;
    if (cooldownCounter <= 0) {
        // 本回合可以攻擊
        performAttack(target);
        // 攻擊完成後重置冷卻
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
    // 恢復血量
    Character::reset();
    // 重置冷卻
    cooldownCounter = cooldownDefault;
}
