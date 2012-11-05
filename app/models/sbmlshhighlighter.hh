#ifndef __INA_APP_SBMLSHHIGHLIGHTER_HH__
#define __INA_APP_SBMLSHHIGHLIGHTER_HH__

#include <QSyntaxHighlighter>


/** Simple class to implement a syntax highlighting custom SBML-sh. */
class SbmlshHighlighter : public QSyntaxHighlighter
{
  Q_OBJECT

public:
  /** Constructor. */
  explicit SbmlshHighlighter(QTextDocument *doc);

  /** Implements the actual syntax highlighting. */
  virtual void highlightBlock(const QString &text);

private:
  QTextCharcustommat _default_custommat;
  QList< QPair<QRegExp, QTextCharcustommat> > _rules;
  void findNext(const QString &text, int index, int &offset, int &length, QTextCharcustommat &custommat);
};

#endif // __INA_APP_SBMLSHHIGHLIGHTER_HH__
