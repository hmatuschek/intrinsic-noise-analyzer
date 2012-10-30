SYSTEM_LIBRARY_DIR = /opt/local/lib
SYSTEM_BINARY_DIR = /opt/local/bin
APP_LIBRARY_DIR = IntrinsicNoiseAnalyzer.app/Contents/Frameworks
APP_BINARY_DIR  = IntrinsicNoiseAnalyzer.app/Contents/MacOS
VERSION = 0.4.0

app: dmg

clean:
	rm -f ${APP_BINARY_DIR}/ina
	rm -f ${APP_LIBRARY_DIR}/*.dylib
	rm -f IntrinsicNoiseAnalyzer-${VERSION}.dmg

dmg: ina-app
	hdiutil create -srcfolder IntrinsicNoiseAnalyzer.app IntrinsicNoiseAnalyzer-${VERSION}.dmg

ina-app: libina libQtGui libQtXml libQtSvg libQtCore libQtNetwork libginac libcln libsbml libffi
	cp $(SYSTEM_BINARY_DIR)/ina IntrinsicNoiseAnalyzer.app/Contents/MacOS
	cp ina.icns IntrinsicNoiseAnalyzer.app/Contents/Resources
	install_name_tool -change ${SYSTEM_LIBRARY_DIR}/libina.dylib @executable_path/../Frameworks/libina.dylib ${APP_BINARY_DIR}/ina
	install_name_tool -change ${SYSTEM_LIBRARY_DIR}/libQtGui.4.dylib @executable_path/../Frameworks/libQtGui.4.dylib ${APP_BINARY_DIR}/ina
	install_name_tool -change ${SYSTEM_LIBRARY_DIR}/libQtXml.4.dylib @executable_path/../Frameworks/libQtXml.4.dylib ${APP_BINARY_DIR}/ina
	install_name_tool -change ${SYSTEM_LIBRARY_DIR}/libQtSvg.4.dylib @executable_path/../Frameworks/libQtSvg.4.dylib ${APP_BINARY_DIR}/ina
	install_name_tool -change ${SYSTEM_LIBRARY_DIR}/libQtCore.4.dylib @executable_path/../Frameworks/libQtCore.4.dylib ${APP_BINARY_DIR}/ina
  install_name_tool -change ${SYSTEM_LIBRARY_DIR}/libQtNetwork.4.dylib @executable_path/../Frameworks/libQtNetwork.4.dylib ${APP_BINARY_DIR}/ina
  install_name_tool -change ${SYSTEM_LIBRARY_DIR}/libginac-1.5.0.dylib @executable_path/../Frameworks/libginac-1.5.0.dylib ${APP_BINARY_DIR}/ina
	install_name_tool -change ${SYSTEM_LIBRARY_DIR}/libcln.6.dylib @executable_path/../Frameworks/libcln.6.dylib ${APP_BINARY_DIR}/ina
	install_name_tool -change ${SYSTEM_LIBRARY_DIR}/libsbml.5.dylib @executable_path/../Frameworks/libsbml.5.dylib ${APP_BINARY_DIR}/ina
	install_name_tool -change ${SYSTEM_LIBRARY_DIR}/libffi.6.dylib @executable_path/../Frameworks/libffi.6.dylib ${APP_BINARY_DIR}/ina 

libina: libginac libcln libsbml
	cp $(SYSTEM_LIBRARY_DIR)/libina.dylib ${APP_LIBRARY_DIR}
	install_name_tool -id @executable_path/../Frameworks/libina.dylib ${APP_LIBRARY_DIR}/libina.dylib
	install_name_tool -change ${SYSTEM_LIBRARY_DIR}/libginac-1.5.0.dylib @executable_path/../Frameworks/libginac-1.5.0.dylib ${APP_LIBRARY_DIR}/libina.dylib
	install_name_tool -change ${SYSTEM_LIBRARY_DIR}/libcln.6.dylib @executable_path/../Frameworks/libcln.6.dylib ${APP_LIBRARY_DIR}/libina.dylib
	install_name_tool -change ${SYSTEM_LIBRARY_DIR}/libsbml.5.dylib @executable_path/../Frameworks/libsbml.5.dylib ${APP_LIBRARY_DIR}/libina.dylib
	install_name_tool -change ${SYSTEM_LIBRARY_DIR}/libffi.6.dylib @executable_path/../Frameworks/libffi.6.dylib ${APP_LIBRARY_DIR}/libina.dylib 

libQtGui: libQtCore libz libpng
	cp ${SYSTEM_LIBRARY_DIR}/libQtGui.4.dylib ${APP_LIBRARY_DIR}
	install_name_tool -id @executable_path/../Frameworks/libQtGui.4.dylib ${APP_LIBRARY_DIR}/libQtGui.4.dylib
	install_name_tool -change ${SYSTEM_LIBRARY_DIR}/libQtCore.4.dylib @executable_path/../Frameworks/libQtCore.4.dylib ${APP_LIBRARY_DIR}/libQtGui.4.dylib
	install_name_tool -change ${SYSTEM_LIBRARY_DIR}/libz.1.dylib @executable_path/../Frameworks/libz.1.dylib ${APP_LIBRARY_DIR}/libQtGui.4.dylib
	install_name_tool -change ${SYSTEM_LIBRARY_DIR}/libpng14.14.dylib @executable_path/../Frameworks/libpng14.14.dylib ${APP_LIBRARY_DIR}/libQtGui.4.dylib
	install_name_tool -change ${SYSTEM_LIBRARY_DIR}/libpng14.14.dylib @executable_path/../Frameworks/libpng14.14.dylib ${APP_LIBRARY_DIR}/libQtGui.4.dylib

libQtXml: libQtCore 
	cp ${SYSTEM_LIBRARY_DIR}/libQtXml.4.dylib ${APP_LIBRARY_DIR}
	install_name_tool -id @executable_path/../Frameworks/libQtXml.4.dylib ${APP_LIBRARY_DIR}/libQtXml.4.dylib
	install_name_tool -change ${SYSTEM_LIBRARY_DIR}/libQtCore.4.dylib @executable_path/../Frameworks/libQtCore.4.dylib ${APP_LIBRARY_DIR}/libQtXml.4.dylib
	
libQtSvg: libQtCore libQtXml libz
	cp ${SYSTEM_LIBRARY_DIR}/libQtSvg.4.dylib ${APP_LIBRARY_DIR}
	install_name_tool -id @executable_path/../Frameworks/libQtSvg.4.dylib ${APP_LIBRARY_DIR}/libQtSvg.4.dylib
	install_name_tool -change ${SYSTEM_LIBRARY_DIR}/libQtCore.4.dylib @executable_path/../Frameworks/libQtCore.4.dylib ${APP_LIBRARY_DIR}/libQtSvg.4.dylib
	install_name_tool -change ${SYSTEM_LIBRARY_DIR}/libQtGui.4.dylib @executable_path/../Frameworks/libQtGui.4.dylib ${APP_LIBRARY_DIR}/libQtSvg.4.dylib
	install_name_tool -change ${SYSTEM_LIBRARY_DIR}/libz.1.dylib @executable_path/../Frameworks/libz.1.dylib ${APP_LIBRARY_DIR}/libQtSvg.4.dylib

libQtCore: libz 
	cp ${SYSTEM_LIBRARY_DIR}/libQtCore.4.dylib ${APP_LIBRARY_DIR}
	install_name_tool -id @executable_path/../Frameworks/libQtCore.4.dylib ${APP_LIBRARY_DIR}/libQtCore.4.dylib
	install_name_tool -change ${SYSTEM_LIBRARY_DIR}/libz.1.dylib @executable_path/../Frameworks/libz.1.dylib ${APP_LIBRARY_DIR}/libQtCore.4.dylib

libQtNetwork: libz libQtCore libssl libcrypto
  cp ${SYSTEM_LIBRARY_DIR}/libQtNetwork.4.dylib ${APP_LIBRARY_DIR}
  install_name_tool -id @executable_path/../Frameworks/libQtNetwork.4.dylib ${APP_LIBRARY_DIR}/libQtNetwork.4.dylib
  install_name_tool -change ${SYSTEM_LIBRARY_DIR}/libQtCore.4.dylib @executable_path/../Frameworks/libQtCore.4.dylib ${APP_LIBRARY_DIR}/libQtNetwork.4.dylib
  install_name_tool -change ${SYSTEM_LIBRARY_DIR}/libz.1.dylib @executable_path/../Frameworks/libz.1.dylib ${APP_LIBRARY_DIR}/libQtCore.4.dylib

libginac: libcln libgmp
	cp ${SYSTEM_LIBRARY_DIR}/libginac-1.5.0.dylib ${APP_LIBRARY_DIR}
	install_name_tool -id @executable_path/../Frameworks/libginac-1.5.0.dylib ${APP_LIBRARY_DIR}/libginac-1.5.0.dylib
	install_name_tool -change ${SYSTEM_LIBRARY_DIR}/libcln.6.dylib @executable_path/../Frameworks/libcln.6.dylib ${APP_LIBRARY_DIR}/libginac-1.5.0.dylib
	install_name_tool -change ${SYSTEM_LIBRARY_DIR}/libgmp.10.dylib @executable_path/../Frameworks/libgmp.10.dylib ${APP_LIBRARY_DIR}/libginac-1.5.0.dylib

libcln: libgmp
	cp ${SYSTEM_LIBRARY_DIR}/libcln.6.dylib  ${APP_LIBRARY_DIR}
	install_name_tool -id @executable_path/../Frameworks/libcln.6.dylib ${APP_LIBRARY_DIR}/libcln.6.dylib
	install_name_tool -change ${SYSTEM_LIBRARY_DIR}/libgmp.10.dylib @executable_path/../Frameworks/libgmp.10.dylib ${APP_LIBRARY_DIR}/libcln.6.dylib

libsbml: libxml2 libz libiconv libbz2
	cp ${SYSTEM_LIBRARY_DIR}/libsbml.5.dylib ${APP_LIBRARY_DIR}
	install_name_tool -id @executable_path/../Frameworks/libsbml.5.dylib ${APP_LIBRARY_DIR}/libsbml.5.dylib
	install_name_tool -change ${SYSTEM_LIBRARY_DIR}/libxml2.2.dylib @executable_path/../Frameworks/libxml2.2.dylib ${APP_LIBRARY_DIR}/libsbml.5.dylib
	install_name_tool -change ${SYSTEM_LIBRARY_DIR}/libz.1.dylib @executable_path/../Frameworks/libz.1.dylib ${APP_LIBRARY_DIR}/libsbml.5.dylib
	install_name_tool -change ${SYSTEM_LIBRARY_DIR}/libiconv.2.dylib @executable_path/../Frameworks/libiconv.2.dylib ${APP_LIBRARY_DIR}/libsbml.5.dylib
	install_name_tool -change ${SYSTEM_LIBRARY_DIR}/libbz2.1.0.dylib @executable_path/../Frameworks/libbz2.1.0.dylib ${APP_LIBRARY_DIR}/libsbml.5.dylib

libz:
	cp ${SYSTEM_LIBRARY_DIR}/libz.1.dylib ${APP_LIBRARY_DIR}
	install_name_tool -id @executable_path/../Frameworks/libz.1.dylib ${APP_LIBRARY_DIR}/libz.1.dylib

libpng: libz
	cp ${SYSTEM_LIBRARY_DIR}/libpng14.14.dylib ${APP_LIBRARY_DIR}
	install_name_tool -id @executable_path/../Frameworks/libpng14.14.dylib ${APP_LIBRARY_DIR}/libpng14.14.dylib
	install_name_tool -change ${SYSTEM_LIBRARY_DIR}/libz.1.dylib @executable_path/../Frameworks/libz.1.dylib ${APP_LIBRARY_DIR}/libpng14.14.dylib

libgmp: 
	cp ${SYSTEM_LIBRARY_DIR}/libgmp.10.dylib ${APP_LIBRARY_DIR}
	install_name_tool -id @executable_path/../Frameworks/libgmp.10.dylib ${APP_LIBRARY_DIR}/libgmp.10.dylib

libxml2: libz libiconv
	cp ${SYSTEM_LIBRARY_DIR}/libxml2.2.dylib ${APP_LIBRARY_DIR}
	install_name_tool -id @executable_path/../Frameworks/libxml2.2.dylib ${APP_LIBRARY_DIR}/libxml2.2.dylib
	install_name_tool -change ${SYSTEM_LIBRARY_DIR}/libz.1.dylib @executable_path/../Frameworks/libz.1.dylib ${APP_LIBRARY_DIR}/libxml2.2.dylib
	install_name_tool -change ${SYSTEM_LIBRARY_DIR}/libiconv.2.dylib @executable_path/../Frameworks/libiconv.2.dylib ${APP_LIBRARY_DIR}/libxml2.2.dylib

libiconv: 
	cp ${SYSTEM_LIBRARY_DIR}/libiconv.2.dylib ${APP_LIBRARY_DIR}
	install_name_tool -id @executable_path/../Frameworks/libiconv.2.dylib ${APP_LIBRARY_DIR}/libiconv.2.dylib

libbz2:
	cp ${SYSTEM_LIBRARY_DIR}/libbz2.1.0.dylib ${APP_LIBRARY_DIR}
	install_name_tool -id @executable_path/../Frameworks/libbz2.1.0.dylib ${APP_LIBRARY_DIR}/libbz2.1.0.dylib

libffi:
	cp ${SYSTEM_LIBRARY_DIR}/libffi.6.dylib ${APP_LIBRARY_DIR}
	install_name_tool -id @executable_path/../Frameworks/libffi.6.dylib ${APP_LIBRARY_DIR}/libffi.6.dylib

libssl: libcrypto libz
  cp ${SYSTEM_LIBRARY_DIR}/libssl.1.dylib ${APP_LIBRARY_DIR}
  install_name_tool -id @executable_path/../Frameworks/libssl.1.dylib ${APP_LIBRARY_DIR}/libssl.1.dylib
  install_name_tool -change ${SYSTEM_LIBRARY_DIR}/libz.1.dylib @executable_path/../Frameworks/libz.1.dylib ${APP_LIBRARY_DIR}/libssl.1.dylib
  install_name_tool -change ${SYSTEM_LIBRARY_DIR}/libcrypto.1.dylib @executable_path/../Frameworks/libcrypto.1.dylib ${APP_LIBRARY_DIR}/libssl.1.dylib

libcrypto:
  cp ${SYSTEM_LIBRARY_DIR}/libcrypto.1.dylib ${APP_LIBRARY_DIR}
  install_name_tool -id @executable_path/../Frameworks/libcrypto.1.dylib ${APP_LIBRARY_DIR}/libcrypto.1.dylib
  install_name_tool -change ${SYSTEM_LIBRARY_DIR}/libz.1.dylib @executable_path/../Frameworks/libz.1.dylib ${APP_LIBRARY_DIR}/libcrypto.1.dylib

