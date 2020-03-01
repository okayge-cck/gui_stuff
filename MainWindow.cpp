#include "MainWindow.hpp"

#include <QString>

void keepSize(QWidget* widget)
{
    QSizePolicy retainSizePolicy;
    retainSizePolicy.setRetainSizeWhenHidden(true);
    widget->setSizePolicy(retainSizePolicy);
}

FancySlider::FancySlider(const std::string& text, int minValue, int maxValue, int defaultValue_, QWidget* parent)
    : QWidget(parent)
{
    if (maxValue <= minValue)
    {
        minValue = -100;
        maxValue = 100;
    }

    layout = new QGridLayout();

    slider = new QSlider(Qt::Horizontal, this);
    spinBox = new QSpinBox();
    label = new QLabel(QString::fromStdString(text));
    minValueLabel = new QLabel(QString::fromStdString(std::to_string(minValue)), this);
	maxValueLabel = new QLabel(QString::fromStdString(std::to_string(maxValue)), this);
    zeroLabel = new QLabel("0", this);
    resetButton = new QPushButton("Reset");

    if (!(minValue < 0 && minValue == -maxValue))
    {
        zeroLabel->setText("");
    }

    minValueLabel->setAlignment(Qt::AlignBottom | Qt::AlignLeft);
    maxValueLabel->setAlignment(Qt::AlignBottom | Qt::AlignRight);
    zeroLabel->setAlignment(Qt::AlignBottom | Qt::AlignHCenter);

    slider->setRange(minValue, maxValue);
    slider->setSingleStep(1);
    slider->setTickInterval((maxValue - minValue) / 20);
    slider->setTickPosition(QSlider::TicksBothSides);

    spinBox->setRange(minValue, maxValue);
    spinBox->setSingleStep(1);

    spinBox->setFixedWidth(100);
    label->setFixedWidth(200);

    if (minValue < defaultValue_ && defaultValue_ < maxValue)
    {
        this->defaultValue = defaultValue_;
    }
    else
    {
        this->defaultValue = (maxValue + minValue) / 2;
    }

    connect(slider, &QSlider::valueChanged, this, updateValue);

    connect(spinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, updateValue);

    connect(resetButton, &QPushButton::clicked, [&]()
    {
        updateValue(defaultValue);
    });

    slider->setSliderPosition(defaultValue);
    spinBox->setValue(defaultValue);

    layout->addWidget(minValueLabel, 0, 1);
    layout->addWidget(zeroLabel, 0, 3);
    layout->addWidget(maxValueLabel, 0, 5);
    layout->addWidget(label, 1, 0);
    layout->addWidget(slider, 1, 1, 1, 5);
    layout->addWidget(spinBox, 1, 6);
    layout->addWidget(resetButton, 2, 6);

    this->setLayout(layout);
}

void FancySlider::updateValue(int value)
{
    slider->setSliderPosition(value);
    spinBox->setValue(value);
    emit valueChanged(value);
}










PhaseListElement::PhaseListElement(const std::string& phaseName_, QWidget* parent)
    : QWidget(parent), phaseName(phaseName_)
{
    layout = new QHBoxLayout();
    leftBorderLabel = new QLabel();
	rightBorderLabel = new QLabel();
	textLabel = new QLabel(QString::fromStdString(phaseName));
	executePhaseButton = new QPushButton("Execute");

    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);

    leftBorderLabel->setFixedWidth(50);
    rightBorderLabel->setFixedWidth(50);
    textLabel->setFixedWidth(200);
    executePhaseButton->setFixedWidth(150);

    textLabel->setAlignment(Qt::AlignCenter);

    leftBorderLabel->setAutoFillBackground(true);
	rightBorderLabel->setAutoFillBackground(true);
	textLabel->setAutoFillBackground(true);

    connect(executePhaseButton, &QPushButton::clicked, [this]()
    {
        emit phaseExecutionRequested(phaseName);
    });

    layout->addWidget(leftBorderLabel);
    layout->addWidget(textLabel);
    layout->addWidget(rightBorderLabel);
    layout->addWidget(executePhaseButton);

    this->setLayout(layout);

    keepSize(this);
    keepSize(executePhaseButton);

    setStatus(notReady);
}

void PhaseListElement::setStatus(PhaseStatus status)
{
    QPalette palette;
    palette.setColor(QPalette::Window, statusColors.at(status));

	textLabel->setPalette(palette);

    if (status == running || status == ready)
    {
        leftBorderLabel->setPalette(palette);
	    rightBorderLabel->setPalette(palette);
    }
    else
    {
        QPalette backgroundPalette;
        backgroundPalette.setColor(QPalette::Window, this->palette().color(QPalette::Background));

        leftBorderLabel->setPalette(backgroundPalette);
	    rightBorderLabel->setPalette(backgroundPalette);
    }

    if (status == ready)
    {
        executePhaseButton->show();
    }
    else
    {
        executePhaseButton->hide();
    }

    textLabel->setText(QString::fromStdString(phaseName + ": " + statusMessages.at(status)));
}















PhaseDisplay::PhaseDisplay(const std::vector<std::string>& phaseNames, bool automaticTransitions_, QWidget* parent)
    : QWidget(parent), automaticTransitions(automaticTransitions_)
{
    layout = new QVBoxLayout();

    for (std::string phaseName: phaseNames)
    {
        PhaseListElement* el = new PhaseListElement(phaseName);
        phaseWidgets.push_back(el);

        connect(el, &PhaseListElement::phaseExecutionRequested, [this](const std::string& phaseName)
        {
            emit phaseExecutionRequested(phaseName);
        });

        layout->addWidget(el);
    }

    // if (phaseWidgets.size() > 0)
    // {
    //     phaseWidgets.at(0)->setStatus(ready);
    // }

    this->setLayout(layout);
    keepSize(this);
}








GraspListElement::GraspListElement(const std::string& graspName_, bool reachable, QWidget* parent)
    : QWidget(parent), graspName(graspName_)
{
    layout = new QHBoxLayout();
	textLabel = new QLabel(QString::fromStdString(graspName));
    reachabilityLabel = new QLabel();
	previewButton = new QPushButton("Preview");
	chooseButton = new QPushButton("Choose");

    textLabel->setFixedWidth(250);
    reachabilityLabel->setFixedWidth(100);
    previewButton->setMinimumWidth(100);
    chooseButton->setMinimumWidth(100);

    this->setFixedWidth(600);

    connect(previewButton, &QPushButton::clicked, [this]()
    {
        emit graspPreviewRequested(graspName);
    });

    connect(chooseButton, &QPushButton::clicked, [this]()
    {
        emit graspChosen(graspName);
    });

    layout->addWidget(textLabel);
    layout->addWidget(reachabilityLabel);
    layout->addWidget(previewButton);
    layout->addWidget(chooseButton);

    this->setLayout(layout);
    keepSize(this);

    setReachable(reachable);
}

void GraspListElement::setReachable(bool reachable)
{
    if (reachable)
    {
        reachabilityLabel->setStyleSheet("QLabel { color: green }");
    	reachabilityLabel->setText("Reachable");

        chooseButton->setEnabled(true);
    }
    else
    {
        reachabilityLabel->setStyleSheet("QLabel { color: red }");
    	reachabilityLabel->setText("Not reachable");

        chooseButton->setEnabled(false);
    }
}











GraspSelector::GraspSelector(QWidget* parent)
    : QWidget(parent)
{
    layout = new QHBoxLayout();
    graspList = new QListWidget();

    layout->addWidget(graspList);
    this->setMinimumWidth(650);
    this->setFixedHeight(300);

    this->setLayout(layout);

    keepSize(this);
}

void GraspSelector::addGrasp(const std::string& graspName, bool reachable)
{
    auto graspIt = graspMap.find(graspName);
    if (graspIt != graspMap.end())
    {
        (*graspIt).second->setReachable(reachable);

        return;
    }

    GraspListElement* el = new GraspListElement(graspName, reachable);
    QListWidgetItem* listItem = new QListWidgetItem();
    listItem->setSizeHint(el->minimumSizeHint());
    graspList->addItem(listItem);
    graspList->setItemWidget(listItem, el);

    connect(el, &GraspListElement::graspPreviewRequested, [this](const std::string& graspName)
    {
        emit graspPreviewRequested(graspName);
    });

    connect(el, &GraspListElement::graspChosen, [this](const std::string& graspName)
    {
        emit graspChosen(graspName);
    });

    graspMap.emplace(graspName, el);
}

void GraspSelector::clearList()
{
    graspMap.clear();
    graspList->clear();
}













MainWindow::MainWindow(QWidget *parent)
	: QWidget(parent)
{
	this->resize(1280, 720);

    vLayout = new QVBoxLayout();
	hLayout = new QHBoxLayout();

    // TODO: use new slider
    FancySlider* sldr = new FancySlider("test", -5000, 5000, 0);
    FancySlider* sldr2 = new FancySlider("WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA", 1, 25, 20);

	handYSlider = new QSlider(Qt::Horizontal);
	handDistanceSlider = new QSlider(Qt::Horizontal);

	invertedCheckbox = new QCheckBox("Show inverted grasps");
	sideCheckbox = new QCheckBox("Show grasps for both sides");

	startButton = new QPushButton("Start");
	stopButton = new QPushButton("Stop");

    statusLabel = new QLabel("Status");

    connect(sldr, &FancySlider::valueChanged, [this](int value)
    {
        statusLabel->setText(QString::fromStdString("value: " + std::to_string(value)));
    });

    handYSlider->setMinimum(-500);
    handYSlider->setMaximum(500);
    handYSlider->setTickInterval(10);
    handYSlider->setTickPosition(QSlider::TicksAbove);

    handDistanceSlider->setMinimum(-500);
    handDistanceSlider->setMaximum(500);
    handDistanceSlider->setTickInterval(10);
    handDistanceSlider->setTickPosition(QSlider::TicksAbove);

    vLayout->addWidget(handYSlider);
    vLayout->addWidget(handDistanceSlider);
    vLayout->addWidget(invertedCheckbox);
    vLayout->addWidget(sideCheckbox);
    hLayout->addWidget(startButton);
    hLayout->addWidget(stopButton);
    vLayout->addLayout(hLayout);
    vLayout->addWidget(sldr);
    vLayout->addWidget(sldr2);
    vLayout->addWidget(statusLabel);

    PhaseDisplay* phaseDisplay = new PhaseDisplay({"Generate", "Choose", "Approach", "Grasp", "Lift"}, false);
    vLayout->addWidget(phaseDisplay);

    GraspSelector* sel = new GraspSelector();

    sel->addGrasp("Grasp 1", true);
    sel->addGrasp("Grasp 2", false);
    sel->addGrasp("Grasp 3", false);
    sel->addGrasp("Grasp 4", true);
    sel->addGrasp("Grasp 5", true);
    sel->addGrasp("Grasp 6", true);
    sel->addGrasp("Grasp 7", true);
    sel->addGrasp("Grasp 8", true);
    sel->addGrasp("Grasp 9", true);
    sel->addGrasp("Grasp 10", true);
    sel->addGrasp("Grasp 11", true);
    sel->addGrasp("Grasp 12", true);
    sel->addGrasp("Grasp WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW", true);

    // {"Grasp 1", "Grasp 2", "Grasp 3", "Grasp 4", "Grasp 5", "Grasp 6", "Grasp 7", "Grasp 8", "Grasp 9", "Grasp 10", "Grasp 11", "Grasp 12", "Grasp WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW"}
    vLayout->addWidget(sel);

    connect(sel, &GraspSelector::graspPreviewRequested, [](const std::string& graspName)
    {
        std::cout << "Grasp preview req: " << graspName << std::endl;
    });

    connect(sel, &GraspSelector::graspChosen, [sel](const std::string& graspName)
    {
        std::cout << "Grasp chosen: " << graspName << std::endl;
    });

    this->setLayout(vLayout);
}
