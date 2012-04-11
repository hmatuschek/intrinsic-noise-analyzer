#ifndef __INA_APP_DOCTREE_SBMLMODELREACTIONS_HH__
#define __INA_APP_DOCTREE_SBMLMODELREACTIONS_HH__

#include <QObject>
#include "ast/model.hh"
#include "reactionitem.hh"
#include "documenttreeitem.hh"


class ReactionsItem : public QObject, public DocumentTreeItem
{
  Q_OBJECT

public:
  explicit ReactionsItem(Fluc::Ast::Model *model, QObject *parent = 0);

  virtual const QString &getLabel() const;

private:
  QString itemLabel;
};


#endif // __INA_APP_DOCTREE_SBMLMODELREACTIONS_HH__
