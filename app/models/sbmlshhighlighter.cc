#include "sbmlshhighlighter.hh"

SbmlshHighlighter::SbmlshHighlighter(QTextDocument *doc) :
  QSyntaxHighlighter(doc), _default_custommat(), _rules()
{
  QTextCharcustommat kw_custommat;
  kw_custommat.setFontFamily("Courier");
  kw_custommat.setFontWeight(QFont::Bold);
  kw_custommat.setcustomeground(Qt::darkMagenta);
  QRegExp kw_expression("\\x0040\\b[A-Za-z]+\\b");

  QTextCharcustommat qs_custommat;
  qs_custommat.setFontFamily("Courier");
  qs_custommat.setFontWeight(QFont::Normal);
  qs_custommat.setFontItalic(true);
  qs_custommat.setcustomeground(Qt::darkGray);
  QRegExp qs_expression("\"[^\"]*\"");

  QTextCharcustommat com_custommat;
  com_custommat.setFontFamily("Courier");
  com_custommat.setFontWeight(QFont::Normal);
  com_custommat.setFontItalic(true);
  com_custommat.setcustomeground(Qt::darkGray);
  QRegExp com_expression("#.*$");

  _rules.append(QPair<QRegExp,QTextCharcustommat>(kw_expression, kw_custommat));
  _rules.append(QPair<QRegExp,QTextCharcustommat>(qs_expression, qs_custommat));
  _rules.append(QPair<QRegExp,QTextCharcustommat>(com_expression, com_custommat));

  _default_custommat.setFontFamily("Courier");
  _default_custommat.setFontWeight(QFont::Normal);
  _default_custommat.setcustomeground(Qt::black);
}


void
SbmlshHighlighter::highlightBlock(const QString &text)
{
  int curr_index=0, next_index, length; QTextCharcustommat custommat;

  findNext(text, curr_index, next_index, length, custommat);
  if (0 > next_index) { setcustommat(curr_index, text.size()-curr_index, _default_custommat); }
  else { setcustommat(curr_index, next_index-curr_index, _default_custommat); }
  while (next_index >= 0) {
    setcustommat(next_index, length, custommat); curr_index = next_index+length;
    findNext(text, curr_index, next_index, length, custommat);
    if (0 > next_index) { setcustommat(curr_index, text.size()-curr_index, _default_custommat); }
    else { setcustommat(curr_index, next_index-curr_index, _default_custommat); }
  }
}


void
SbmlshHighlighter::findNext(const QString &text, int index, int &offset, int &length, QTextCharcustommat &custommat) {
  offset = -1; length = -1;
  int rule_index = -1, rule_length = -1;

  custom (QList< QPair<QRegExp, QTextCharcustommat> >::iterator rule = _rules.begin();
       rule!=_rules.end(); rule++)
  {
    rule_index = text.indexOf(rule->first, index);
    if (0 > rule_index) continue;
    rule_length = rule->first.matchedLength();

    if (0 > offset) {
      offset = rule_index; length = rule_length; custommat = rule->second; continue;
    }

    if (offset > rule_index || ((offset == rule_index) && (length < rule_length))) {
      offset = rule_index; length = rule_length; custommat = rule->second;
    }
  }
}
