// PrepareStageWidget.cpp
#include "PrepareStageWidget.h"
#include <QMessageBox>

PrepareStageWidget::PrepareStageWidget(QWidget *parent)
    : QWidget(parent)
{
    // 這裡不需再呼叫 setFixedSize，MainWindow 已經替此 Widget 設定為 540×960

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
    //    - 第一項為空（表示此槽位不選人物）
    //    - 後面 1~5 為角色 ID，同時配合 Icon 圖示顯示
    QHBoxLayout *hLayoutChars = new QHBoxLayout;
    hLayoutChars->setSpacing(8);

    // List 內先放空白，然後把 ID 1~5 加入
    // Icon 路徑請自行修改成實際放置的檔案
    QStringList charList;
    charList << "";  // index 0: 空白
    for (int id = 1; id <= 5; ++id) {
        charList << QString::number(id);
    }

    for (int i = 0; i < 6; ++i) {
        comboChars[i] = new QComboBox(this);
        // 設定大小：寬 70、高 70，圖示和文字都置中
        comboChars[i]->setFixedSize(70, 70);
        comboChars[i]->setIconSize(QSize(48, 48)); // 圖示大小大約 48x48

        // 第一個選項先放空白
        comboChars[i]->addItem("");

        // 加入 ID=1~5，帶上對應圖示與文字
        // 假設角色圖檔在 resource (qrc) 裡面，路徑為 :/characters/char1.png, char2.png, ...
        for (int id = 1; id <= 5; ++id) {
            QString text = QString::number(id);
            QString iconPath = QString(":/character/dataset/character/ID%1.png").arg(id);
            comboChars[i]->addItem(QIcon(iconPath), text);
        }

        comboChars[i]->setCurrentIndex(0); // 預設為空白
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
    spinMission->setRange(1, 10);  // 範例：未來若要做 bonus 可擴增到 10 關
    spinMission->setValue(1);      // 預設為 1
    spinMission->setFixedSize(100, 40);
    spinMission->setKeyboardTracking(false); // 用戶輸入後才觸發 valueChanged()
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
    QVector<int> selectedChars;

    // 把六個下拉式選單中所有 index > 0 (非空白) 的角色 ID 加進 QVector
    for (int i = 0; i < 6; ++i) {
        int idx = comboChars[i]->currentIndex();
        if (idx > 0) {
            // comboChars[i]->itemText(idx) = "1"~"5"
            int charID = comboChars[i]->itemText(idx).toInt();
            selectedChars.append(charID);
        }
    }

    // 至少要選一個角色才行
    if (selectedChars.isEmpty()) {
        QMessageBox::warning(this, "Warning", "請至少選擇一個角色！");
        return;
    }

    // 取得 missionID
    int missionID = spinMission->value();

    // 發送信號，帶選到的角色列表和 missionID
    emit startClicked(selectedChars, missionID);
}
