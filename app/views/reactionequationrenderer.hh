#ifndef REACTIONEQUATIONRENDERER_HH
#define REACTIONEQUATIONRENDERER_HH

#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QGraphicsLineItem>


/**
 * Implements a simple reaction-equation renderer.
 *
 * @todo Make it more beautiful.
 *
 * @ingroup gui
 */
class ReactionEquationRenderer : public QGraphicsScene
{
  Q_OBJECT


protected:
  /**
   * Defines the arrow length.
   */
  double arrow_length;

  /**
   * Defines the arrow-head width (length).
   */
  double arrow_head_width;

  /**
   * Defines the arrow-head height.
   */
  double arrow_head_height;

  /**
   * Defines the margin between reaction and rate-rule.
   */
  double reaction_margin;

  /**
   * List of rendered reactants with stoichiometry.
   */
  QList<QGraphicsItem *> reactants;

  /**
   * List of rendered products with stoichiometry.
   */
  QList<QGraphicsItem *> products;

  /**
   * Rendered rate-rule.
   */
  QGraphicsItem *rate;

  /**
   * Rendered arrow.
   */
  QGraphicsItemGroup *arrow;

  /**
   * The colon, between reaction and rate-rule.
   */
  QGraphicsTextItem *colon;


public:
  /**
   * Constructs an empty rendered reaction.
   */
  explicit ReactionEquationRenderer(QObject *parent = 0);

  /**
   * Adds and renders a reactant.
   */
  void addReactant(const std::string &stoi, const std::string &spec);

  /**
   * Adds and renders a product.
   */
  void addProduct(const std::string &stoi, const std::string &spec);

  /**
   * Sets and renders the rate-rule.
   */
  void setRate(const std::string &rate);

  /**
   * Rearanges the reaction.
   */
  void updateLayout();
};

#endif // REACTIONEQUATIONRENDERER_HH
