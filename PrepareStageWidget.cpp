#include "PrepareStageWidget.h"
#include <QMessageBox>
#include <QIcon>
#include <QSize>
#include <QFont>

PrepareStageWidget::PrepareStageWidget(QWidget *parent)
    : QWidget(parent)
{
    // MainWindow 會替本 Widget 設定固定大小 540×960，所以這裡不需再呼叫 setFixedSize

    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(30);

    //----------------------------------------
    // (1) Player's Character: 標題
    QLabel *labelTitle = new QLabel("Player's Character:", this);
    QFont font1;
    font1.setPointSize(18);
    labelTitle->setFont(font1);
    mainLayout->addWidget(labelTitle, 0, Qt::AlignLeft);

    //----------------------------------------
    // (2) 六個 QComboBox 並排，用於選角色
    //    - 第一項為空（表示此槽位不選人物，回傳 0）
    //    - 後面 1~5 為角色 ID，同時配合 Icon 圖示顯示
    QHBoxLayout *hLayoutChars = new QHBoxLayout;
    hLayoutChars->setSpacing(8);

    // 準備角色清單 (index 0 = 空白)
    for (int i = 0; i < 6; ++i) {
        comboChars[i] = new QComboBox(this);
        comboChars[i]->setFixedSize(70, 70);
        comboChars[i]->setIconSize(QSize(48, 48));

        // 第一個選項是空白 (代表此槽不選角色)
        comboChars[i]->addItem("");

        // 後面 1~5 為角色 ID，同時顯示圖示
        for (int id = 1; id <= 5; ++id) {
            QString text = QString::number(id);
            // 注意：請把下面這條路徑改成你真正放置角色圖示的 qrc 路徑
            QString iconPath = QString(":/character/dataset/character/ID%1.png").arg(id);
            comboChars[i]->addItem(QIcon(iconPath), text);
        }

        comboChars[i]->setCurrentIndex(0); // 預設空白
        hLayoutChars->addWidget(comboChars[i]);
    }
    mainLayout->addLayout(hLayoutChars);

    //----------------------------------------
    // (3) 空一段縱向間距
    mainLayout->addSpacing(20);

    //----------------------------------------
    // (4) Game Mission: 標題
    QLabel *labelMission = new QLabel("Game Mission:", this);
    QFont font2;
    font2.setPointSize(18);
    labelMission->setFont(font2);
    mainLayout->addWidget(labelMission, 0, Qt::AlignLeft);

    //----------------------------------------
    // (5) QSpinBox 作為 mission 輸入，可以用鍵盤直接輸入數字
    spinMission = new QSpinBox(this);
    spinMission->setRange(1, 10);  // 範例只給 1~10
    spinMission->setValue(1);      // 預設為 1
    spinMission->setFixedSize(100, 40);
    spinMission->setKeyboardTracking(false);
    spinMission->setFrame(true);
    mainLayout->addWidget(spinMission, 0, Qt::AlignLeft);

    //----------------------------------------
    // (6) 拉伸一下，把 Start 按鈕推到最下方
    mainLayout->addStretch();

    //----------------------------------------
    // (7) Start 按鈕
    startButton = new QPushButton("Start", this);
    startButton->setFixedSize(200, 60);
    QFont fontBtn;
    fontBtn.setPointSize(16);
    startButton->setFont(fontBtn);
    connect(startButton, &QPushButton::clicked,
            this, &PrepareStageWidget::onStartButtonClicked);
    mainLayout->addWidget(startButton, 0, Qt::AlignHCenter);

    mainLayout->addSpacing(20);
}

void PrepareStageWidget::onStartButtonClicked()
{
    // (1) 先建一個長度 6、初始值全為 0 的向量
    QVector<int> padded(6, 0);

    // (2) 把每個 comboChars[i] 的選項轉成對應 ID (0~5)
    //     index = comboChars[i]->currentIndex()
    //     如果 index == 0 → 空白 → padded[i] = 0
    //     如果 index > 0 → 代表角色 ID = itemText(index).toInt()
    for (int i = 0; i < 6; ++i) {
        int idx = comboChars[i]->currentIndex();
        if (idx > 0) {
            // itemText(idx) = "1"~"5"
            int charID = comboChars[i]->itemText(idx).toInt();
            padded[i] = charID;
        }
        // 若 idx == 0，padded[i] 保持為 0 (空位)
    }

    // (3) 檢查：至少要選一個角色 (也就是 padded 中必須有一個 > 0)
    bool anySelected = false;
    for (int v : padded) {
        if (v > 0) {
            anySelected = true;
            break;
        }
    }
    if (!anySelected) {
        QMessageBox::warning(this, "Warning", "請至少選擇一個角色！");
        return;
    }

    // (4) 取得 missionID
    int missionID = spinMission->value();

    // (5) 發射信號：帶出長度 6、空位以 0 表示的 selectedChars
    emit startClicked(padded, missionID);
}
