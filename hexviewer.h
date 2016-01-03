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
    void loadData(QByteArray *data);

private slots:
    void on_openButton_clicked();
    void on_startButton_clicked();
    void on_quitButton_clicked();

private:
    Ui::HexViewer *_ui;
    FileReader *_fileReader;
    void updateFromFileReaderState(FileReader::State state);
};

#endif // HEXVIEWER_H
