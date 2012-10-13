#
# Build script for the GUI application:
#

SET(ina_app_base_SOURCES main.cc configuration.cc
    application.cc mainwindow.cc task.cc timeseries.cc module.cc)
SET(ina_app_base_HEADERS
    application.hh mainwindow.hh task.hh timeseries.hh module.hh)

SET(ina_app_plot_SOURCES plot/axisticks.cc plot/extent.cc  plot/configuration.cc
    plot/canvas.cc plot/axis.cc plot/graph.cc plot/legend.cc
    plot/variancelinegraph.cc plot/mapping.cc plot/figure.cc plot/linegraph.cc plot/savedialog.cc
    plot/boxplot.cc plot/plotrangedialog.cc)
SET(ina_app_plot_HEADERS
    plot/canvas.hh plot/axis.hh plot/graph.hh plot/legend.hh
    plot/variancelinegraph.hh plot/mapping.hh plot/figure.hh plot/linegraph.hh plot/savedialog.hh
    plot/boxplot.hh plot/plotrangedialog.hh)

SET(ina_app_re_SOURCES
    re/remodule.cc re/retask.cc re/retaskwrapper.cc re/retaskview.cc re/rewizard.cc re/replot.cc)
SET(ina_app_re_HEADERS
    re/remodule.hh re/retask.hh re/retaskwrapper.hh re/retaskview.hh re/rewizard.hh re/replot.hh)

SET(ina_app_lna_SOURCES
    lna/lnawizard.cc lna/lnatask.cc lna/lnataskwrapper.cc lna/lnataskview.cc
    lna/lnaplot.cc lna/lnamodule.cc)
SET(ina_app_lna_HEADERS
    lna/lnawizard.hh lna/lnatask.hh lna/lnataskwrapper.hh lna/lnataskview.hh
    lna/lnaplot.hh lna/lnamodule.hh)

SET(ina_app_sse_SOURCES sse/ssetaskconfig.cc
    sse/ssemodule.cc sse/ssewizard.cc
    sse/iostask.cc sse/iostaskwrapper.cc sse/iostaskview.cc sse/iosplot.cc
    sse/lnatask.cc sse/lnataskwrapper.cc sse/lnataskview.cc sse/lnaplot.cc
    sse/retask.cc  sse/retaskwrapper.cc  sse/retaskview.cc  sse/replot.cc
    sse/replotdialog.cc)
SET(ina_app_sse_HEADERS
    sse/ssemodule.hh sse/ssewizard.hh
    sse/iostask.hh sse/iostaskwrapper.hh sse/iostaskview.hh sse/iosplot.hh
    sse/lnatask.hh sse/lnataskwrapper.hh sse/lnataskview.hh sse/lnaplot.hh
    sse/retask.hh  sse/retaskwrapper.hh  sse/retaskview.hh  sse/replot.hh
    sse/replotdialog.hh)

SET(ina_app_steadystate_SOURCES
    steadystate/lnasteadystatetask.cc steadystate/lnasteadystatetaskwrapper.cc
    steadystate/lnasteadystatewizard.cc steadystate/lnasteadystateview.cc
    steadystate/lnasteadystatemodule.cc steadystate/steadystatespectrumplot.cc
    steadystate/plotdialog.cc)
SET(ina_app_steadystate_HEADERS
    steadystate/lnasteadystatetask.hh steadystate/lnasteadystatetaskwrapper.hh
    steadystate/lnasteadystatewizard.hh steadystate/lnasteadystateview.hh
    steadystate/lnasteadystatemodule.hh steadystate/steadystatespectrumplot.hh
    steadystate/plotdialog.hh)

SET(ina_app_ssa_SOURCES
    ssa/ssamodule.cc ssa/ssatask.cc ssa/ssawizard.cc ssa/ssataskwrapper.cc ssa/ssaplot.cc
    ssa/ssaview.cc)
SET(ina_app_ssa_HEADERS
    ssa/ssamodule.hh ssa/ssatask.hh ssa/ssawizard.hh ssa/ssataskwrapper.hh ssa/ssaplot.hh
    ssa/ssaview.hh)

SET(ina_app_ssscan_SOURCES
    ssscan/ssscantask.cc ssscan/ssscanwizard.cc ssscan/ssscanmodule.cc ssscan/ssscantaskview.cc
    ssscan/ssscantaskwrapper.cc ssscan/ssscanplot.cc)
SET(ina_app_ssscan_HEADERS
    ssscan/ssscantask.hh ssscan/ssscanwizard.hh ssscan/ssscanmodule.hh ssscan/ssscantaskview.hh
    ssscan/ssscantaskwrapper.hh ssscan/ssscanplot.hh)

SET(ina_app_doctree_SOURCES doctree/documenttreeitem.cc doctree/treeitem.cc
    doctree/compartmentsitem.cc doctree/documenttree.cc doctree/speciesitem.cc
    doctree/parametersitem.cc doctree/reactionitem.cc doctree/reactionsitem.cc
    doctree/modelitem.cc doctree/tableitem.cc doctree/plotitem.cc doctree/taskitem.cc
    doctree/analysesitem.cc doctree/documentitem.cc)
SET(ina_app_doctree_HEADERS
    doctree/compartmentsitem.hh doctree/documenttree.hh doctree/speciesitem.hh
    doctree/parametersitem.hh doctree/reactionitem.hh doctree/reactionsitem.hh
    doctree/modelitem.hh doctree/tableitem.hh doctree/plotitem.hh doctree/taskitem.hh
    doctree/analysesitem.hh doctree/documentitem.hh)

SET(ina_app_models_SOURCES
    models/parameterlist.cc models/specieslist.cc models/compartmentlist.cc
    models/reactionparameterlist.cc models/taskerror.cc models/tablewrapper.cc
    models/generaltaskconfig.cc models/compartmentdelegate.cc models/variableruledelegate.cc
    models/variableruledata.cc models/referencecounter.cc models/scopeitemmodel.cc
    models/expressiondelegate.cc models/sbmlshhighlighter.cc models/reactionlist.cc
    models/pixmapdelegate.cc models/plotformulaparser.cc)
SET(ina_app_models_HEADERS
    models/parameterlist.hh models/specieslist.hh models/compartmentlist.hh
    models/reactionparameterlist.hh models/taskerror.hh models/tablewrapper.hh
    models/generaltaskconfig.hh models/compartmentdelegate.hh models/variableruledelegate.hh
    models/variableruledata.hh models/referencecounter.hh models/scopeitemmodel.hh
    models/expressiondelegate.hh models/sbmlshhighlighter.hh models/reactionlist.hh
    models/pixmapdelegate.hh models/plotformulaparser.hh)

SET(ina_app_views_SOURCES
    views/documentsview.cc views/parameterview.cc views/reactionview.cc
    views/reactionequationrenderer.cc views/compartmentview.cc views/speciesview.cc
    views/plotview.cc views/taskerrorwidget.cc views/aboutdialog.cc
    views/taskview.cc views/generaltaskwizard.cc views/varianceplot.cc views/logwindow.cc
    views/fileselector.cc views/importmodeldialog.cc views/exportmodeldialog.cc
    views/variableruleeditor.cc views/expressioneditor.cc views/unitrenderer.cc
    views/sbmlsheditordialog.cc views/modelview.cc views/uniteditor.cc views/reactionlistview.cc
    views/reactioneditor.cc views/timeseriesplotwizard.cc)
SET(ina_app_views_HEADERS
    views/documentsview.hh views/parameterview.hh views/reactionview.hh
    views/reactionequationrenderer.hh views/compartmentview.hh views/speciesview.hh
    views/plotview.hh views/taskerrorwidget.hh views/aboutdialog.hh
    views/taskview.hh views/generaltaskwizard.hh views/varianceplot.hh views/logwindow.hh
    views/fileselector.hh views/importmodeldialog.hh views/exportmodeldialog.hh
    views/variableruleeditor.hh views/expressioneditor.hh views/unitrenderer.hh
    views/sbmlsheditordialog.hh views/modelview.hh views/uniteditor.hh views/reactionlistview.hh
    views/reactioneditor.hh views/timeseriesplotwizard.hh)

SET(ina_app_tinytex_SOURCES
    tinytex/tinytex.cc tinytex/formula.cc tinytex/ginac2formula.cc)
SET(ina_app_tinytex_HEADERS)

SET(ina_app_SOURCES
    ${ina_app_base_SOURCES}
    ${ina_app_doctree_SOURCES}
    ${ina_app_models_SOURCES}
    ${ina_app_views_SOURCES}
    ${ina_app_plot_SOURCES}
    ${ina_app_tinytex_SOURCES}
    ${ina_app_sse_SOURCES}
    ${ina_app_steadystate_SOURCES}
    ${ina_app_ssa_SOURCES}
    ${ina_app_ssscan_SOURCES})
SET(ina_app_HEADERS
    ${ina_app_base_HEADERS}
    ${ina_app_doctree_HEADERS}
    ${ina_app_models_HEADERS}
    ${ina_app_views_HEADERS}
    ${ina_app_plot_HEADERS}
    ${ina_app_tinytex_HEADERS}
    ${ina_app_sse_HEADERS}
    ${ina_app_steadystate_HEADERS}
    ${ina_app_ssa_HEADERS}
    ${ina_app_ssscan_HEADERS})


QT4_WRAP_CPP(ina_app_HEADERS_MOC ${ina_app_HEADERS})


ADD_EXECUTABLE(ina ${ina_app_SOURCES} ${ina_app_HEADERS_MOC})
TARGET_LINK_LIBRARIES(ina ${QT_LIBRARIES} ${LIBS} libina)

INSTALL_TARGETS(/bin ina)
