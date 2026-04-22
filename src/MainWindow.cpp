#include "MainWindow.h"

#include <QAction>
#include <QComboBox>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QFileSystemModel>
#include <QFont>
#include <QFontDatabase>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMenuBar>
#include <QPushButton>
#include <QSplitter>
#include <QStackedWidget>
#include <QTextCursor>
#include <QTextEdit>
#include <QTreeView>
#include <QVBoxLayout>
#include <QWidget>

#include "CppRunner.h"

namespace {
constexpr int kTextMode = 0;
constexpr int kInteractiveMode = 1;
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      m_fsModel(new QFileSystemModel(this)),
      m_tree(new QTreeView(this)),
      m_modeCombo(new QComboBox(this)),
      m_inputStack(new QStackedWidget(this)),
      m_input(new QTextEdit(this)),
      m_console(new QTextEdit(this)),
      m_consoleInput(new QLineEdit(this)),
      m_stdoutWrap(nullptr),
      m_stdout(new QTextEdit(this)),
      m_stderr(new QTextEdit(this)),
      m_runButton(new QPushButton("Run", this)),
      m_status(new QLabel("Ready — open a folder to start", this)),
      m_workspaceLabel(new QLabel("(no folder opened)", this)),
      m_runner(new CppRunner(this)) {
    setWindowTitle("justrunthis");
    resize(1100, 700);

    m_tree->setModel(m_fsModel);
    m_tree->setHeaderHidden(true);
    m_tree->hideColumn(1);
    m_tree->hideColumn(2);
    m_tree->hideColumn(3);
    m_tree->setAnimated(true);
    m_tree->setMinimumWidth(220);

    const QFont mono = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    m_input->setFont(mono);
    m_console->setFont(mono);
    m_consoleInput->setFont(mono);
    m_stdout->setFont(mono);
    m_stderr->setFont(mono);
    m_stdout->setReadOnly(true);
    m_stderr->setReadOnly(true);
    m_console->setReadOnly(true);
    m_input->setPlaceholderText("Paste stdin here (Codeforces sample input).");
    m_stdout->setPlaceholderText("Program stdout will appear here after you click Run.");
    m_stderr->setPlaceholderText("Compiler errors and stderr output will appear here.");
    m_console->setPlaceholderText("Interactive console — program stdout appears here.");
    m_consoleInput->setPlaceholderText("Type a line and press Enter to send to the program.");

    const QString outputStyle =
        "QTextEdit { background: #ffffff; color: #222222; "
        "selection-background-color: #3a6ea5; selection-color: #ffffff; "
        "border: 1px solid #cccccc; padding: 4px; }";
    const QString stderrStyle =
        "QTextEdit { background: #ffffff; color: #c0392b; "
        "selection-background-color: #3a6ea5; selection-color: #ffffff; "
        "border: 1px solid #e0b0b0; padding: 4px; }";
    m_stdout->setStyleSheet(outputStyle);
    m_stderr->setStyleSheet(stderrStyle);
    m_console->setStyleSheet(outputStyle);

    m_modeCombo->addItem("TXT (stdin from box)");
    m_modeCombo->addItem("Interactive console");

    auto *textModePage = new QWidget;
    auto *textModeLayout = new QVBoxLayout(textModePage);
    textModeLayout->setContentsMargins(0, 0, 0, 0);
    textModeLayout->addWidget(m_input);

    auto *interactivePage = new QWidget;
    auto *interactiveLayout = new QVBoxLayout(interactivePage);
    interactiveLayout->setContentsMargins(0, 0, 0, 0);
    interactiveLayout->setSpacing(4);
    interactiveLayout->addWidget(m_console, 1);
    interactiveLayout->addWidget(m_consoleInput);

    m_inputStack->addWidget(textModePage);
    m_inputStack->addWidget(interactivePage);
    m_inputStack->setCurrentIndex(kTextMode);

    auto *inputWrap = new QWidget;
    auto *inputLayout = new QVBoxLayout(inputWrap);
    inputLayout->setContentsMargins(0, 0, 0, 0);
    auto *inputHeader = new QHBoxLayout;
    inputHeader->setContentsMargins(0, 0, 0, 0);
    inputHeader->addWidget(new QLabel("Input (stdin)"));
    inputHeader->addStretch(1);
    inputHeader->addWidget(new QLabel("Mode:"));
    inputHeader->addWidget(m_modeCombo);
    inputLayout->addLayout(inputHeader);
    inputLayout->addWidget(m_inputStack);

    auto *controlsWrap = new QWidget;
    auto *controlsLayout = new QHBoxLayout(controlsWrap);
    controlsLayout->setContentsMargins(0, 0, 0, 0);
    controlsLayout->addWidget(m_runButton);
    controlsLayout->addWidget(m_status, 1);
    m_runButton->setEnabled(false);

    m_stdoutWrap = new QWidget;
    auto *stdoutLayout = new QVBoxLayout(m_stdoutWrap);
    stdoutLayout->setContentsMargins(0, 0, 0, 0);
    auto *stdoutLabel = new QLabel("stdout");
    stdoutLabel->setStyleSheet("QLabel { font-weight: bold; padding: 2px 4px; }");
    stdoutLayout->addWidget(stdoutLabel);
    stdoutLayout->addWidget(m_stdout);

    auto *stderrWrap = new QWidget;
    auto *stderrLayout = new QVBoxLayout(stderrWrap);
    stderrLayout->setContentsMargins(0, 0, 0, 0);
    auto *stderrLabel = new QLabel("stderr");
    stderrLabel->setStyleSheet(
        "QLabel { font-weight: bold; color: #c0392b; padding: 2px 4px; }");
    stderrLayout->addWidget(stderrLabel);
    stderrLayout->addWidget(m_stderr);

    auto *rightSplitter = new QSplitter(Qt::Vertical);
    rightSplitter->addWidget(inputWrap);
    rightSplitter->addWidget(controlsWrap);
    rightSplitter->addWidget(m_stdoutWrap);
    rightSplitter->addWidget(stderrWrap);
    rightSplitter->setStretchFactor(0, 2);
    rightSplitter->setStretchFactor(1, 0);
    rightSplitter->setStretchFactor(2, 3);
    rightSplitter->setStretchFactor(3, 2);
    rightSplitter->setCollapsible(1, false);

    m_workspaceLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    m_workspaceLabel->setStyleSheet(
        "QLabel { padding: 4px 6px; background: palette(alternate-base); "
        "border-bottom: 1px solid palette(mid); font-weight: bold; }");
    m_workspaceLabel->setWordWrap(false);

    auto *treeWrap = new QWidget;
    auto *treeLayout = new QVBoxLayout(treeWrap);
    treeLayout->setContentsMargins(0, 0, 0, 0);
    treeLayout->setSpacing(0);
    treeLayout->addWidget(m_workspaceLabel);
    treeLayout->addWidget(m_tree);

    auto *mainSplitter = new QSplitter(Qt::Horizontal);
    mainSplitter->addWidget(treeWrap);
    mainSplitter->addWidget(rightSplitter);
    mainSplitter->setStretchFactor(0, 1);
    mainSplitter->setStretchFactor(1, 3);
    mainSplitter->setCollapsible(0, false);
    mainSplitter->setCollapsible(1, false);
    mainSplitter->setSizes({280, 820});

    auto *central = new QWidget;
    auto *centralLayout = new QVBoxLayout(central);
    centralLayout->setContentsMargins(10, 10, 10, 10);
    centralLayout->addWidget(mainSplitter);
    setCentralWidget(central);

    auto *fileMenu = menuBar()->addMenu("&File");
    auto *openAction = fileMenu->addAction("&Open Folder...");
    openAction->setShortcut(QKeySequence::Open);
    fileMenu->addSeparator();
    auto *quitAction = fileMenu->addAction("&Quit");
    quitAction->setShortcut(QKeySequence::Quit);

    connect(openAction, &QAction::triggered, this, &MainWindow::openFolder);
    connect(quitAction, &QAction::triggered, this, &QWidget::close);
    connect(m_tree, &QTreeView::clicked, this, &MainWindow::onFileClicked);
    connect(m_runButton, &QPushButton::clicked, this, &MainWindow::runCurrent);
    connect(m_modeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onModeChanged);
    connect(m_consoleInput, &QLineEdit::returnPressed,
            this, &MainWindow::onInteractiveLineEntered);
    connect(m_runner, &CppRunner::statusChanged, this, &MainWindow::onStatusChanged);
    connect(m_runner, &CppRunner::stdoutReceived, this, &MainWindow::onStdoutReceived);
    connect(m_runner, &CppRunner::stderrReceived, this, &MainWindow::onStderrReceived);
    connect(m_runner, &CppRunner::finished, this, &MainWindow::onRunFinished);

    onModeChanged(kTextMode);
}

bool MainWindow::isInteractiveMode() const {
    return m_modeCombo->currentIndex() == kInteractiveMode;
}

void MainWindow::openFolder() {
    const QString dir = QFileDialog::getExistingDirectory(
        this, "Open Folder", QDir::homePath());
    if (dir.isEmpty()) return;

    m_fsModel->setRootPath(dir);
    const QModelIndex idx = m_fsModel->index(dir);
    m_tree->setRootIndex(idx);
    m_currentFile.clear();
    m_runButton->setEnabled(false);
    m_workspaceLabel->setText(QDir::toNativeSeparators(dir));
    m_workspaceLabel->setToolTip(QDir::toNativeSeparators(dir));
    setWindowTitle("justrunthis — " + QDir(dir).dirName());
    m_status->setText("Ready — pick a .cpp file");
}

void MainWindow::onFileClicked(const QModelIndex &index) {
    const QString path = m_fsModel->filePath(index);
    const QFileInfo info(path);
    if (info.isFile() && info.suffix().compare("cpp", Qt::CaseInsensitive) == 0) {
        m_currentFile = path;
        m_runButton->setEnabled(!m_running);
        m_status->setText("Selected: " + info.fileName());
    } else {
        m_currentFile.clear();
        m_runButton->setEnabled(false);
    }
}

void MainWindow::runCurrent() {
    if (m_running) {
        m_runner->stop();
        return;
    }
    if (m_currentFile.isEmpty()) return;

    m_stdout->clear();
    m_stderr->clear();
    m_console->clear();
    m_consoleInput->clear();

    const bool interactive = isInteractiveMode();
    m_running = true;
    if (interactive) {
        m_runButton->setText("Stop");
        m_consoleInput->setEnabled(true);
        m_consoleInput->setFocus();
    } else {
        m_runButton->setEnabled(false);
    }

    m_runner->run(m_currentFile, interactive ? QString() : m_input->toPlainText(), interactive);
}

void MainWindow::onModeChanged(int index) {
    m_inputStack->setCurrentIndex(index);
    const bool interactive = (index == kInteractiveMode);
    m_stdoutWrap->setVisible(!interactive);
    m_consoleInput->setEnabled(interactive && m_running);
}

void MainWindow::onInteractiveLineEntered() {
    if (!m_running || !isInteractiveMode()) return;
    const QString line = m_consoleInput->text();
    m_console->moveCursor(QTextCursor::End);
    m_console->insertPlainText(line + "\n");
    m_console->moveCursor(QTextCursor::End);
    m_runner->sendLine(line);
    m_consoleInput->clear();
}

void MainWindow::onStatusChanged(const QString &status) {
    m_status->setText(status);
}

void MainWindow::onStdoutReceived(const QString &text) {
    QTextEdit *target = isInteractiveMode() ? m_console : m_stdout;
    target->moveCursor(QTextCursor::End);
    target->insertPlainText(text);
    target->moveCursor(QTextCursor::End);
}

void MainWindow::onStderrReceived(const QString &text) {
    m_stderr->moveCursor(QTextCursor::End);
    m_stderr->insertPlainText(text);
    m_stderr->moveCursor(QTextCursor::End);
}

void MainWindow::onRunFinished(bool, int, qint64) {
    m_running = false;
    m_runButton->setText("Run");
    m_runButton->setEnabled(!m_currentFile.isEmpty());
    m_consoleInput->setEnabled(false);
}
