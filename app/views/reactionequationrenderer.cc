#include "reactionequationrenderer.hh"
#include <QGraphicsItem>
#include <QGraphicsTextItem>



ReactionEquationRenderer::ReactionEquationRenderer(QObject *parent) :
  QGraphicsScene(parent), rate(0)
{
  this->arrow_length = 20.0;
  this->arrow_head_width = 6.;
  this->arrow_head_height = 6.;
  this->reaction_margin = 20.;

  // Construct arrow
  QList<QGraphicsItem *> arrow_elm;
  arrow_elm.append(this->addLine(0, this->arrow_head_height/2,
                                 this->arrow_length, this->arrow_head_height/2));
  arrow_elm.append(this->addLine(this->arrow_length-this->arrow_head_width, 0.,
                                 this->arrow_length, this->arrow_head_height/2));
  arrow_elm.append(this->addLine(this->arrow_length-this->arrow_head_width, this->arrow_head_height,
                                 this->arrow_length, this->arrow_head_height/2));
  this->arrow = this->createItemGroup(arrow_elm);

  this->colon = this->addText(":");

  this->updateLayout();
}


void
ReactionEquationRenderer::addReactant(const std::string &stoi, const std::string &spec)
{
  if (0 != this->reactants.size())
  {
    this->reactants.append(this->addText("+"));
  }

  this->reactants.append(this->addText((stoi+" "+spec).c_str()));

  this->updateLayout();
}


void
ReactionEquationRenderer::addProduct(const std::string &stoi, const std::string &spec)
{
  if (0 != this->products.size())
  {
    this->products.append(this->addText("+"));
  }

  this->products.append(this->addText((stoi+" "+spec).c_str()));

  this->updateLayout();
}


void
ReactionEquationRenderer::setRate(const std::string &rate)
{
  if (0 != this->rate)
  {
    this->removeItem(this->rate);
    delete this->rate;
  }

  this->rate = this->addText(rate.c_str());

  this->updateLayout();
}


void
ReactionEquationRenderer::updateLayout()
{
  double overall_width = 0.0;
  double overall_height = 0.0;


  for (QList<QGraphicsItem *>::iterator iter = this->reactants.begin();
       iter != this->reactants.end(); iter++)
  {
    overall_width += (*iter)->boundingRect().width();
    overall_height = std::max(overall_height, (*iter)->boundingRect().height());
  }

  overall_width += this->arrow->boundingRect().width();

  for (QList<QGraphicsItem *>::iterator iter = this->products.begin();
       iter != this->products.end(); iter++)
  {
    overall_width += (*iter)->boundingRect().width();
    overall_height = std::max(overall_height, (*iter)->boundingRect().height());
  }

  overall_width += this->reaction_margin;
  overall_width += this->colon->boundingRect().width();
  overall_width += this->reaction_margin;

  if (0 != this->rate)
  {
    overall_width += this->rate->boundingRect().width();
    overall_height = std::max(overall_height, overall_height/2+this->rate->boundingRect().height());
  }

  /*
   * Update positions of elements.
   */
  double x = 0.0;
  for (QList<QGraphicsItem *>::iterator iter = this->reactants.begin();
       iter != this->reactants.end(); iter++)
  {
    double h = (*iter)->boundingRect().height();
    (*iter)->setPos(x, overall_height/2 - h/2);
    x += (*iter)->boundingRect().width();
  }

  // Set start-position of arrow
  this->arrow->setPos(x, overall_height/2 - this->arrow->boundingRect().height()/2);
  x += this->arrow->boundingRect().width();

  for (QList<QGraphicsItem *>::iterator iter = this->products.begin();
       iter != this->products.end(); iter++)
  {
    double h = (*iter)->boundingRect().height();
    (*iter)->setPos(x, overall_height/2 - h/2);
    x += (*iter)->boundingRect().width();
  }

  x += this->reaction_margin;
  this->colon->setPos(x, overall_height/2-this->colon->boundingRect().height()/2);
  x += this->colon->boundingRect().width();

  x += this->reaction_margin;
  if (0 != this->rate)
  {
    this->rate->setPos(x, overall_height/2-this->rate->boundingRect().height()/2);
    x += this->rate->boundingRect().width();
  }
}
