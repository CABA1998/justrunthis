#pragma once

#include <QElapsedTimer>
#include <QObject>
#include <QProcess>
#include <QString>
#include <QTimer>

class CppRunner : public QObject {
    Q_OBJECT

public:
    explicit CppRunner(QObject *parent = nullptr);

    void run(const QString &sourceFile, const QString &stdinData, bool interactive = false);
    void sendLine(const QString &line);
    void stop();
    bool isRunning() const { return m_compiler != nullptr || m_runner != nullptr; }

signals:
    void statusChanged(const QString &status);
    void stdoutReceived(const QString &text);
    void stderrReceived(const QString &text);
    void finished(bool success, int exitCode, qint64 elapsedMs);

private:
    void startCompile();
    void startRun();
    void cleanup();

    QString m_sourceFile;
    QString m_binaryPath;
    QString m_stdinData;
    bool m_interactive = false;
    QProcess *m_compiler = nullptr;
    QProcess *m_runner = nullptr;
    QElapsedTimer m_timer;
    QTimer *m_timeoutTimer = nullptr;
    bool m_timedOut = false;

    static constexpr int kTimeoutMs = 5000;
};
