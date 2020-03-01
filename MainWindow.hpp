#pragma once

#include <vector>
#include <unordered_map>
#include <functional>
#include <iostream>

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QCheckBox>
#include <QLabel>
#include <QSlider>
#include <QPushButton>
#include <string>
#include <QSpinBox>
#include <QListWidget>
#include <QListWidgetItem>

class FancySlider : public QWidget
{
	Q_OBJECT

public:

	FancySlider(const std::string& text, int minValue, int maxValue, int defaultValue = 0, QWidget* parent = 0);
	~FancySlider() {}

signals:

	void valueChanged(int value);

private slots:

	void updateValue(int value);

private:

	int defaultValue = 0;

	QGridLayout* layout;
	QSlider* slider;
	QSpinBox* spinBox;
	QLabel* label;
	QLabel* minValueLabel;
	QLabel* maxValueLabel;
	QLabel* zeroLabel;
	QPushButton* resetButton;

};

enum PhaseStatus
{
	notReady,
	ready,
	running,
	completed,
	error
};

const std::unordered_map<PhaseStatus, QColor, std::hash<int>> statusColors =
{
	{notReady, QColor(150, 150, 150)},
	{ready, QColor(255, 255, 150)},
	{running, QColor(150, 150, 255)},
	{completed, QColor(150, 255, 150)},
	{error, QColor(255, 150, 150)}
};

const std::unordered_map<PhaseStatus, std::string, std::hash<int>> statusMessages =
{
	{notReady, "not ready"},
	{ready, "ready"},
	{running, "running"},
	{completed, "completed"},
	{error, "ERROR"}
};

class PhaseListElement : public QWidget
{

	Q_OBJECT

public:

	PhaseListElement(const std::string& phaseName_, QWidget* parent = 0);
	~PhaseListElement() {}

public slots:

	void setStatus(PhaseStatus status);

signals:

	void phaseExecutionRequested(const std::string& phaseName);

private:

	std::string phaseName;

	QHBoxLayout* layout;
	QLabel* leftBorderLabel;
	QLabel* rightBorderLabel;
	QLabel* textLabel;
	QPushButton* executePhaseButton;

};

class PhaseDisplay : public QWidget
{

	Q_OBJECT

public:

	PhaseDisplay(const std::vector<std::string>& phaseNames, bool automaticTransitions_, QWidget* parent = 0);
	~PhaseDisplay() {}

signals:

	void phaseExecutionRequested(const std::string& phaseName);

private:

	bool automaticTransitions;
	std::vector<PhaseListElement*> phaseWidgets;
	QVBoxLayout* layout;

	void executePhaseAction(const std::string& phaseName);

};

class GraspListElement : public QWidget
{

	Q_OBJECT

public:

	GraspListElement(const std::string& graspName_, bool reachable, QWidget* parent = 0);
	~GraspListElement()
	{
		delete layout;
		delete textLabel;
		delete reachabilityLabel;
		delete previewButton;
		delete chooseButton;
	}

public slots:

	void setReachable(bool reachable);

signals:

	void graspPreviewRequested(const std::string& graspName);
	void graspChosen(const std::string& graspName);

private:

	std::string graspName;

	QHBoxLayout* layout;
	QLabel* textLabel;
	QLabel* reachabilityLabel;
	QPushButton* previewButton;
	QPushButton* chooseButton;

};

class GraspSelector : public QWidget
{

	Q_OBJECT

public:

	GraspSelector(QWidget* parent = 0);
	~GraspSelector() {}

	void addGrasp(const std::string& graspName, bool reachable);
	void clearList();

signals:

	void graspPreviewRequested(const std::string& graspName);
	void graspChosen(const std::string& graspName);

private:

	QHBoxLayout* layout;
	QListWidget* graspList;
	std::unordered_map<std::string, GraspListElement*> graspMap;

};

class MainWindow : public QWidget
{
	Q_OBJECT

public:

	MainWindow(QWidget *parent = 0);
    ~MainWindow() {}

private:

	QVBoxLayout* vLayout;
	QHBoxLayout* hLayout;

	QSlider* handYSlider;
	QSlider* handDistanceSlider;

	QCheckBox* invertedCheckbox;
	QCheckBox* sideCheckbox;

	QPushButton* startButton;
	QPushButton* stopButton;

	QLabel* statusLabel;

};
