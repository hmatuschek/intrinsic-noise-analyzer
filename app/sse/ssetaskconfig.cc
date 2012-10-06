#include "ssetaskconfig.hh"

SSETaskConfig::SSETaskConfig()
  : GeneralTaskConfig(), ModelSelectionTaskConfig(), EngineTaskConfig(), ODEIntTaskConfig(),
    selected_method(UNDEFINED_ANALYSIS), re_model(0), lna_model(0), ios_model(0)
{
  // pass...
}

SSETaskConfig::SSETaskConfig(const SSETaskConfig &other)
  : GeneralTaskConfig(), ModelSelectionTaskConfig(other), EngineTaskConfig(other),
    ODEIntTaskConfig(other), selected_method(other.selected_method), re_model(other.re_model),
    lna_model(other.lna_model), ios_model(other.ios_model)
{
  // Pass...
}


void
SSETaskConfig::setModelDocument(DocumentItem *document)
{
  ModelSelectionTaskConfig::setModelDocument(document);

  // Construct analysis model depending on the selected method:
  switch (selected_method) {
  case RE_ANALYSIS:
    this->re_model = new iNA::Models::REmodel(document->getModel());
    this->lna_model = 0;
    this->ios_model = 0;
    break;

  case LNA_ANALYSIS:
    this->re_model = 0;
    this->lna_model = new iNA::Models::LNAmodel(document->getModel());
    this->ios_model = 0;
    break;

  case IOS_ANALYSIS:
    this->re_model = 0;
    this->lna_model = 0;
    this->ios_model = new iNA::Models::IOSmodel(document->getModel());
    break;

  case UNDEFINED_ANALYSIS:
    this->re_model = 0;
    this->lna_model = 0;
    this->ios_model = 0;
    break;
  }
}


iNA::Ast::Model *
SSETaskConfig::getModel() const
{
  switch (selected_method) {
  case RE_ANALYSIS:
    return this->re_model;

  case LNA_ANALYSIS:
    return this->lna_model;

  case IOS_ANALYSIS:
    return this->ios_model;

  default:
    break;
  }

  return 0;
}


void
SSETaskConfig::setMethod(SSEMethod method)
{
  selected_method = method;
}


SSETaskConfig::SSEMethod
SSETaskConfig::getMethod() const
{
  return selected_method;
}
