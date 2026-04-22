#include "MainWindow.h"

#include <QAction>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QFileSystemModel>
#include <QFont>
#include <QFontDatabase>
#include <QHBoxLayout>
#include <QLabel>
#include <QMenuBar>
#include <QPushButton>
#include <QSplitter>
#include <QTextCursor>
#include <QTextEdit>
#include <QTreeView>
#include <QVBoxLayout>
#include <QWidget>

#include "CppRunner.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      m_fsModel(new QFileSystemModel(this)),
      m_tree(new QTreeView(this)),
      m_input(new QTextEdit(this)),
      m_output(new QTextEdit(this)),
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
    m_output->setFont(mono);
    m_output->setReadOnly(true);
    m_output->setPlaceholderText("Output will appear here after you click Run.");
    m_input->setPlaceholderText("Paste stdin here (Codeforces sample input).");

    auto *inputWrap = new QWidget;
    auto *inputLayout = new QVBoxLayout(inputWrap);
    inputLayout->setContentsMargins(0, 0, 0, 0);
    inputLayout->addWidget(new QLabel("Input (stdin)"));
    inputLayout->addWidget(m_input);

    auto *controlsWrap = new QWidget;
    auto *controlsLayout = new QHBoxLayout(controlsWrap);
    controlsLayout->setContentsMargins(0, 0, 0, 0);
    controlsLayout->addWidget(m_runButton);
    controlsLayout->addWidget(m_status, 1);
    m_runButton->setEnabled(false);

    auto *outputWrap = new QWidget;
    auto *outputLayout = new QVBoxLayout(outputWrap);
    outputLayout->setContentsMargins(0, 0, 0, 0);
    outputLayout->addWidget(new QLabel("Output"));
    outputLayout->addWidget(m_output);

    auto *rightSplitter = new QSplitter(Qt::Vertical);
    rightSplitter->addWidget(inputWrap);
    rightSplitter->addWidget(controlsWrap);
    rightSplitter->addWidget(outputWrap);
    rightSplitter->setStretchFactor(0, 2);
    rightSplitter->setStretchFactor(1, 0);
    rightSplitter->setStretchFactor(2, 3);
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

    setCentralWidget(mainSplitter);

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
    connect(m_runner, &CppRunner::statusChanged, this, &MainWindow::onStatusChanged);
    connect(m_runner, &CppRunner::outputReceived, this, &MainWindow::onOutputReceived);
    connect(m_runner, &CppRunner::finished, this, &MainWindow::onRunFinished);
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
        m_runButton->setEnabled(!m_runner->isRunning());
        m_status->setText("Selected: " + info.fileName());
    } else {
        m_currentFile.clear();
        m_runButton->setEnabled(false);
    }
}

void MainWindow::runCurrent() {
    if (m_currentFile.isEmpty()) return;
    m_output->clear();
    m_runButton->setEnabled(false);
    m_runner->run(m_currentFile, m_input->toPlainText());
}

void MainWindow::onStatusChanged(const QString &status) {
    m_status->setText(status);
}

void MainWindow::onOutputReceived(const QString &text) {
    m_output->moveCursor(QTextCursor::End);
    m_output->insertPlainText(text);
    m_output->moveCursor(QTextCursor::End);
}

void MainWindow::onRunFinished(bool, int, qint64) {
    m_runButton->setEnabled(!m_currentFile.isEmpty());
}
