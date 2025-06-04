// Gem.cpp
#include "Gem.h"
#include <algorithm>

////////////////////////////////////////////////////////////////////////////////
// Constructor & Destructor
////////////////////////////////////////////////////////////////////////////////

Gem::Gem(Attribute type, int row, int col, const QString &iconPath)
    : type(type),
      effectStatus(Normal),
      row(row),
      col(col),
      state(Idle),
      toBeCleared(false),
      fallDistance(0),
      iconPath(iconPath),
      currentPos(col * TILE_SIZE, row * TILE_SIZE),
      targetPos(currentPos)
{
    pixmap.load(iconPath);
}

Gem::~Gem()
{
    // 無需額外釋放
}

////////////////////////////////////////////////////////////////////////////////
// Getter / Setter
////////////////////////////////////////////////////////////////////////////////

Gem::Attribute Gem::getType() const
{
    return type;
}

Gem::EffectStatus Gem::getEffectStatus() const
{
    return effectStatus;
}

int Gem::getRow() const
{
    return row;
}

int Gem::getCol() const
{
    return col;
}

Gem::State Gem::getState() const
{
    return state;
}

bool Gem::isToBeCleared() const
{
    return toBeCleared;
}

void Gem::setType(Attribute t)
{
    type = t;
}

void Gem::setEffectStatus(EffectStatus s)
{
    effectStatus = s;
}

void Gem::setRow(int r)
{
    row = r;
    targetPos.setY(r * TILE_SIZE);
}

void Gem::setCol(int c)
{
    col = c;
    targetPos.setX(c * TILE_SIZE);
}

void Gem::setState(State s)
{
    state = s;
}

void Gem::markForClearing()
{
    toBeCleared = true;
    state = Clearing;
}

////////////////////////////////////////////////////////////////////////////////
// Swap Animation
////////////////////////////////////////////////////////////////////////////////

void Gem::swapWith(Gem *other)
{
    if (!other) return;

    // 交換 row/col
    std::swap(this->row, other->row);
    std::swap(this->col, other->col);

    // 設定新的目標像素座標
    this->targetPos = QPointF(this->col * TILE_SIZE, this->row * TILE_SIZE);
    other->targetPos = QPointF(other->col * TILE_SIZE, other->row * TILE_SIZE);

    // 切換狀態
    this->state = Swapping;
    other->state = Swapping;
}

void Gem::updateSwapAnimation()
{
    if (state != Swapping) return;

    // X 平滑移動至 targetPos.x()
    if (std::abs(currentPos.x() - targetPos.x()) > SWAP_STEP) {
        if (currentPos.x() < targetPos.x())
            currentPos.rx() += SWAP_STEP;
        else
            currentPos.rx() -= SWAP_STEP;
    } else {
        currentPos.rx() = targetPos.x();
    }

    // Y 平滑移動至 targetPos.y()
    if (std::abs(currentPos.y() - targetPos.y()) > SWAP_STEP) {
        if (currentPos.y() < targetPos.y())
            currentPos.ry() += SWAP_STEP;
        else
            currentPos.ry() -= SWAP_STEP;
    } else {
        currentPos.ry() = targetPos.y();
    }

    // 如果已經到達目標，切回 Idle
    if (currentPos == targetPos) {
        state = Idle;
    }
}

////////////////////////////////////////////////////////////////////////////////
// Fall Animation
////////////////////////////////////////////////////////////////////////////////

void Gem::startFalling(int numRows)
{
    if (numRows <= 0) return;

    fallDistance = numRows;
    state = Falling;
    targetPos = QPointF(col * TILE_SIZE, (row + fallDistance) * TILE_SIZE);
}

void Gem::updateFallAnimation()
{
    if (state != Falling) return;

    // Y 平滑移動至 targetPos.y()
    if (currentPos.y() < targetPos.y()) {
        currentPos.ry() = std::min(currentPos.y() + FALL_STEP, targetPos.y());
    }

    // 檢查是否到達目標
    if (currentPos.y() >= targetPos.y()) {
        // 更新 row
        row += fallDistance;
        fallDistance = 0;
        currentPos.setY(row * TILE_SIZE);
        state = Idle;
    }
}

////////////////////////////////////////////////////////////////////////////////
// Clearing 判定
////////////////////////////////////////////////////////////////////////////////

bool Gem::hasFinishedClearing() const
{
    // 當狀態為 Clearing，表示已被標記，由外層 Board 處理 delete
    return (state == Clearing);
}

////////////////////////////////////////////////////////////////////////////////
// Paint
////////////////////////////////////////////////////////////////////////////////

void Gem::paint(QPainter *painter) const
{
    if (!painter) return;

    // 只要 pixmap 已載入，就畫在 currentPos
    if (!pixmap.isNull()) {
        painter->drawPixmap(currentPos.toPoint(), pixmap);
    }
}

QString Gem::getIconPath() const {
    return iconPath;
}
