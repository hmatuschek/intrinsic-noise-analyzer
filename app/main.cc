#include "main.hh"

#include "doctree/documentitem.hh"
#include "utils/logger.hh"


using namespace std;


class ExceptionApplication : public QApplication
{
public:
  ExceptionApplication(int argc, char *argv[]) : QApplication(argc, argv) { }

  bool notify(QObject *obj, QEvent *event)
  {
    try {
      return QApplication::notify(obj, event);
    } catch (std::exception &err) {
      std::cerr << __FILE__ << " @" << __LINE__
                << ": Oops, there was an error: " << err.what() << std::endl;
      return false;
    }
  }
};

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
  if (2 <= argc)
  {
    app->docTree()->addDocument(new DocumentItem(argv[1]));
  }

  // run.
  qapp.exec();

  // On exit -> destroy application model (frees all data):
  Application::shutdown();

  // Deactivate logger.
  iNA::Utils::Logger::shutdown();

  return 0;
}
