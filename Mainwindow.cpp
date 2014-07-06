#include "Mainwindow.h"
#include "ui_Mainwindow.h"
#include <iostream>
#include "Utilities.h"

using namespace std;
bool running = false;
#define SIZE 1300

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	_ui(new Ui::MainWindow),
	_delay (this),
	_aliveColor(255, 255, 255),
	_deadColor(0,0,0),
	data (new uchar[SIZE*SIZE]),
	_render(data, SIZE, SIZE, QImage::Format_Indexed8),
	_nbCellLabel (new QLabel("Nb de cellules en vie :")),
	_nbGenerationLabel (new QLabel("Nb de générations :"))
{
	_ui->setupUi(this);
	this->setWindowTitle("Simulation du jeu de la vie");
	this->setWindowIcon (QIcon(QString("./icon.icns")));

	_grid = new Grid (SIZE,SIZE);

	_grid->generateRandomGrid(_ui->randomDoubleSpin->value());
	_ui->statusBar->addWidget(_nbCellLabel);
	_ui->statusBar->addWidget(_nbGenerationLabel);
    _delay.setInterval(1);

	connect (_ui->startStopButton, SIGNAL(clicked()),this, SLOT(startStopAnimation()));
	connect (&_delay, SIGNAL(timeout()), this, SLOT(updateScene()));
	connect (_ui->speedSlider, SIGNAL(valueChanged(int)), this, SLOT(changeDelay(int)));
	connect (_ui->reinitButton, SIGNAL(clicked()), this, SLOT(changeRandom()));

	_ui->sceneGV->setScene (&_scene);
	_ui->sceneGV->show ();
	changeRandom ();
}


void MainWindow::updateScene()
{
	#pragma omp parallel for
	for (int i = 0; i < SIZE*SIZE; i++)
			data[i] =  (_grid->getState (i%SIZE, i /SIZE) == ALIVE)? 255 : 0;

	_monPixmap = QPixmap::fromImage(_render);
	_scene.clear ();
	_scene.addPixmap (_monPixmap);
	_grid->nextGeneration ();
	_nbCellLabel->setText("Nb de cellules en vie : " + QString::number(_grid->getNbCellAlive()));
    _nbGenerationLabel->setText("Nb de générations : " + QString::number(_grid->getNbGenerations ()));
}

void MainWindow::changeDelay (int newDelay){
	_delay.setInterval (991-(newDelay*10));

}

void MainWindow::startStopAnimation(){

	if (running){
		_delay.stop ();
		_ui->startStopButton->setText ("Démarrer la simulation");
		_ui->actionStartStop->setText ("Démarrer la simulation");
	}
	else {
		_delay.start ();
		_ui->startStopButton->setText ("Stopper la simulation");
		_ui->actionStartStop->setText ("Stopper la simulation");
	}
	running = !running;
}



void MainWindow::changeRandom(){

    _grid->generateRandomGrid(_ui->randomDoubleSpin->value()/100);
	updateScene ();
}

MainWindow::~MainWindow()
{
	delete _ui;
	delete[] data;
}
