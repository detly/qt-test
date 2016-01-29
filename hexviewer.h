#ifndef HEXVIEWER_H
#define HEXVIEWER_H

#include <QMainWindow>
#include "filereader.h"

namespace Ui {
class HexViewer;
}

class HexViewer : public QMainWindow
{
    Q_OBJECT

public:
    explicit HexViewer(QWidget *parent = 0);
    ~HexViewer();

    void lock();
    void unlock();

signals:
    void fileSelected(QString fileName);
    void readyForData();

public slots:
    void loadData(QByteArray data);

private slots:
    void on_openButton_clicked();
    void on_startButton_clicked();
    void on_quitButton_clicked();

private:
    /* The states the UI can be in. NEED_FILE means the UI requires a file
     * before any action can be taken. CAN_PROCESS means the UI can start
     * processing the file or accept a new one. ERROR means that an error
     * occurred during processing. WAITING means that the UI has triggered an
     * action in the file reader and is waiting for the state to change.
     */ 
    enum UIState
    {
        NEED_FILE,
        CAN_PROCESS,
        PROCESSING,
        WAITING,
        ERROR
    };

    Ui::HexViewer *_ui;
    FileReader *_fileReader;
    void updateFromFileReaderState(FileReader::State state);
    void setState(UIState state);
};

#endif // HEXVIEWER_H
