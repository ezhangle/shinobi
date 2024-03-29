#include "config.h"
#include <QApplication>
#include <QIcon>
#include <QSettings>
#include "Shinobi.h"

int main(int argc, char** argv) {
  QApplication app(argc, argv);
  app.setQuitOnLastWindowClosed(false);
  app.setWindowIcon(QIcon(":/icon_shinobi.png"));

  QSettings settings("shinobi.ini", QSettings::IniFormat);
  shinobi::Shinobi shinobi(&settings);
  return app.exec();
}