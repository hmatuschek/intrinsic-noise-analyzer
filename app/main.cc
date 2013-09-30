#include "main.hh"

#include "doctree/documenttree.hh"
#include "doctree/documentitem.hh"
#include "utils/logger.hh"


using namespace std;

class DebugApplication: public QApplication
{
public:
  DebugApplication(int &argc, char **argv)
    : QApplication(argc, argv) { }

  virtual bool notify(QObject *obj, QEvent *evt) {
    try {
      return QApplication::notify(obj, evt);
    } catch (std::exception &err) {
      std::cerr << "Caught exception: " << err.what()
                << ". Exit..." << std::endl;
      this->exit(-1);
    }
    return false;
  }
};


int main(int argc, char *argv[])
{
  // Instantiate Logger and handler:
  iNA::Utils::Logger::get().addHandler(
        new iNA::Utils::TextMessageHandler(std::cerr, iNA::Utils::Message::DEBUG));

  // Instantiate a QApplication
  //QApplication qapp(argc, argv);
  DebugApplication qapp(argc, argv);

  // Instantiate our own application model, holds all the data of the running application
  Application *app = Application::getApp();

  // Create new main-window, this initializes all the view-part
  MainWindow *window = new MainWindow();
  app->setMainWindow(window);

  window->show();

  // If there is a SBML file passed as CMD line argument:
  if (2 <= argc) {
    app->docTree()->addDocument(new DocumentItem(argv[1]));
  }

  // Check for new version of iNA:
  app->checkForNewVersion();

  // run.
  qapp.exec();

  // On exit -> destroy application model (frees all data):
  Application::shutdown();

  // Deactivate logger.
  iNA::Utils::Logger::shutdown();

  return 0;
}
