#include "main.hh"

#include "lna/lnamodule.hh"
#include "steadystate/lnasteadystatemodule.hh"
#include "ssa/ssamodule.hh"
#include "re/remodule.hh"
#include "ios/iosmodule.hh"
#include "doctree/documentitem.hh"
#include "utils/logger.hh"


using namespace std;


int main(int argc, char *argv[])
{
  // Instantiate Logger and handler:
  Fluc::Utils::Logger::get().addHandler(
        new Fluc::Utils::TextMessageHandler(std::cerr, Fluc::Utils::Message::DEBUG));

  // Instantiate a QApplication
  QApplication qapp(argc, argv);

  // Instantiate our own application model, holds all the data of the running application
  Application *app = Application::factory();

  // Create new main-window, this initializes all the view-part
  MainWindow *window = new MainWindow();
  app->setMainWindow(window);

  // Load SteadyState module:
  app->addModule(new LNASteadyStateModule(app));
  // Load RE module
  app->addModule(new REModule(app));
  // Load LNA module
  app->addModule(new LNAModule(app));
  // Load IOS module
  app->addModule(new IOSModule(app));
  // Load SSA module:
  app->addModule(new SSAModule(app));

  window->show();

  // If there is a SBML file passed as CMD line argument:
  if (2 <= argc)
  {
    app->docTree()->addDocument(new DocumentItem(argv[1]));
  }

  // run.
  qapp.exec();

  // On exit -> destroy application model (frees all data):
  Application::shutdown();

  // Deactivate logger.
  Fluc::Utils::Logger::shutdown();

  return 0;
}
