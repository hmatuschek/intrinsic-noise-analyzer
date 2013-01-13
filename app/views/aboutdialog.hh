#ifndef __INA_VIEWS_ABOUTDIALOG_HH__
#define __INA_VIEWS_ABOUTDIALOG_HH__

#include <QDialog>
#include <QTextEdit>


/** The about dialog.
 * @ingroup gui */
class AboutDialog : public QDialog
{
  Q_OBJECT

public:
  /** Constructor. */
  explicit AboutDialog(QWidget *parent = 0);
};


/** First page.
 * @ingroup gui */
class AboutPage : public QTextEdit
{
  Q_OBJECT

public:
  /** Constructor. */
  explicit AboutPage(QWidget *parent=0);

private:
  /** The content. */
  static QString content;
};


/** List of authors.
 * @ingroup gui */
class AuthorsPage : public QTextEdit
{
  Q_OBJECT

public:
  /** Constructor. */
  explicit AuthorsPage(QWidget *parent=0);

private:
  /** The content. */
  static QString content;
};


/** The license page.
 * @ingroup gui */
class LicensePage : public QTextEdit
{
  Q_OBJECT

public:
  /** Constructor. */
  explicit LicensePage(QWidget *parent=0);

private:
  /** The content. */
  static QString content;
};


#endif // __INA_VIEWS_ABOUTDIALOG_HH__
