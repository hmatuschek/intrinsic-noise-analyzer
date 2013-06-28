#ifndef __INA_APP_SSE_SSETASKCONFIG_HH__
#define __INA_APP_SSE_SSETASKCONFIG_HH__


#include "../models/generaltaskconfig.hh"
#include "models/REmodel.hh"
#include "models/LNAmodel.hh"
#include "models/IOSmodel.hh"


/**
 * Holds the complete configuration for the SSE task.
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


public:
  /**
   * Default constructor.
   */
  SSETaskConfig();

  /** Copy constructor. */
  SSETaskConfig(const SSETaskConfig &other);

  /** Overrides the default implenentation of @c ModelSelectionTaskConfig and checks if
   * the selected model can be used to instantiate the specified analysis. */
  virtual void setModelDocument(DocumentItem *document);

  /** Allows to reset the model. */
  void setModel(iNA::Ast::Model *m);

  /** Implements the @c SpeciesSelectionTaskConfig interface, and returns the model instance. */
  virtual iNA::Ast::Model *getModel() const;

  /** Get model as specific type.
  * @note Now absolete!
  */
  template <class T> T* getModelAs() const {
    return dynamic_cast<T *>(getModel());
  }

  /** Sets the selected analysis method. This determines, which kind of analysis will be
   * instantiated when @c setModelDocument is called. */
  void setMethod(SSEMethod method);
  /** Returns the selected method. */
  SSEMethod method() const;

protected:
  /** Specifies the selected SSE analysis method. */
  SSEMethod _selected_method;
  /** The model to be analyzed. */
  iNA::Ast::Model *_re_model;

};


#endif
