#pragma once

#include "Character.h"

class Enemy : public Character
{
public:
    Enemy(int id,
          Attribute attr,
          int maxHP,
          const QString &iconPath,
          int cooldownDefault);

    virtual ~Enemy();
    void onNewTurn(Character &target);
    void performAttack(Character &target);
    int getCooldownCounter() const;
    int getCooldownDefault() const;
    void reset() override;

private:
    int cooldownCounter;
    int cooldownDefault;
};
