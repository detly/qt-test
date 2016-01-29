#include <assert.h>
#include <iostream>

#include <QObject>
#include <QThread>
#include <QFileDialog>
#include <QFont>
#include <QFontDatabase>

#include "hexviewer.h"
#include "ui_hexviewer.h"

HexViewer::HexViewer(QWidget *parent) :
    QMainWindow(parent),
    _ui(new Ui::HexViewer),
    _fileReader(new FileReader())
{
    /* Initialise threading. */
    qRegisterMetaType<FileReader::State>("FileReader::State");

    QThread *readerThread = new QThread(this);
    readerThread->setObjectName("ReaderThread");

    connect(
        readerThread, &QThread::finished,
        _fileReader,  &QObject::deleteLater
    );

    _fileReader->moveToThread(readerThread);
    readerThread->start();

    /* UI setup. */
    _ui->setupUi(this);

    const QFont fixedFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    _ui->hexTextView->setCurrentFont(fixedFont);

    /* Set initial state. */
    setState(HexViewer::WAITING);

    /* Wire everything up. */
    connect(
        _fileReader, &FileReader::stateChanged,
        this,        &HexViewer::updateFromFileReaderState
    );

    updateFromFileReaderState(_fileReader->state());

    connect(
        _fileReader, &FileReader::dataRead,
        this,        &HexViewer::loadData
    );

    connect(
        this,        &HexViewer::readyForData,
        _fileReader, &FileReader::startReading
    );

    connect(
        this,        &HexViewer::fileSelected,
        _fileReader, &FileReader::fileSelected
    );
}

void HexViewer::setState(HexViewer::UIState state)
{
    bool openEnabled  = state == NEED_FILE || state == CAN_PROCESS;
    bool startEnabled = state == CAN_PROCESS;

    _ui->openButton->setEnabled(openEnabled);
    _ui->startButton->setEnabled(startEnabled);
}

void HexViewer::loadData(QByteArray data)
{
    const int hexLength = 2;

    QString hexString = data.toHex();

    QString renderString;
    renderString.reserve(hexString.size() * 1.5);

    for (int i = 0; i < hexString.length(); i += hexLength)
    {
        renderString.append(hexString.mid(i, hexLength) + " ");
    }
    
    _ui->hexTextView->insertPlainText(renderString);
}

void HexViewer::updateFromFileReaderState(FileReader::State state)
{
    FileReader::ReadingState rState = state.first;
    HexViewer::UIState uiState;

    switch (rState)
    {
        case FileReader::IDLE:
            uiState = HexViewer::NEED_FILE;
            break;

        case FileReader::READY:
            uiState = HexViewer::CAN_PROCESS;
            break;

        case FileReader::STARTED:
            uiState = HexViewer::PROCESSING;
            break;

        case FileReader::ERROR:
            uiState = HexViewer::ERROR;
            break;

        default:
            assert(0);
            break;
    }

    setState(uiState);
}

void HexViewer::on_startButton_clicked()
{
    setState(HexViewer::WAITING);
    emit readyForData();
}

void HexViewer::on_quitButton_clicked()
{
    close();
}

void HexViewer::closeEvent(QCloseEvent *event)
{
    event->accept();

    _fileReader->thread()->quit();
    _fileReader->thread()->wait();

    qApp->quit();
}

void HexViewer::on_openButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Open File");
    if (!fileName.isNull()) {
        emit fileSelected(fileName);
    }
}

HexViewer::~HexViewer()
{
    delete _ui;
}
