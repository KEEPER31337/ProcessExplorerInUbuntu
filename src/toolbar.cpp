#include <QMainWindow>
#include <QApplication>
#include <QToolBar>
#include <QIcon>
#include <QAction>

class MainWindow : public QMainWindow {

  public:
    MainWindow(QWidget *parent = nullptr);
};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent) {

  //QPixmap newpix("add-file.png");
  //QPixmap openpix("open-file-button.png");
  //QPixmap quitpix("logout.png");
  QPixmap savepix("diskette.png");
  QPixmap findpix("find.png");
  QPixmap killpix("kill.png");
  QPixmap sysinfopix("audio-waves.png");

  QToolBar *toolbar = addToolBar("main toolbar");
  //toolbar->addAction(QIcon(newpix), "New File");
  //toolbar->addAction(QIcon(openpix), "Open File");
  toolbar->addAction(QIcon(savepix), "Save");
  toolbar->addSeparator();
  toolbar->addAction(QIcon(sysinfopix), "System Information");
  toolbar->addSeparator();
  toolbar->addAction(QIcon(killpix), "Kill Process");
  toolbar->addSeparator();
  toolbar->addAction(QIcon(findpix), "Find Process");

  toolbar->addSeparator();

  //QAction *quit = toolbar->addAction(QIcon(quitpix),"Quit Application");

  //connect(quit, &QAction::triggered, qApp, &QApplication::quit);
}

int main(int argc, char *argv[]) {

  QApplication app(argc, argv);

  MainWindow window;

  window.resize(350, 250);
  window.setWindowTitle("QToolBar");
  window.show();

  return app.exec();
}
