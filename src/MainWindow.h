#pragma once

#include <QMainWindow>
#include <QModelIndex>

class QTreeView;
class QFileSystemModel;
class QTextEdit;
class QLineEdit;
class QPushButton;
class QLabel;
class QComboBox;
class QStackedWidget;
class QWidget;
class CppRunner;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void openFolder();
    void onFileClicked(const QModelIndex &index);
    void runCurrent();
    void onModeChanged(int index);
    void onInteractiveLineEntered();
    void onStatusChanged(const QString &status);
    void onStdoutReceived(const QString &text);
    void onStderrReceived(const QString &text);
    void onRunFinished(bool success, int exitCode, qint64 elapsedMs);

private:
    bool isInteractiveMode() const;

    QFileSystemModel *m_fsModel;
    QTreeView *m_tree;
    QComboBox *m_modeCombo;
    QStackedWidget *m_inputStack;
    QTextEdit *m_input;
    QTextEdit *m_console;
    QLineEdit *m_consoleInput;
    QWidget *m_stdoutWrap;
    QTextEdit *m_stdout;
    QTextEdit *m_stderr;
    QPushButton *m_runButton;
    QLabel *m_status;
    QLabel *m_workspaceLabel;
    CppRunner *m_runner;
    QString m_currentFile;
    bool m_running = false;
};
