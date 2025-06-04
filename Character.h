// Character.h
#pragma once

#include <QString>

/*
 * Character 類別
 *  - id: 角色 ID（例如 1~5）
 *  - attribute: 角色屬性（enum）
 *  - maxHP / currentHP: 血量
 *  - attackPower: 固定為 1
 *  - iconPath: 圖示路徑，用於 UI 顯示
 *
 *  - 建構子會依 totalPlayerHP 與 numSelectedChars 自動計算 maxHP
 *  - 提供 getIconPath() 讓外部直接使用
 */

class Character
{
public:
    // 屬性列舉
    enum Attribute { Water, Fire, Earth, Light, Dark };

    // 建構子：
    //   id:             角色唯一識別
    //   attr:           角色屬性
    //   totalPlayerHP:  玩家隊伍總血量 (一般設定為 2000)
    //   numSelected:    玩家這次選的角色數量 (1~6)
    //   iconPath:       圖示路徑 (e.g. ":/characters/char1.png")
    Character(int id,
              Attribute attr,
              int totalPlayerHP,
              int numSelectedChars,
              const QString &iconPath);

    // Getter
    int getID() const;
    Attribute getAttribute() const;
    int getMaxHP() const;
    int getCurrentHP() const;
    int getAttackPower() const;
    QString getIconPath() const;

    // 承受傷害：扣血，但不低於 0
    void takeDamage(int damage);

    // 判斷是否存活
    bool isAlive() const;

    // 回復到滿血；virtual 以利子類別 override
    virtual void reset();

    // 計算輸出傷害 (comboMultiplier * 1)
    int calculateDamageOutput(int comboMultiplier) const;

    // 空的技能函式，將來可 override
    virtual void useSkill(/*int skillIndex, Character &target*/);

private:
    int id;
    Attribute attribute;
    int maxHP;
    int currentHP;
    int attackPower;    // 固定為 1
    QString iconPath;
};
