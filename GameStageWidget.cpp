// GameStageWidget.cpp
#include "GameStageWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFont>
#include <QLabel>
#include <QPixmap>

// ------------------------------------------------------------------
// 簡單版的 RunestoneWidget：繼承自 QLabel，大小固定 90×90，作為占位
class RunestoneWidget : public QLabel {
public:
    explicit RunestoneWidget(QWidget *parent = nullptr)
        : QLabel(parent)
    {
        setFixedSize(90, 90);
        setStyleSheet("background-color: lightGray; border: 1px solid gray;");
        setAlignment(Qt::AlignCenter);
        setText("Gem");
    }
};
// ------------------------------------------------------------------

GameStageWidget::GameStageWidget(QWidget *parent)
    : QWidget(parent)
    , enemyArea(new QWidget(this))
    , playerArea(new QWidget(this))
    , runestoneArea(new QWidget(this))
    , gemLayout(new QGridLayout(runestoneArea))
    , settingButton(new QPushButton("⚙", this))
    , fakeWinBtn(new QPushButton("模擬勝利", this))
    , fakeLoseBtn(new QPushButton("模擬失敗", this))
    , isPaused(false)
    , missionID(1) // 可以給個預設值
    , charLayout(nullptr)
{
    // 把此 Widget 的大小固定為 540×960
    setFixedSize(540, 960);

    // 設定初始 UI 版面 (敵人區 / 玩家區 / 符石區)
    setupUI();
}

// ----------------------------------------------------------
// setupUI：只負責畫面佈局，不放任何動態內容
void GameStageWidget::setupUI()
{
    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // ===== 1. 敵人區 (540×410) =====
    enemyArea->setFixedSize(540, 410);
    {
        auto enemyLayout = new QVBoxLayout(enemyArea);
        enemyLayout->setContentsMargins(0, 0, 0, 0);
        enemyLayout->setSpacing(5);

        // 暫放一個 Label 當作占位
        QLabel *enemyLabel = new QLabel("敵人區 (410×540)", enemyArea);
        enemyLabel->setAlignment(Qt::AlignCenter);
        QFont f; f.setPointSize(18); enemyLabel->setFont(f);
        enemyLayout->addWidget(enemyLabel);

        // 再放兩個測試用的「模擬勝利」「模擬失敗」按鈕
        auto btnLayout = new QHBoxLayout;
        btnLayout->addStretch();
        fakeWinBtn->setFixedSize(100, 40);
        fakeLoseBtn->setFixedSize(100, 40);
        connect(fakeWinBtn, &QPushButton::clicked, this, &GameStageWidget::onFakeWinButtonClicked);
        connect(fakeLoseBtn, &QPushButton::clicked, this, &GameStageWidget::onFakeLoseButtonClicked);
        btnLayout->addWidget(fakeWinBtn);
        btnLayout->addSpacing(10);
        btnLayout->addWidget(fakeLoseBtn);
        btnLayout->addStretch();
        enemyLayout->addLayout(btnLayout);
    }
    mainLayout->addWidget(enemyArea);

    // ===== 2. 玩家區 (540×100) =====
    playerArea->setFixedSize(540, 100);
    {
        // 先建立一個水平的 charLayout，用來動態放角色圖示
        charLayout = new QHBoxLayout;
        charLayout->setContentsMargins(10, 5, 10, 5);
        charLayout->setSpacing(10);

        // 預設先放一個占位 Label，initGame() 會把它清掉
        QLabel *placeholder = new QLabel("玩家區 (100×540)", playerArea);
        placeholder->setAlignment(Qt::AlignCenter);
        QFont f; f.setPointSize(16); placeholder->setFont(f);
        placeholder->setStyleSheet("background-color: #EFEFEF;");
        placeholder->setFixedSize(500, 80);
        charLayout->addWidget(placeholder);

        // ⚙ 設定按鈕放在最右側
        settingButton->setFixedSize(30, 30);
        connect(settingButton, &QPushButton::clicked, this, &GameStageWidget::onSettingClicked);
        charLayout->addWidget(settingButton, 0, Qt::AlignTop);

        // 把 charLayout 加到 playerArea
        auto playerOuterLayout = new QHBoxLayout(playerArea);
        playerOuterLayout->setContentsMargins(0, 0, 0, 0);
        playerOuterLayout->addLayout(charLayout);
    }
    mainLayout->addWidget(playerArea);

    // ===== 3. 符石區 (540×450) =====
    runestoneArea->setFixedSize(540, 450);
    runestoneArea->setStyleSheet("background-color: #000000;");
    {
        gemLayout->setContentsMargins(0, 0, 0, 0);
        gemLayout->setSpacing(0);

        // 先產生 5×6 = 30 個符石格子 (RunestoneWidget)
        for (int r = 0; r < 5; ++r) {
            for (int c = 0; c < 6; ++c) {
                RunestoneWidget *w = new RunestoneWidget(runestoneArea);
                gems.append(w);
                gemLayout->addWidget(w, r, c);
            }
        }
    }
    mainLayout->addWidget(runestoneArea);
}
// ----------------------------------------------------------

// setter：存下從 Prepare 階段傳來的角色 ID 列表
void GameStageWidget::setSelectedCharacters(const QVector<int> &chars)
{
    selectedChars = chars;
}

// setter：存下從 Prepare 階段傳來的 mission ID
void GameStageWidget::setMissionID(int mission)
{
    missionID = mission;
}

// initGame：真正要開始「遊戲邏輯初始化」時呼叫
void GameStageWidget::initGame()
{
    // (1) 先重置遊戲：把上一輪殘留的動態元素都清掉
    resetGame();

    // ===== (2) 清空玩家區 charLayout 中的所有 widget（包括 placeholder） =====
    while (QLayoutItem *item = charLayout->takeAt(0)) {
        if (QWidget *w = item->widget()) {
            w->deleteLater();
        }
        delete item;
    }

    // ===== (3) 根據 selectedChars，逐一建立 QLabel 顯示對應角色圖示 =====
    for (int id : selectedChars) {
        // 假設角色圖片放在資源路徑 :":/character/dataset/character/ID%1.png"
        QString iconPath = QString(":/character/dataset/character/ID%1.png").arg(id);
        QLabel *charLabel = new QLabel(playerArea);
        charLabel->setFixedSize(80, 80);
        charLabel->setPixmap(
            QPixmap(iconPath)
                .scaled(80, 80, Qt::KeepAspectRatio, Qt::SmoothTransformation)
        );
        charLabel->setToolTip(QString("角色 %1").arg(id));
        charLayout->addWidget(charLabel);
    }

    // ===== (4) 最後再把⚙按鈕加回到最右 =====
    charLayout->addStretch();
    charLayout->addWidget(settingButton, 0, Qt::AlignTop);

    // ===== (5) 這裡之後可插入「根據 missionID 載入第一波敵人」 等邏輯 =====
    //    例如：
    //    qDebug() << "GameStage 初始化: selectedChars =" << selectedChars << ", missionID =" << missionID;
    //    loadWave1(selectedChars, missionID);
    //    startGemBoardTimer();
    //    ...

    isPaused = false;
}

// resetGame：把所有動態物件都清空
void GameStageWidget::resetGame()
{
    isPaused = false;

    // (A) 如果你有在 enemyArea 動態 new Enemy 的 QLabel 或其他 widget，要在這裡刪除
    //     例如：for (auto eWidget : enemyWidgets) { eWidget->deleteLater(); } enemyWidgets.clear();

    // (B) 如果你重玩，符石格子 gems 目前已經固定生成 30 個，若改成動態 new/delete 的話，
    //     也要在這裡一併刪除並 clear gems 與 gemLayout。

    // 但重點是「playerArea 內的 placeholder/角色圖示」已在 initGame() 處理過；
    // 此處就不用再針對 charLayout 做事情。
}

void GameStageWidget::pauseGame()
{
    if (isPaused) return;
    isPaused = true;
    // 例如：如果有 QTimer 在跑，呼叫 timer->stop();
    //       如果有拖珠回合在跑，要阻斷滑鼠事件
}

void GameStageWidget::resumeGame()
{
    if (!isPaused) return;
    isPaused = false;
    // 例如：如果之前停了 QTimer，此處再 timer->start();
    //       繼續允許拖珠回合
}

void GameStageWidget::onSettingClicked()
{
    if (isPaused) return;
    pauseGame();
    emit pauseRequested();
}

void GameStageWidget::onFakeWinButtonClicked()
{
    emit gameOver(true);
}

void GameStageWidget::onFakeLoseButtonClicked()
{
    emit gameOver(false);
}
