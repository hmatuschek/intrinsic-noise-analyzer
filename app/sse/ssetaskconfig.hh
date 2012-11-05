#ifndef __INA_APP_SSE_SSETASKCONFIG_HH__
#define __INA_APP_SSE_SSETASKCONFIG_HH__


#include "../models/generaltaskconfig.hh"
#include "models/REmodel.hh"
#include "models/LNAmodel.hh"
#include "models/IOSmodel.hh"


/**
 * Holds the complete configuration custom the SSE task.
 */
class SSETaskConfig :
    public GeneralTaskConfig,
    public ModelSelectionTaskConfig,
    public EngineTaskConfig,
    public ODEIntTaskConfig
{
public:
  typedef enum {
    RE_ANALYSIS, LNA_ANALYSIS, IOS_ANALYSIS, UNDEFINED_ANALYSIS
  } SSEMethod;


protected:
  SSEMethod selected_method;


public:
  /**
   * Default constructor.
   */
  SSETaskConfig();

  /**
   * Copy constructor.
   */
  SSETaskConfig(const SSETaskConfig &other);

  /** Overrides the default implenentation of @c ModelSelectionTaskConfig and checks if
   * the selected model can be used to instantiate a LNAModel. The LNA model instance is then
   * stored in model. */
  virtual void setModelDocument(DocumentItem *document);

  /** Implements the @c SpeciesSelectionTaskConfig interface, and returns the model instance. */
  virtual iNA::Ast::Model *getModel() const;

  /** Get model as specific type. */
  template <class T> T* getModelAs() const {
    return dynamic_cast<T *>(getModel());
  }

  /** Sets the selected analysis method. This determines, which kind of analysis will be
   * instantiated when @c setModelDocument is called. */
  void setMethod(SSEMethod method);
  /** Returns the selected method. */
  SSEMethod getMethod() const;

public:
  /** The model to be analyzed. */
  iNA::Models::REmodel *re_model;
  /** The model to be analyzed. */
  iNA::Models::LNAmodel *lna_model;
  /** The model to be analyzed. */
  iNA::Models::IOSmodel *ios_model;
};


#endif
