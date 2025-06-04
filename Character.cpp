// Character.cpp
#include "Character.h"
#include <algorithm>  // for std::max

// 建構子實作
Character::Character(int id,
                     Attribute attr,
                     int totalPlayerHP,
                     int numSelectedChars,
                     const QString &iconPath)
    : id(id),
      attribute(attr),
      attackPower(1),
      iconPath(iconPath)
{
    // 計算每隻角色的 maxHP = totalPlayerHP / numSelectedChars
    if (numSelectedChars > 0) {
        maxHP = totalPlayerHP / numSelectedChars;
    } else {
        // 保險起見，如果參數不合理，就把所有血量給這隻
        maxHP = totalPlayerHP;
    }
    currentHP = maxHP;
}

int Character::getID() const
{
    return id;
}

Character::Attribute Character::getAttribute() const
{
    return attribute;
}

int Character::getMaxHP() const
{
    return maxHP;
}

int Character::getCurrentHP() const
{
    return currentHP;
}

int Character::getAttackPower() const
{
    return attackPower;
}

QString Character::getIconPath() const
{
    return iconPath;
}

void Character::takeDamage(int damage)
{
    currentHP = std::max(0, currentHP - damage);
}

bool Character::isAlive() const
{
    return currentHP > 0;
}

void Character::reset()
{
    currentHP = maxHP;
}

int Character::calculateDamageOutput(int comboMultiplier) const
{
    return attackPower * comboMultiplier; // 一律乘 1
}

void Character::useSkill(/*int skillIndex, Character &target*/)
{
    // 暫時留空，未來可在子類別 override
}
