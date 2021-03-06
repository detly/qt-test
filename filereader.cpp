#include <assert.h>

#include <QDebug>
#include <QFile>
#include <QThread>

#include "filereader.h"

FileReader::FileReader(unsigned int depth, QObject *parent) :
    QObject(parent),
    _stateLock(),
    _state(IDLE, QString()),
    _dataCounter(depth)
{
}

void FileReader::fileSelected(QString fileName)
{
    changeState(IDLE, State(READY, fileName));
}

void FileReader::startReading()
{
    static const unsigned DATA_SIZE = 256;

    unsigned debugCounter = 0;

    QString fileName = state().second;

    if (changeState(READY, State(STARTED, QString())))
    {
        {
            QFile inFile(fileName);

            if (!inFile.open(QIODevice::ReadOnly))
            {
                changeState(STARTED, State(ERROR, QString()));
                return;
            }

            while(!inFile.atEnd())
            {
                QByteArray qa = inFile.read(DATA_SIZE);
                _dataCounter.acquire();
                qDebug() << "emitting dataRead(): "
                         << debugCounter
                         << " / "
                         << _dataCounter.available();

                emit dataRead(qa);
                debugCounter++;
            }
        }
        
        changeState(STARTED, State(IDLE, QString()));
    }
}

void FileReader::processedChunk()
{
    qDebug() << "called processedChunk()";
    _dataCounter.release();
    qDebug() << "released data counter: " << _dataCounter.available();
}

FileReader::~FileReader()
{
}

FileReader::State FileReader::state()
{
    QMutexLocker locker(&_stateLock);
    return _state;
}

bool FileReader::changeState(ReadingState oldRState, State newState)
{   
    ReadingState newRState = newState.first;
    QString fileName = newState.second;
    bool changed = false;
    bool valid;

    /* It only makes sense to have a file name if the reading state is being set
     * to READY and any other state should have no file name.
     */
    assert((newRState == READY) == (!fileName.isNull()));

    /* Only note a change in _state, not _fileName, since we can't transition
     * from READY to READY.
     */
    {
        QMutexLocker locker(&_stateLock);
        valid = oldRState == _state.first;

        /* Only change state if the oldState matches what we expected. */
        if (valid)
        {
            changed = newState != _state;
            _state = newState;
        }
    }

    if (changed)
    {    
        emit stateChanged(newState);
    }

    return valid;
}
