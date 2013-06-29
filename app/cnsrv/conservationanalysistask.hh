#ifndef __INA_APP_CONSERVATION_ANALYSIS_TAKS_HH__
#define __INA_APP_CONSERVATION_ANALYSIS_TAKS_HH__

#include <QWidget>

#include "../models/task.hh"
#include "../models/generaltaskconfig.hh"
#include "../views/taskview.hh"
#include "../views/generaltaskwizard.hh"

#include "conservedquantities.hh"


/** Implements the trivial config of the conservation analysis (just a selected model).
 * However, to maintain consistency with the other tasks, I implement the full scheme. */
class ConservationAnalysisConfig
    : public GeneralTaskConfig, public ModelSelectionTaskConfig
{
  Q_OBJECT

public:
  /** Default constructor. */
  ConservationAnalysisConfig();
  /** Copy constructor. */
  ConservationAnalysisConfig(const ConservationAnalysisConfig &other);
};

/** Implements the actual conservation analysis as a @c Task. */
class ConservationAnalysisTask : public Task
{
  Q_OBJECT

public:
  /** Constructs the task from the given config. */
  explicit ConservationAnalysisTask(const ConservationAnalysisConfig &config, QObject *parent=0);
  virtual ~ConservationAnalysisTask();

  /** Performs the task. */
  virtual void process();

  virtual QString getLabel();

  /** Returns the number of conservation cycles. */
  size_t numCycles() const;

  /** Retuns the specified cycle. */
  GiNaC::ex cycle(size_t i) const;
  /** Retuns the specified cycle (constant). */
  GiNaC::ex constant(size_t i) const;

  /** Returns the analysis model, used to pretty print the conservation cycle expressions. */
  iNA::Ast::Model &model();

private:
  ConservationAnalysisConfig _config;
  iNA::Models::ConservedQuantities *_analysis;
  Eigen::VectorXex _cons_expr;
  Eigen::VectorXex _cons_const;
};


/** A table model holding the conservation cycles of the analysis. */
class ConservationAnalysisTable: public QAbstractTableModel
{
  Q_OBJECT

public:
  ConservationAnalysisTable(ConservationAnalysisTask *task);

  int rowCount(const QModelIndex &parent) const;
  int columnCount(const QModelIndex &parent) const;
  QVariant data(const QModelIndex &index, int role) const;
  QVariant headerData(int section, Qt::Orientation orientation, int role) const;

private:
  ConservationAnalysisTask *_task;
};


/** The result widget, presenting the conservation analysis results as a table. */
class ConservationAnalysisWidget: public QWidget
{
  Q_OBJECT

public:
  ConservationAnalysisWidget(ConservationAnalysisTask *task, QWidget *parent=0);

private slots:
  void onUpdateAnalysis();

protected:
  /** A weak reference to the analsis task. */
  ConservationAnalysisTask *_task;
};


/** The result widget, presenting the conservation analysis results as a equation. */
class ConservationAnalysisEqWidget: public QWidget
{
  Q_OBJECT

public:
  ConservationAnalysisEqWidget(ConservationAnalysisTask *task, QWidget *parent=0);

private slots:
  void onUpdateAnalysis();

protected:
  /** A weak reference to the analsis task. */
  ConservationAnalysisTask *_task;
};


/** The analysis result view. */
class ConservationAnalysisView: public TaskView
{
  Q_OBJECT

public:
  ConservationAnalysisView(TaskItem *item, QWidget *parent=0);

  virtual QWidget *createResultWidget(TaskItem *task_item);
};


/** Configuration wizard for the analysis. */
class ConservationAnalysisWizard: public GeneralTaskWizard
{
  Q_OBJECT

public:
  ConservationAnalysisWizard(ConservationAnalysisConfig &config);
  virtual GeneralTaskConfig &getConfig();

protected:
  ConservationAnalysisConfig &_config;
};


/** This class represents the analysis or its results within the document tree, it creates
 * the view for the task. */
class ConservationAnalysisItem: public TaskItem {
  Q_OBJECT

public:
  /** Constructs the analysis item given holding the given task. */
  explicit ConservationAnalysisItem(ConservationAnalysisTask *task);
  /** Returns true, as there is a view for the item. */
  virtual bool providesView() const;
  /** Creates a view for the task. */
  virtual QWidget *createView();
};

#endif
