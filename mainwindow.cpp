#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);

  system = new System(this);

  // memoryWidget = new MemoryWidget(this, system);
  // this->addDockWidget(Qt::LeftDockWidgetArea, memoryWidget);

  cpuWidget = new CpuWidget(this, system->memoryView());
  this->addDockWidget(Qt::RightDockWidgetArea, cpuWidget);

  assemblerWidget = new AssemblerWidget(this);
  this->setCentralWidget(assemblerWidget);

  pollTimer = new QTimer(this);
  connect(pollTimer, &QTimer::timeout, system, &System::propagateCurrentState);
  // pollTimer->start(1000);

  connect(cpuWidget, &CpuWidget::programCounterChangeRequested, system, &System::changeProgramCounter);
  connect(cpuWidget, &CpuWidget::singleStepExecutionRequested, system, &System::executeSingleStep);

  connect(system, &System::cpuStateChanged, cpuWidget, &CpuWidget::updateState);
  connect(system, &System::memoryContentChanged, cpuWidget, &CpuWidget::updateMemoryContent);

  system->propagateCurrentState();
}

MainWindow::~MainWindow() {
  delete ui;
}
