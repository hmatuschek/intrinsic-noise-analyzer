#include "uniteditor.hh"
#include "unitrenderer.hh"

#include <parser/unit/unitparser.hh>
#include <QMessageBox>


UnitEditor::UnitEditor(QWidget *parent)
  : QStackedWidget(parent), _unit(), _unitDisplay(0), _unitEditor(0)
{
  postConstructor();
}

UnitEditor::UnitEditor(const iNA::Ast::Unit &unit, QWidget *parent)
  : QStackedWidget(parent), _unit(unit), _unitDisplay(0), _unitEditor(0)
{
  postConstructor();
}

void UnitEditor::postConstructor()
{
  // Assemble editor and display
  _unitDisplay = new QLabel(); _unitDisplay->setPixmap(UnitRenderer::toPixmap(_unit));
  _unitEditor  = new QLineEdit(iNA::Parser::Unit::UnitParser::write(_unit).c_str());

  // Assemble stacked widgets
  addWidget(_unitDisplay);
  addWidget(_unitEditor);
  setCurrentIndex(0);

  // Connect signals:
  QObject::connect(_unitEditor, SIGNAL(editingFinished()), this, SLOT(onEditingFinished()));
}


const iNA::Ast::Unit &UnitEditor::unit() const {
  return _unit;
}

void UnitEditor::setUnit(const iNA::Ast::Unit &unit) {
  // Store unit
  _unit = unit;
  // Update display and editor widgets:
  _unitDisplay->setPixmap(UnitRenderer::toPixmap(_unit));
  _unitEditor->setText(iNA::Parser::Unit::UnitParser::write(_unit).c_str());
}


void
UnitEditor::mouseDoubleClickEvent(QMouseEvent *event)
{
  // If not the first widget (formula item) is visible
  if (0 != currentIndex()) { QStackedWidget::mouseDoubleClickEvent(event); return; }
  // Swap widgets:
  setCurrentIndex(1);
  _unitEditor->setFocus();
}


void
UnitEditor::onEditingFinished() {
  // Get text as std::string
  std::string unit_code = _unitEditor->text().toStdString();

  try {
    // parse unit:
    _unit = iNA::Parser::Unit::UnitParser::parse(unit_code);
  } catch (const iNA::Parser::ParserError &err) {
    // Reset to current unit:
    _unitEditor->setText(iNA::Parser::Unit::UnitParser::write(_unit).c_str());
    QMessageBox::critical(0, "Can not set unit.", err.what());
    return;
  }

  // Update unit and emit signal
  _unitDisplay->setPixmap(UnitRenderer::toPixmap(_unit));
  setCurrentIndex(0);
  emit unitChanged();
}
