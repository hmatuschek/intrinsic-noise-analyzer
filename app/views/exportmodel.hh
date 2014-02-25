#ifndef __INA_GUI_EXPORTMODEL_HH__
#define __INA_GUI_EXPORTMODEL_HH__

#include <QString>
#include <QDialog>
#include "ast/model.hh"

/** A simple helper function to export a model.
 * Returns @c true on success. */
bool exportModel(iNA::Ast::Model &model, const QString &path="");

#endif
