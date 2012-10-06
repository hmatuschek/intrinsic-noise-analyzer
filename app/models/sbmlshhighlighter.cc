#include "sbmlshhighlighter.hh"

SbmlshHighlighter::SbmlshHighlighter(QTextDocument *doc) :
  QSyntaxHighlighter(doc), _default_format(), _rules()
{
  QTextCharFormat kw_format;
  kw_format.setFontFamily("Courier");
  kw_format.setFontWeight(QFont::Bold);
  kw_format.setForeground(Qt::darkMagenta);
  QRegExp kw_expression("\\x0040\\b[A-Za-z]+\\b");

  QTextCharFormat qs_format;
  qs_format.setFontFamily("Courier");
  qs_format.setFontWeight(QFont::Normal);
  qs_format.setFontItalic(true);
  qs_format.setForeground(Qt::darkGray);
  QRegExp qs_expression("\"[^\"]*\"");

  QTextCharFormat com_format;
  com_format.setFontFamily("Courier");
  com_format.setFontWeight(QFont::Normal);
  com_format.setFontItalic(true);
  com_format.setForeground(Qt::darkGray);
  QRegExp com_expression("#.*$");

  _rules.append(QPair<QRegExp,QTextCharFormat>(kw_expression, kw_format));
  _rules.append(QPair<QRegExp,QTextCharFormat>(qs_expression, qs_format));
  _rules.append(QPair<QRegExp,QTextCharFormat>(com_expression, com_format));

  _default_format.setFontFamily("Courier");
  _default_format.setFontWeight(QFont::Normal);
  _default_format.setForeground(Qt::black);
}


void
SbmlshHighlighter::highlightBlock(const QString &text)
{
  int curr_index=0, next_index, length; QTextCharFormat format;

  findNext(text, curr_index, next_index, length, format);
  if (0 > next_index) { setFormat(curr_index, text.size()-curr_index, _default_format); }
  else { setFormat(curr_index, next_index-curr_index, _default_format); }
  while (next_index >= 0) {
    setFormat(next_index, length, format); curr_index = next_index+length;
    findNext(text, curr_index, next_index, length, format);
    if (0 > next_index) { setFormat(curr_index, text.size()-curr_index, _default_format); }
    else { setFormat(curr_index, next_index-curr_index, _default_format); }
  }
}


void
SbmlshHighlighter::findNext(const QString &text, int index, int &offset, int &length, QTextCharFormat &format) {
  offset = -1; length = -1;
  int rule_index = -1, rule_length = -1;

  for (QList< QPair<QRegExp, QTextCharFormat> >::iterator rule = _rules.begin();
       rule!=_rules.end(); rule++)
  {
    rule_index = text.indexOf(rule->first, index);
    if (0 > rule_index) continue;
    rule_length = rule->first.matchedLength();

    if (0 > offset) {
      offset = rule_index; length = rule_length; format = rule->second; continue;
    }

    if (offset > rule_index || ((offset == rule_index) && (length < rule_length))) {
      offset = rule_index; length = rule_length; format = rule->second;
    }
  }
}
