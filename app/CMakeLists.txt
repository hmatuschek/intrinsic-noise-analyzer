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
    sse/retask.cc  sse/retaskwrapper.cc  sse/retaskview.cc  sse/replot.cc)
SET(ina_app_sse_HEADERS
    sse/ssemodule.hh sse/ssewizard.hh
    sse/iostask.hh sse/iostaskwrapper.hh sse/iostaskview.hh sse/iosplot.hh
    sse/lnatask.hh sse/lnataskwrapper.hh sse/lnataskview.hh sse/lnaplot.hh
    sse/retask.hh  sse/retaskwrapper.hh  sse/retaskview.hh  sse/replot.hh)

SET(ina_app_steadystate_SOURCES
    steadystate/lnasteadystatetask.cc steadystate/lnasteadystatetaskwrapper.cc
    steadystate/lnasteadystatewizard.cc steadystate/lnasteadystateview.cc
    steadystate/lnasteadystatemodule.cc steadystate/steadystatespectrumplot.cc)
SET(ina_app_steadystate_HEADERS
    steadystate/lnasteadystatetask.hh steadystate/lnasteadystatetaskwrapper.hh
    steadystate/lnasteadystatewizard.hh steadystate/lnasteadystateview.hh
    steadystate/lnasteadystatemodule.hh steadystate/steadystatespectrumplot.hh)

SET(ina_app_ssa_SOURCES
    ssa/ssamodule.cc ssa/ssatask.cc ssa/ssawizard.cc ssa/ssataskwrapper.cc ssa/ssaplot.cc
    ssa/ssaview.cc)
SET(ina_app_ssa_HEADERS
    ssa/ssamodule.hh ssa/ssatask.hh ssa/ssawizard.hh ssa/ssataskwrapper.hh ssa/ssaplot.hh
    ssa/ssaview.hh)

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
    models/generaltaskconfig.cc models/compartmentdelegate.cc)
SET(ina_app_models_HEADERS
    models/parameterlist.hh models/specieslist.hh models/compartmentlist.hh
    models/reactionparameterlist.hh models/taskerror.hh models/tablewrapper.hh
    models/generaltaskconfig.hh models/compartmentdelegate.hh)

SET(ina_app_views_SOURCES
    views/documentsview.cc views/parameterview.cc views/reactionview.cc
    views/reactionequationrenderer.cc views/compartmentview.cc views/speciesview.cc
    views/plotview.cc views/taskerrorwidget.cc views/aboutdialog.cc
    views/taskview.cc views/generaltaskwizard.cc views/varianceplot.cc views/logwindow.cc
    views/fileselector.cc views/importmodeldialog.cc views/exportmodeldialog.cc)
SET(ina_app_views_HEADERS
    views/documentsview.hh views/parameterview.hh views/reactionview.hh
    views/reactionequationrenderer.hh views/compartmentview.hh views/speciesview.hh
    views/plotview.hh views/taskerrorwidget.hh views/aboutdialog.hh
    views/taskview.hh views/generaltaskwizard.hh views/varianceplot.hh views/logwindow.hh
    views/fileselector.hh views/importmodeldialog.hh views/exportmodeldialog.hh)


SET(ina_app_SOURCES
    ${ina_app_base_SOURCES}
    ${ina_app_doctree_SOURCES}
    ${ina_app_models_SOURCES}
    ${ina_app_views_SOURCES}
    ${ina_app_plot_SOURCES}
    #${ina_app_re_SOURCES}
    #${ina_app_lna_SOURCES}
    #${ina_app_ios_SOURCES}
    ${ina_app_sse_SOURCES}
    ${ina_app_steadystate_SOURCES}
    ${ina_app_ssa_SOURCES})
SET(ina_app_HEADERS
    ${ina_app_base_HEADERS}
    ${ina_app_doctree_HEADERS}
    ${ina_app_models_HEADERS}
    ${ina_app_views_HEADERS}
    ${ina_app_plot_HEADERS}
    #${ina_app_re_HEADERS}
    #${ina_app_lna_HEADERS}
    #${ina_app_ios_HEADERS}
    ${ina_app_sse_HEADERS}
    ${ina_app_steadystate_HEADERS}
    ${ina_app_ssa_HEADERS})


QT4_WRAP_CPP(ina_app_HEADERS_MOC ${ina_app_HEADERS})


ADD_EXECUTABLE(ina ${ina_app_SOURCES} ${ina_app_HEADERS_MOC})
TARGET_LINK_LIBRARIES(ina ${QT_LIBRARIES} ${LIBS} libina)

INSTALL_TARGETS(/bin ina)
