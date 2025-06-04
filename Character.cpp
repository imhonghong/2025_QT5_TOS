#include "Character.h"
#include <algorithm>  // for std::max

// 建構子（原本就放在 .cpp 裡）
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
    if (numSelectedChars > 0) {
        maxHP = totalPlayerHP / numSelectedChars;
    } else {
        maxHP = totalPlayerHP;
    }
    currentHP = maxHP;
}

// ★ 在這裡才真正實作一次 virtual destructor ★
Character::~Character()
{

}

// 以下為其他成員函式的實作……
// 比如：
int Character::getID() const { return id; }
Character::Attribute Character::getAttribute() const { return attribute; }
int Character::getMaxHP() const { return maxHP; }
int Character::getCurrentHP() const { return currentHP; }
int Character::getAttackPower() const { return attackPower; }
QString Character::getIconPath() const { return iconPath; }

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
    return attackPower * comboMultiplier;
}

void Character::useSkill(/*int skillIndex, Character &target*/)
{
    // 暫時留空
}
