#include "ssetaskconfig.hh"

SSETaskConfig::SSETaskConfig()
  : GeneralTaskConfig(), ModelSelectionTaskConfig(), EngineTaskConfig(), ODEIntTaskConfig(),
    _selected_method(UNDEFINED_ANALYSIS), _re_model(0)
{
  // pass...
}

SSETaskConfig::SSETaskConfig(const SSETaskConfig &other)
  : GeneralTaskConfig(), ModelSelectionTaskConfig(other), EngineTaskConfig(other),
    ODEIntTaskConfig(other), _selected_method(other._selected_method), _re_model(other._re_model)
{
  // Pass...
}


void
SSETaskConfig::setModelDocument(DocumentItem *document)
{
  ModelSelectionTaskConfig::setModelDocument(document);

  this->_re_model = new iNA::Models::ConservationAnalysis(document->getModel());

//  // Construct analysis model depending on the selected method:
//  switch (_selected_method) {
//  case RE_ANALYSIS:
//    this->_re_model = new iNA::Models::REmodel(document->getModel());
//    this->_lna_model = 0;
//    this->_ios_model = 0;
//    break;

//  case LNA_ANALYSIS:
//    this->_re_model = 0;
//    this->_lna_model = new iNA::Models::LNAmodel(document->getModel());
//    this->_ios_model = 0;
//    break;

//  case IOS_ANALYSIS:
//    this->_re_model = 0;
//    this->_lna_model = 0;
//    this->_ios_model = new iNA::Models::IOSmodel(document->getModel());
//    break;

//  case UNDEFINED_ANALYSIS:
//    this->_re_model = 0;
//    this->_lna_model = 0;
//    this->_ios_model = 0;
//    break;
//  }
}

void
SSETaskConfig::setModel(iNA::Ast::Model *m)
{
    this->_re_model = m;
}

iNA::Ast::Model *
SSETaskConfig::getModel() const
{
  return _re_model;
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
