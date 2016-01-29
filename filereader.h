#ifndef FILEREADER_H
#define FILEREADER_H

#include <QObject>
#include <QPair>
#include <QThread>
#include <QMutex>

/* A FileReader reads data from a file and periodically emits a signal
 * containing this data.
 *
 * It can be in these states:
 *   - IDLE: no activity, no filename
 *   - READY: no activity, filename is set
 *   - STARTED: reading from the file previously specified
 *   - ERROR: an error has occurred
 *
 * The FileReader can only transition IDLE to READY (when a file name is set),
 * READY to STARTED (when the startReading() slot is activated), and STARTED to
 * IDLE when the end of the file is reached.
 *
 * It can transition to ERROR at any time, and cannot transition out of it.
 *
 * The full state of the FileReader is a tuple of the reading state and the
 * file name. The file name can only be non-null if the reading state is READY.
 * 
 */
class FileReader : public QObject
{
    Q_OBJECT

public:
    enum ReadingState
    {
        IDLE,
        READY,
        STARTED,
        ERROR
    };

    typedef QPair<ReadingState, QString> State;

    explicit FileReader(QObject *parent = 0);
    ~FileReader();

    State state();

signals:
    void dataRead(QByteArray data);
    void stateChanged(FileReader::State newState);

public slots:
    void fileSelected(QString fileName);
    void startReading();

private:
    /* The mutex for the "reading state"/filename state of the FileReader. */
    QMutex _stateLock;
    State _state;

    /* Checks whether the old reading state matches the desired new state, and
     * returns true if and only if they match. The caller needs to implement the
     * transition logic.
     */
    bool changeState(ReadingState oldState, State newState);
};

#endif // FILEREADER_H
