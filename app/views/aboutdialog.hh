#ifndef ABOUTDIALOG_HH
#define ABOUTDIALOG_HH

#include <QDialog>
#include <QTextEdit>


/**
 * The about dialog.
 *
 * @ingroup gui
 */
class AboutDialog : public QDialog
{
  Q_OBJECT

public:
  /**
   * Constructor.
   */
  explicit AboutDialog(QWidget *parent = 0);
};


/**
 * First page.
 *
 * @ingroup gui
 */
class AboutPage : public QTextEdit
{
  Q_OBJECT

private:
  /**
   * The content.
   */
  static QString content;

public:
  /**
   * Constructor.
   */
  explicit AboutPage(QWidget *parent=0);
};


/**
 * List of authors.
 *
 * @ingroup gui
 */
class AuthorsPage : public QTextEdit
{
  Q_OBJECT

private:
  /**
   * The content.
   */
  static QString content;

public:
  /**
   * Constructor.
   */
  explicit AuthorsPage(QWidget *parent=0);
};


/**
 * The license page.
 *
 * @ingroup gui
 */
class LicensePage : public QTextEdit
{
  Q_OBJECT

private:
  /**
   * The content.
   */
  static QString content;

public:
  /**
   * Constructor.
   */
  explicit LicensePage(QWidget *parent=0);
};


#endif // ABOUTDIALOG_HH
