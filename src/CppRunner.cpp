#include "CppRunner.h"

#include <QCryptographicHash>
#include <QDir>
#include <QFileInfo>
#include <QStandardPaths>
#include <QStringList>

CppRunner::CppRunner(QObject *parent) : QObject(parent) {}

void CppRunner::run(const QString &sourceFile, const QString &stdinData) {
    if (isRunning()) return;

    m_sourceFile = sourceFile;
    m_stdinData = stdinData;
    m_timedOut = false;

    QString cacheDir = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
    if (cacheDir.isEmpty()) {
        cacheDir = QDir::tempPath() + "/justrunthis";
    }
    QDir().mkpath(cacheDir);

    const QFileInfo info(sourceFile);
    const QByteArray hash = QCryptographicHash::hash(
        sourceFile.toUtf8(), QCryptographicHash::Md5).toHex().left(8);
    m_binaryPath = cacheDir + "/" + info.baseName() + "_" + QString::fromLatin1(hash) + ".exe";

    startCompile();
}

void CppRunner::startCompile() {
    emit statusChanged("Compiling...");
    m_timer.start();

    m_compiler = new QProcess(this);

    connect(m_compiler, &QProcess::readyReadStandardError, this, [this]() {
        emit outputReceived(QString::fromLocal8Bit(m_compiler->readAllStandardError()));
    });
    connect(m_compiler, &QProcess::readyReadStandardOutput, this, [this]() {
        emit outputReceived(QString::fromLocal8Bit(m_compiler->readAllStandardOutput()));
    });
    connect(m_compiler, &QProcess::errorOccurred, this,
            [this](QProcess::ProcessError err) {
                if (err != QProcess::FailedToStart) return;
                emit outputReceived(
                    "ERROR: Failed to start g++. Is MinGW in your PATH?\n"
                    "Typical path: C:\\Qt\\Tools\\mingw1310_64\\bin\n");
                emit statusChanged("g++ not found");
                emit finished(false, -1, m_timer.elapsed());
                cleanup();
            });
    connect(m_compiler,
            QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this,
            [this](int exitCode, QProcess::ExitStatus status) {
                Q_UNUSED(status);
                if (exitCode != 0) {
                    emit statusChanged(
                        QString("Compile failed (exit %1)").arg(exitCode));
                    emit finished(false, exitCode, m_timer.elapsed());
                    cleanup();
                    return;
                }
                m_compiler->deleteLater();
                m_compiler = nullptr;
                startRun();
            });

    const QStringList args = {
        "-std=c++17", "-O2", "-Wall",
        m_sourceFile, "-o", m_binaryPath
    };
    m_compiler->start("g++", args);
}

void CppRunner::startRun() {
    emit statusChanged("Running...");
    m_timer.restart();

    m_runner = new QProcess(this);
    m_runner->setProcessChannelMode(QProcess::MergedChannels);

    connect(m_runner, &QProcess::readyReadStandardOutput, this, [this]() {
        emit outputReceived(QString::fromLocal8Bit(m_runner->readAllStandardOutput()));
    });
    connect(m_runner, &QProcess::started, this, [this]() {
        if (!m_stdinData.isEmpty()) {
            m_runner->write(m_stdinData.toUtf8());
        }
        m_runner->closeWriteChannel();
    });
    connect(m_runner, &QProcess::errorOccurred, this,
            [this](QProcess::ProcessError err) {
                if (err != QProcess::FailedToStart) return;
                emit outputReceived("ERROR: Failed to start compiled binary.\n");
                emit statusChanged("Failed to start");
                emit finished(true, -1, m_timer.elapsed());
                cleanup();
            });
    connect(m_runner,
            QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this,
            [this](int exitCode, QProcess::ExitStatus status) {
                const qint64 elapsed = m_timer.elapsed();
                if (m_timeoutTimer) {
                    m_timeoutTimer->stop();
                    m_timeoutTimer->deleteLater();
                    m_timeoutTimer = nullptr;
                }
                QString msg;
                if (m_timedOut) {
                    msg = QString("Timeout — killed after %1 ms").arg(elapsed);
                } else if (status == QProcess::CrashExit) {
                    msg = QString("Crashed (%1 ms)").arg(elapsed);
                } else if (exitCode != 0) {
                    msg = QString("Exited with %1 (%2 ms)").arg(exitCode).arg(elapsed);
                } else {
                    msg = QString("Done in %1 ms").arg(elapsed);
                }
                emit statusChanged(msg);
                emit finished(true, exitCode, elapsed);
                cleanup();
            });

    m_runner->start(m_binaryPath);

    m_timeoutTimer = new QTimer(this);
    m_timeoutTimer->setSingleShot(true);
    connect(m_timeoutTimer, &QTimer::timeout, this, [this]() {
        if (m_runner && m_runner->state() != QProcess::NotRunning) {
            m_timedOut = true;
            m_runner->kill();
        }
    });
    m_timeoutTimer->start(kTimeoutMs);
}

void CppRunner::cleanup() {
    if (m_compiler) {
        m_compiler->deleteLater();
        m_compiler = nullptr;
    }
    if (m_runner) {
        m_runner->deleteLater();
        m_runner = nullptr;
    }
    if (m_timeoutTimer) {
        m_timeoutTimer->deleteLater();
        m_timeoutTimer = nullptr;
    }
}
