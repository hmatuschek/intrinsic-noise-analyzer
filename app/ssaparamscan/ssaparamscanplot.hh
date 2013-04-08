#ifndef __INA_APP_SSAPARAMSCAN_PLOT_HH__
#define __INA_APP_SSAPARAMSCAN_PLOT_HH__

#include "../plot/plot.hh"
#include "../plot/graph.hh"
#include "../plot/variancelinegraph.hh"
#include "../models/timeseries.hh"
#include "ssaparamscantask.hh"
#include "../views/varianceplot.hh"

/**
 * Collects data from a @c Table and updates the
 * @c VarianceLinesGraph instance.
 */
class SSASimpleParameterScanPlot : public LinePlot
{
  Q_OBJECT

public:
  explicit SSASimpleParameterScanPlot(const QStringList &selected_species, SSAParamScanTask *task, QObject *parent=0);
};


/**
 * Collects data from a @c Table and updates the
 * @c VarianceLinesGraph instance.
 */
class SSAParameterScanPlot : public LinePlot
{
  Q_OBJECT

public:
  explicit SSAParameterScanPlot(const QStringList &selected_species, SSAParamScanTask *task, QObject *parent=0);
};


/**
 * Collects data from a @c Table and updates the
 * @c VarianceLinesGraph instance.
 */
class SSAParameterScanCVPlot : public LinePlot
{
  Q_OBJECT

public:
  explicit SSAParameterScanCVPlot(const QStringList &selected_species, SSAParamScanTask *task, QObject *parent=0);
};

/**
 * Collects data from a @c Table and updates the
 * @c VarianceLinesGraph instance.
 */
class SSAParameterScanFanoPlot : public LinePlot
{
  Q_OBJECT

public:
  explicit SSAParameterScanFanoPlot(const QStringList &selected_species, SSAParamScanTask *task, QObject *parent=0);
};

#endif
