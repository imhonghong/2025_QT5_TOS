// Enemy.h
#pragma once

#include "Character.h"

/*
 * Enemy 類別，繼承 Character
 *  - 額外有 cooldownCounter 與 cooldownDefault 兩個欄位
 *  - 每回合呼叫 onNewTurn() 使 cooldownCounter--，若 <= 0 則可以攻擊一次
 *  - 攻擊時呼叫 performAttack(Character &target)
 *  - reset() override：恢復滿血，重置 cooldownCounter
 */

class Enemy : public Character
{
public:
    // 建構子：
    //   id:             敵人 ID (例如 101, 202, 301…)
    //   attr:           屬性 (Water, Fire, Earth, Light, Dark)
    //   maxHP:          這隻敵人的最大血量
    //   iconPath:       圖示路徑 (e.g. ":/enemy/dataset/enemy/100n.png")
    //   cooldownDefault:預設冷卻回合 (e.g. 3, 5)
    Enemy(int id,
          Attribute attr,
          int maxHP,
          const QString &iconPath,
          int cooldownDefault);

    // 每回合呼叫一次：cooldownCounter--，若 <= 0，則對 target 執行 performAttack()，然後重置 cooldownCounter
    void onNewTurn(Character &target);

    // 直接對 target 造成攻擊 (attackPower * 1)
    void performAttack(Character &target);

    // 取得當前冷卻值
    int getCooldownCounter() const;

    // 取得預設冷卻值
    int getCooldownDefault() const;

    // override reset()：先呼叫 Character::reset()，再把 cooldownCounter 重置
    void reset() override;

private:
    int cooldownCounter;  // 當前剩餘冷卻回合
    int cooldownDefault;  // 每隔 cooldownDefault 回合可攻擊一次
};
