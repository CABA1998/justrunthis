#pragma once

#include <QMainWindow>
#include <QModelIndex>

class QTreeView;
class QFileSystemModel;
class QTextEdit;
class QPushButton;
class QLabel;
class CppRunner;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void openFolder();
    void onFileClicked(const QModelIndex &index);
    void runCurrent();
    void onStatusChanged(const QString &status);
    void onOutputReceived(const QString &text);
    void onRunFinished(bool success, int exitCode, qint64 elapsedMs);

private:
    QFileSystemModel *m_fsModel;
    QTreeView *m_tree;
    QTextEdit *m_input;
    QTextEdit *m_output;
    QPushButton *m_runButton;
    QLabel *m_status;
    QLabel *m_workspaceLabel;
    CppRunner *m_runner;
    QString m_currentFile;
};
