#pragma once

#include <QString>

class Character
{
public:
    enum Attribute { Water, Fire, Earth, Light, Dark };

    Character(int id,
              Attribute attr,
              int totalPlayerHP,
              int numSelectedChars,
              const QString &iconPath);

    virtual ~Character();

    int getID() const;
    Attribute getAttribute() const;
    int getMaxHP() const;
    int getCurrentHP() const;
    int getAttackPower() const;
    QString getIconPath() const;

    void takeDamage(int damage);
    bool isAlive() const;
    virtual void reset();
    int calculateDamageOutput(int comboMultiplier) const;
    virtual void useSkill(/*int skillIndex, Character &target*/);

private:
    int id;
    Attribute attribute;
    int maxHP;
    int currentHP;
    int attackPower;
    QString iconPath;
};
