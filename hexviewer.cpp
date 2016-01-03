#include <iostream>

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
    _ui->setupUi(this);

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

    const QFont fixedFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    _ui->hexTextView->setCurrentFont(fixedFont);
}

void HexViewer::loadData(QByteArray *data)
{
    QString hexString = data->toHex();

    for (int i = 0; i < hexString.length(); i+=2)
    {
        _ui->hexTextView->insertPlainText(hexString.at(i));
        _ui->hexTextView->insertPlainText(hexString.at(i+1));
        _ui->hexTextView->insertPlainText(" ");
    }
    
    delete data;
}

void HexViewer::updateFromFileReaderState(FileReader::State state)
{
    FileReader::ReadingState rState = state.first;

    bool startEnabled = (rState == FileReader::READY);
    _ui->startButton->setEnabled(startEnabled);

    bool openEnabled = (rState == FileReader::IDLE);
    _ui->openButton->setEnabled(openEnabled);

    if (rState == FileReader::READY)
    {
        _ui->hexTextView->clear();
    }
}

void HexViewer::on_startButton_clicked()
{
    emit readyForData();
}

void HexViewer::on_quitButton_clicked()
{
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
    delete _fileReader;
}
