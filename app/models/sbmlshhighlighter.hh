#ifndef __INA_APP_SBMLSHHIGHLIGHTER_HH__
#define __INA_APP_SBMLSHHIGHLIGHTER_HH__

#include <QSyntaxHighlighter>


/** Simple class to implement a syntax highlighting for SBML-SH. */
class SbmlshHighlighter : public QSyntaxHighlighter
{
  Q_OBJECT

public:
  /** Constructor. */
  explicit SbmlshHighlighter(QTextDocument *doc);

  /** Implements the actual syntax highlighting. */
  virtual void highlightBlock(const QString &text);

private:
  QTextCharFormat _default_format;
  QList< QPair<QRegExp, QTextCharFormat> > _rules;
  void findNext(const QString &text, int index, int &offset, int &length, QTextCharFormat &format);
};

#endif // __INA_APP_SBMLSHHIGHLIGHTER_HH__
