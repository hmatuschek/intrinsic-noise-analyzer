#ifndef __INA_APP_SSSCAN_PLOT_HH__
#define __INA_APP_SSSCAN_PLOT_HH__

#include "../plot/plot.hh"
#include "../plot/graph.hh"
#include "../plot/variancelinegraph.hh"
#include "../timeseries.hh"
#include "ssscantask.hh"


/**
 * Collects data from a @c Table and updates the
 * @c VarianceLinesGraph instance.
 */
class ParameterScanPlot : public Plot::Figure
{
  Q_OBJECT

public:
  explicit ParameterScanPlot(size_t num_species, Table *data,
                             const QString &species_unit, const QString &time_unit,
                             QObject *parent=0);
};

/**
 * Collects data from a @c Table and updates the
 * @c VarianceLinesGraph instance.
 */
class ParameterScanIOSPlot : public Plot::Figure
{
  Q_OBJECT

public:
  explicit ParameterScanIOSPlot(size_t num_species, Table *data,
                             const QString &species_unit, const QString &time_unit,
                             QObject *parent=0);
};

/**
 * Collects data from a @c Table and updates the
 * @c VarianceLinesGraph instance.
 */
class ParameterScanCovPlot : public Plot::Figure
{
  Q_OBJECT

public:
  explicit ParameterScanCovPlot(size_t num_species, Table *data,
                             const QString &species_unit, const QString &time_unit,
                             QObject *parent=0);
};

/**
 * Collects data from a @c Table and updates the
 * @c VarianceLinesGraph instance.
 */
class ParameterScanCovIOSPlot : public Plot::Figure
{
  Q_OBJECT

public:
  explicit ParameterScanCovIOSPlot(size_t num_species, Table *data,
                             const QString &species_unit, const QString &time_unit,
                             QObject *parent=0);
};


#endif
