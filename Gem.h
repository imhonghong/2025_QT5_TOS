// Gem.h
#pragma once

#include <QObject>
#include <QPixmap>
#include <QPointF>
#include <QPainter>

class Gem
{
public:
    // 和 Character 共用的屬性枚舉
    enum Attribute { Water = 0, Fire, Earth, Light, Dark };

    // 敵人技能對符石的附加效果
    enum EffectStatus { Normal = 0, Burning, Weathered };

    // 動畫／狀態
    enum State { Idle = 0, Swapping, Falling, Clearing };

    // 每顆符石格子尺寸（像素）
    static constexpr int TILE_SIZE = 90;

    // 交換／下落速度（像素/次 update 呼叫）
    static constexpr float SWAP_STEP = 16.0f;
    static constexpr float FALL_STEP = 16.0f;

    // 建構子: 傳入屬性、初始格子 row/col、和圖檔路徑
    Gem(Attribute type, int row, int col, const QString &iconPath);
    virtual ~Gem();

    // Getter
    Attribute    getType() const;
    EffectStatus getEffectStatus() const;
    int          getRow() const;
    int          getCol() const;
    State        getState() const;
    QString getIconPath() const;    // 回傳符石圖示路徑
    bool         isToBeCleared() const;

    // Setter
    void setType(Attribute t);
    void setEffectStatus(EffectStatus s);
    void setRow(int r);
    void setCol(int c);
    void setState(State s);
    void markForClearing();          // 標記此顆符石為待清除

    // 交換兩顆符石：交換 row/col，並設置動畫起點與終點
    void swapWith(Gem *other);
    void updateSwapAnimation();      // 在外部每幀呼叫，以平滑移動到目標

    // 開始下落：參數為要下落的格數
    void startFalling(int numRows);
    void updateFallAnimation();      // 在外部每幀呼叫，以平滑下落

    // 清除判定：若 state == Clearing，表示可供外層刪除
    bool hasFinishedClearing() const;

    // 繪製此符石 (在 GameStageWidget::paintEvent 中使用)
    void paint(QPainter *painter) const;

private:
    Attribute    type;            // 符石屬性
    EffectStatus effectStatus;    // 敵人技能附加效果

    int          row;             // 邏輯格子列索引
    int          col;             // 邏輯格子欄索引

    State        state;           // 符石當前狀態
    bool         toBeCleared;     // 是否已標記為待清除
    int          fallDistance;    // 剩餘下落格數

    QString      iconPath;        // 圖檔路徑
    QPixmap      pixmap;          // 讀入的圖資

    QPointF      currentPos;      // 目前畫面座標 (像素)
    QPointF      targetPos;       // 目標畫面座標 (像素)
};
