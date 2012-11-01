#ifndef __INA_APP_PARAMSCAN_PLOT_HH__
#define __INA_APP_PARAMSCAN_PLOT_HH__

#include "../plot/plot.hh"
#include "../plot/graph.hh"
#include "../plot/variancelinegraph.hh"
#include "../models/timeseries.hh"
#include "paramscantask.hh"
#include "../views/varianceplot.hh"

/**
 * Collects data from a @c Table and updates the
 * @c VarianceLinesGraph instance.
 */
class SimpleParameterScanPlot : public LinePlot
{
  Q_OBJECT

public:
  explicit SimpleParameterScanPlot(const QStringList &selected_species, ParamScanTask *task, QObject *parent=0);
};


/**
 * Collects data from a @c Table and updates the
 * @c VarianceLinesGraph instance.
 */
class ParameterScanPlot : public VariancePlot
{
  Q_OBJECT

public:
  explicit ParameterScanPlot(const QStringList &selected_species, ParamScanTask *task, QObject *parent=0);
};

/**
 * Collects data from a @c Table and updates the
 * @c VarianceLinesGraph instance.
 */
class ParameterScanIOSPlot : public VariancePlot
{
  Q_OBJECT

public:
  explicit ParameterScanIOSPlot(const QStringList &selected_species, ParamScanTask *task, QObject *parent=0);
};

/**
 * Collects data from a @c Table and updates the
 * @c VarianceLinesGraph instance.
 */
class ParameterScanCovPlot : public LinePlot
{
  Q_OBJECT

public:
  explicit ParameterScanCovPlot(const QStringList &selected_species, ParamScanTask *task, QObject *parent=0);
};

/**
 * Collects data from a @c Table and updates the
 * @c VarianceLinesGraph instance.
 */
class ParameterScanCovIOSPlot : public LinePlot
{
  Q_OBJECT

public:
  explicit ParameterScanCovIOSPlot(const QStringList &selected_species, ParamScanTask *task, QObject *parent=0);
};


#endif
