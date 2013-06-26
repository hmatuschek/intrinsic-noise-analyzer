#include "ssetaskconfig.hh"

SSETaskConfig::SSETaskConfig()
  : GeneralTaskConfig(), ModelSelectionTaskConfig(), EngineTaskConfig(), ODEIntTaskConfig(),
    _selected_method(UNDEFINED_ANALYSIS), _re_model(0), _lna_model(0), _ios_model(0)
{
  // pass...
}

SSETaskConfig::SSETaskConfig(const SSETaskConfig &other)
  : GeneralTaskConfig(), ModelSelectionTaskConfig(other), EngineTaskConfig(other),
    ODEIntTaskConfig(other), _selected_method(other._selected_method), _re_model(other._re_model),
    _lna_model(other._lna_model), _ios_model(other._ios_model)
{
  // Pass...
}


void
SSETaskConfig::setModelDocument(DocumentItem *document)
{
  ModelSelectionTaskConfig::setModelDocument(document);

  // Construct analysis model depending on the selected method:
  switch (_selected_method) {
  case RE_ANALYSIS:
    this->_re_model = new iNA::Models::REmodel(document->getModel());
    this->_lna_model = 0;
    this->_ios_model = 0;
    break;

  case LNA_ANALYSIS:
    this->_re_model = 0;
    this->_lna_model = new iNA::Models::LNAmodel(document->getModel());
    this->_ios_model = 0;
    break;

  case IOS_ANALYSIS:
    this->_re_model = 0;
    this->_lna_model = 0;
    this->_ios_model = new iNA::Models::IOSmodel(document->getModel());
    break;

  case UNDEFINED_ANALYSIS:
    this->_re_model = 0;
    this->_lna_model = 0;
    this->_ios_model = 0;
    break;
  }
}


iNA::Ast::Model *
SSETaskConfig::getModel() const
{
  switch (_selected_method) {
  case RE_ANALYSIS:
    return this->_re_model;

  case LNA_ANALYSIS:
    return this->_lna_model;

  case IOS_ANALYSIS:
    return this->_ios_model;

  case UNDEFINED_ANALYSIS:
    return 0;
  }

  return 0;
}


void
SSETaskConfig::setMethod(SSEMethod method)
{
  _selected_method = method;
}


SSETaskConfig::SSEMethod
SSETaskConfig::method() const
{
  return _selected_method;
}
