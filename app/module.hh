#ifndef MODULE_HH
#define MODULE_HH

#include <QObject>

/**
 * Trivial base class for all modules.
 */
class Module : public QObject
{
  Q_OBJECT

public:
  /**
   * Constructor.
   */
  explicit Module(QObject *parent = 0);
};

#endif // MODULE_HH
