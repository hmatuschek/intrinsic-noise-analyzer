#include "main.hh"

#include "doctree/documentitem.hh"
#include "utils/logger.hh"


using namespace std;


int main(int argc, char *argv[])
{
  // Instantiate Logger and handler:
  //Fluc::Utils::Logger::get().addHandler(
  //      new Fluc::Utils::TextMessageHandler(std::cerr, Fluc::Utils::Message::DEBUG));

  // Instantiate a QApplication
  QApplication qapp(argc, argv);

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
