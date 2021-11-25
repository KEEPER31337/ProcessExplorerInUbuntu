#include <QWidget>
#include <QApplication>
#include <QLabel>
#include <QGridLayout>
#include <vector>
#include "command.h"

using namespace Qt;
using namespace std;

vector<ProcInfo> vec;

class MainWindow : public QWidget {

  public:
    MainWindow(QWidget *parent = nullptr);
};

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent) {

  auto *grid = new QGridLayout(this);

	QLabel *pid = new QLabel("pid",this);
	QLabel *ppid = new QLabel("ppid", this);
	QLabel *comm = new QLabel("comm", this);

	grid->addWidget(pid,0,0);
	grid->addWidget(ppid,0,1);
	grid->addWidget(comm,0,2);


  for(unsigned long i=0; i<vec.size(); i++){
		QLabel *pid = new QLabel(QString::number(vec[i].pid),this);
		QLabel *ppid = new QLabel(QString::number(vec[i].ppid), this);
		QLabel *comm = new QLabel(QString::fromStdString(vec[i].comm), this);


		grid->addWidget(pid,i+1,0);
		grid->addWidget(ppid,i+1,1);
		grid->addWidget(comm,i+1,2);
	}


	setLayout(grid);

}

int main(int argc, char *argv[]) {
	Command cm;
	cm.UpdateProcStat();
	vec = cm.GetProcInfo();
  QApplication app(argc, argv);

  MainWindow window;

  window.resize(300, 190);
  window.setWindowTitle("Plus minus");
  window.show();

  return app.exec();
}
