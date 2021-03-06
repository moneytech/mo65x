#include "assemblerwidget.h"
#include "assembler.h"
#include "commonformatters.h"
#include "ui_assemblerwidget.h"
#include "uitools.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QMetaEnum>
#include <QMetaType>
#include <QTextBlock>
#include <QTextStream>

AssemblerWidget::AssemblerWidget(QWidget* parent, Memory& memory)
    : QWidget(parent), ui(new Ui::AssemblerWidget), assembler(memory) {
  ui->setupUi(this);
  connect(ui->newFile, &QAbstractButton::clicked, this, &AssemblerWidget::newFile);
  connect(ui->loadFile, &QAbstractButton::clicked, this, &AssemblerWidget::loadEditorFile);
  connect(ui->saveFile, &QAbstractButton::clicked, this, &AssemblerWidget::saveEditorFile);
  connect(ui->saveFileAs, &QAbstractButton::clicked, this, &AssemblerWidget::saveEditorFileAs);
  connect(ui->assembleSourceCode, &QAbstractButton::clicked, this, &AssemblerWidget::assembleSourceCode);
  connect(ui->goToOrigin, &QAbstractButton::clicked, [&] { emit programCounterChanged(assembler.affectedAddressRange().first); });

  setMonospaceFont(ui->sourceCode);
}

AssemblerWidget::~AssemblerWidget() {
  delete ui;
}

void AssemblerWidget::loadFile(const QString& fname) {
  QFile file(fname);
  if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    ui->sourceCode->setPlainText(QTextStream(&file).readAll());
    fileName = fname;
    emit fileLoaded(fname);
  }
}

void AssemblerWidget::saveFile(const QString& fname) {
  QFile file(fname);
  if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    QTextStream(&file) << ui->sourceCode->toPlainText();
    fileName = fname;
    emit fileSaved(fname);
  }
}

void AssemblerWidget::loadEditorFile() {
  QFileDialog::getOpenFileContent("", [&](const QString fname, const QByteArray& fileContent) {
    QString title = tr("Load File");
    if (fname.isEmpty()) {
      QMessageBox::warning(this, title, tr("loading error"));
    } else {
      ui->sourceCode->setPlainText(fileContent);
      fileName = fname;
      emit fileLoaded(fname);
    }
  });
}

void AssemblerWidget::saveEditorFile() {
  if (fileName.isEmpty()) fileName = QFileDialog::getSaveFileName(this, "", tr("Save File"));
  if (!fileName.isEmpty()) saveFile(fileName);
}

void AssemblerWidget::saveEditorFileAs() {
  if (auto fname = QFileDialog::getSaveFileName(this, tr("Save File"), QFileInfo(fileName).absolutePath()); !fname.isEmpty())
    saveFile(fname);
}

std::optional<QString> AssemblerWidget::process() {
  QString src = ui->sourceCode->toPlainText();
  QTextStream is(&src, QIODevice::ReadOnly);
  int lineNum = 0;
  while (!is.atEnd()) {
    const auto line = is.readLine();
    if (line.isNull()) break;
    if (auto result = assembler.processLine(line); result != AssemblyResult::Ok) {
      auto block = ui->sourceCode->document()->findBlockByLineNumber(lineNum);
      auto cursor = ui->sourceCode->textCursor();
      cursor.setPosition(block.position() + block.length() - 1, QTextCursor::MoveAnchor);
      cursor.setPosition(block.position(), QTextCursor::KeepAnchor);
      ui->sourceCode->setTextCursor(cursor);
      ui->sourceCode->setFocus();
      return tr("%1 at line %2").arg(formatAssemblyResult(result)).arg(lineNum + 1);
    }
    lineNum++;
  }
  return std::nullopt;
}

void AssemblerWidget::newFile() {
  fileName.clear();
  ui->sourceCode->clear();
  emit newFileCreated();
}

void AssemblerWidget::assembleSourceCode() {
  assembler.init();
  assembler.changeMode(Assembler::ProcessingMode::ScanForSymbols);
  if (auto ph0 = process()) {
    emit operationCompleted(*ph0, false);
    return;
  }

  assembler.initPreserveSymbols();
  assembler.changeMode(Assembler::ProcessingMode::EmitCode);
  if (auto ph1 = process()) {
    emit operationCompleted(*ph1, false);
    return;
  }

  emit codeWritten(assembler.affectedAddressRange());
  emit programCounterChanged(assembler.affectedAddressRange().first);
  emit operationCompleted(tr("%1 B written in range $%2-$%3, symbols: %4")
                              .arg(assembler.bytesWritten())
                              .arg(formatHexWord(assembler.affectedAddressRange().first))
                              .arg(formatHexWord(assembler.affectedAddressRange().last))
                              .arg(assembler.symbols().size()));
}
