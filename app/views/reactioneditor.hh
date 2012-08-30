#ifndef __INA_APP_VIEWS_REACTIONEDITOR_HH__
#define __INA_APP_VIEWS_REACTIONEDITOR_HH__

#include <QDialog>
#include <QLineEdit>
#include <QCheckBox>
#include <ast/reaction.hh>
#include <QList>
#include <QPair>
#include <QStackedWidget>
#include <QLabel>
#include <QTimer>


/** Simple editor dialog to create/modify a reaction. */
class ReactionCreator : public QDialog
{
  Q_OBJECT

public:
  /** Constructor. */
  ReactionCreator(QWidget *parent = 0);

private slots:
  void onMassActionToggled(bool state);
  void updateKineticLaw();

private:
  bool parseEquation(const QString &text, QList< QPair<int, QString> > &reactants,
                     QList< QPair<int, QString> > &product, bool &reversible);

  bool parseStoichiometry(const QString &text, QList< QPair<int, QString> > &stoichiometry);

private:
  Fluc::Ast::Reaction *_reaction;
  QLineEdit *_name;
  QLineEdit *_equation;
  QStackedWidget *_kineticLaw;
  QLineEdit *_kineticLawEditor;
  QLabel    *_kineticLawFormula;
  QCheckBox *_autoupdate;
  QTimer    *_delayTimer;
};

#endif // REACTIONEDITOR_HH
