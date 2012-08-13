#include "reactionview.hh"
#include "../application.hh"
#include <QLabel>
#include <QVBoxLayout>

#include <QGraphicsView>
#include <QGraphicsItem>
#include <QTableView>

#include "reactionequationrenderer.hh"


ReactionView::ReactionView(ReactionItem *reaction, QWidget *parent) :
  QWidget(parent)
{
  this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);
  this->setBackgroundRole(QPalette::Window);

  QLabel *label = new QLabel(tr("Reaction") + " " + reaction->getDisplayName());
  label->setFont(Application::getApp()->getH1Font());
  label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

  QVBoxLayout *layout = new QVBoxLayout();
  layout->addWidget(label, 0, Qt::AlignRight);

  QGraphicsView *equation_view = new QGraphicsView();
  ReactionEquationRenderer *renderer = new ReactionEquationRenderer(reaction->getReaction());
  equation_view->setScene(renderer);
  layout->addWidget(equation_view);

  QTableView *table_view = new QTableView();
  table_view->setModel(reaction->localParameters());
  layout->addWidget(table_view);

  this->setLayout(layout);

  // Connect signals:
  QObject::connect(reaction, SIGNAL(destroyed()), this, SLOT(reactionDestroyed()));
}


void
ReactionView::reactionDestroyed()
{
  //std::cerr << "ReactionItem destroyed -> close ReactionView. " << std::endl;
  this->deleteLater();
}
