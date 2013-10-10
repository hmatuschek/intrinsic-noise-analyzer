#include "exportmodel.hh"
#include "parser/sbml/sbml.hh"
#include "parser/sbmlsh/sbmlsh.hh"
#include "exception.hh"

#include <QFileDialog>
#include <QString>
#include <QMessageBox>


bool exportModel(iNA::Ast::Model &model, const QString &path)
{
  // Ask for filename and type:
  QString selected_filter("");
  QString filename(path);

  filename = QFileDialog::getSaveFileName(
        0, QObject::tr("Export model"), filename,
        QObject::tr("SBML (*.xml *.sbml);;SBML-sh (*.mod *.sbmlsh)"), &selected_filter);
  if ("" == filename) { return false; }

  // Serialize model into file...
  try {
    if ("SBML (*.xml *.sbml)" == selected_filter) {
      QFileInfo info(filename);
      if ( ("xml" != info.suffix()) && ("sbml" != info.suffix()) ) { filename.append(".xml"); }
      iNA::Parser::Sbml::exportModel(model, filename.toLocal8Bit().data());
      return true;
    } else if ("SBML-sh (*.mod *.sbmlsh)" == selected_filter){
      QFileInfo info(filename);
      if ( ("mod" != info.suffix()) && ("sbmlsh" != info.suffix()) ) { filename.append(".sbmlsh"); }
      iNA::Parser::Sbmlsh::exportModel(model, filename.toLocal8Bit().data());
      return true;
    } else {
      QMessageBox::critical(
            0, QObject::tr("Can not export model"),
            QObject::tr("Unkown file type: %1").arg(selected_filter));
    }
  } catch (iNA::Exception &err) {
    QMessageBox::warning(0, QObject::tr("Can not export model"), err.what());
  }
  return false;
}
