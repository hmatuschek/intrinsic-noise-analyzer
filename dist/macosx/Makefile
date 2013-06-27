SYSTEM_LIBRARY_DIR = /opt/local/lib
SYSTEM_BINARY_DIR = /opt/local/bin
QTCORE_FRAMEWORK = /opt/local/Library/Frameworks/QtCore.framework/Versions/4/QtCore
QTGUI_FRAMEWORK = /opt/local/Library/Frameworks/QtGui.framework/Versions/4/QtGui
QTXML_FRAMEWORK = /opt/local/Library/Frameworks/QtXml.framework/Versions/4/QtXml
QTSVG_FRAMEWORK = /opt/local/Library/Frameworks/QtSvg.framework/Versions/4/QtSvg
QTNET_FRAMEWORK = /opt/local/Library/Frameworks/QtNetwork.framework/Versions/4/QtNetwork
APP_LIBRARY_DIR = IntrinsicNoiseAnalyzer.app/Contents/Frameworks
APP_BINARY_DIR  = IntrinsicNoiseAnalyzer.app/Contents/MacOS
VERSION = 0.4.1

app: dmg

clean:
	rm -f ${APP_BINARY_DIR}/ina
	rm -f ${APP_LIBRARY_DIR}/*.dylib
	rm -f IntrinsicNoiseAnalyzer-${VERSION}.dmg

dmg: ina-app
	hdiutil create -srcfolder IntrinsicNoiseAnalyzer.app IntrinsicNoiseAnalyzer-${VERSION}.dmg


ina-app: libina QtGui QtXml QtSvg QtCore QtNetwork libginac libcln libsbml libffi libffi-gcc47 libgomp libgcc_s libstdc++
	cp $(SYSTEM_BINARY_DIR)/ina IntrinsicNoiseAnalyzer.app/Contents/MacOS
	cp ina.icns IntrinsicNoiseAnalyzer.app/Contents/Resources
	install_name_tool -change ${SYSTEM_LIBRARY_DIR}/libina.0.4.2.dylib @executable_path/../Frameworks/libina.0.4.2.dylib ${APP_BINARY_DIR}/ina
	install_name_tool -change ${QTCORE_FRAMEWORK} @executable_path/../Frameworks/QtCore ${APP_BINARY_DIR}/ina
	install_name_tool -change ${QTGUI_FRAMEWORK} @executable_path/../Frameworks/QtGui ${APP_BINARY_DIR}/ina
	install_name_tool -change ${QTXML_FRAMEWORK} @executable_path/../Frameworks/QtXml ${APP_BINARY_DIR}/ina
	install_name_tool -change ${QTSVG_FRAMEWORK} @executable_path/../Frameworks/QtSvg ${APP_BINARY_DIR}/ina
	install_name_tool -change ${QTNET_FRAMEWORK} @executable_path/../Frameworks/QtNetwork ${APP_BINARY_DIR}/ina
	install_name_tool -change ${SYSTEM_LIBRARY_DIR}/libginac.2.dylib @executable_path/../Frameworks/libginac.2.dylib ${APP_BINARY_DIR}/ina
	install_name_tool -change ${SYSTEM_LIBRARY_DIR}/libcln.6.dylib @executable_path/../Frameworks/libcln.6.dylib ${APP_BINARY_DIR}/ina
	install_name_tool -change ${SYSTEM_LIBRARY_DIR}/libsbml.5.dylib @executable_path/../Frameworks/libsbml.5.dylib ${APP_BINARY_DIR}/ina
	install_name_tool -change ${SYSTEM_LIBRARY_DIR}/libffi.6.dylib @executable_path/../Frameworks/libffi.6.dylib ${APP_BINARY_DIR}/ina 
	install_name_tool -change ${SYSTEM_LIBRARY_DIR}/gcc47/libffi.4.dylib @executable_path/../Frameworks/gcc47/libffi.4.dylib ${APP_BINARY_DIR}/ina 
	install_name_tool -change ${SYSTEM_LIBRARY_DIR}/gcc47/libgomp.1.dylib @executable_path/../Frameworks/gcc47/libgomp.1.dylib ${APP_BINARY_DIR}/ina 
	install_name_tool -change ${SYSTEM_LIBRARY_DIR}/gcc47/libgcc_s.1.dylib @executable_path/../Frameworks/gcc47/libgcc_s.1.dylib ${APP_BINARY_DIR}/ina 
	install_name_tool -change ${SYSTEM_LIBRARY_DIR}/libstdc++.6.dylib @executable_path/../Frameworks/libstdc++.6.dylib ${APP_BINARY_DIR}/ina 

libina: libginac libcln libsbml
	cp $(SYSTEM_LIBRARY_DIR)/libina.0.4.2.dylib ${APP_LIBRARY_DIR}
	install_name_tool -id @executable_path/../Frameworks/libina.0.4.2.dylib ${APP_LIBRARY_DIR}/libina.0.4.2.dylib
	install_name_tool -change ${SYSTEM_LIBRARY_DIR}/libginac.2.dylib @executable_path/../Frameworks/libginac.2.dylib ${APP_LIBRARY_DIR}/libina.0.4.2.dylib
	install_name_tool -change ${SYSTEM_LIBRARY_DIR}/libcln.6.dylib @executable_path/../Frameworks/libcln.6.dylib ${APP_LIBRARY_DIR}/libina.0.4.2.dylib
	install_name_tool -change ${SYSTEM_LIBRARY_DIR}/libsbml.5.dylib @executable_path/../Frameworks/libsbml.5.dylib ${APP_LIBRARY_DIR}/libina.0.4.2.dylib
	install_name_tool -change ${SYSTEM_LIBRARY_DIR}/libffi.6.dylib @executable_path/../Frameworks/libffi.6.dylib ${APP_LIBRARY_DIR}/libina.0.4.2.dylib 
	install_name_tool -change ${SYSTEM_LIBRARY_DIR}/gcc47/libffi.4.dylib @executable_path/../Frameworks/gcc47/libffi.4.dylib ${APP_LIBRARY_DIR}/libina.0.4.2.dylib 
	install_name_tool -change ${SYSTEM_LIBRARY_DIR}/gcc47/libgomp.1.dylib @executable_path/../Frameworks/gcc47/libgomp.1.dylib ${APP_LIBRARY_DIR}/libina.0.4.2.dylib 
	install_name_tool -change ${SYSTEM_LIBRARY_DIR}/gcc47/libgcc_s.1.dylib @executable_path/../Frameworks/gcc47/libgcc_s.1.dylib ${APP_LIBRARY_DIR}/libina.0.4.2.dylib 
	install_name_tool -change ${SYSTEM_LIBRARY_DIR}/libstdc++.6.dylib @executable_path/../Frameworks/libstdc++.6.dylib ${APP_LIBRARY_DIR}/libina.0.4.2.dylib 

QtGui: QtCore libz libpng
	cp ${QTGUI_FRAMEWORK} ${APP_LIBRARY_DIR}
	install_name_tool -id @executable_path/../Frameworks/QtGui ${APP_LIBRARY_DIR}/QtGui
	install_name_tool -change ${QTCORE_FRAMEWORK} @executable_path/../Frameworks/QtCore ${APP_LIBRARY_DIR}/QtGui
	install_name_tool -change ${SYSTEM_LIBRARY_DIR}/libz.1.dylib @executable_path/../Frameworks/libz.1.dylib ${APP_LIBRARY_DIR}/QtGui
	install_name_tool -change ${SYSTEM_LIBRARY_DIR}/libpng15.15.dylib @executable_path/../Frameworks/libpng15.15.dylib ${APP_LIBRARY_DIR}/QtGui

QtXml: QtCore
	cp ${QTXML_FRAMEWORK} ${APP_LIBRARY_DIR}
	install_name_tool -id @executable_path/../Frameworks/QtXml ${APP_LIBRARY_DIR}/QtXml
	install_name_tool -change ${QTCORE_FRAMEWORK} @executable_path/../Frameworks/QtCore ${APP_LIBRARY_DIR}/QtXml

QtSvg: QtCore QtXml libz
	cp ${QTSVG_FRAMEWORK} ${APP_LIBRARY_DIR}
	install_name_tool -id @executable_path/../Frameworks/QtSvg ${APP_LIBRARY_DIR}/QtSvg
	install_name_tool -change ${QTCORE_FRAMEWORK} @executable_path/../Frameworks/QtCore ${APP_LIBRARY_DIR}/QtSvg
	install_name_tool -change ${QTGUI_FRAMEWORK} @executable_path/../Frameworks/QtGui ${APP_LIBRARY_DIR}/QtSvg
	install_name_tool -change ${SYSTEM_LIBRARY_DIR}/libz.1.dylib @executable_path/../Frameworks/libz.1.dylib ${APP_LIBRARY_DIR}/QtSvg

QtCore: libz
	cp ${QTCORE_FRAMEWORK} ${APP_LIBRARY_DIR}
	install_name_tool -id @executable_path/../Frameworks/QtCore ${APP_LIBRARY_DIR}/QtCore
	install_name_tool -change ${SYSTEM_LIBRARY_DIR}/libz.1.dylib @executable_path/../Frameworks/libz.1.dylib ${APP_LIBRARY_DIR}/QtCore

QtNetwork: libz QtCore libssl libcrypto
	cp ${QTNET_FRAMEWORK} ${APP_LIBRARY_DIR}
	install_name_tool -id @executable_path/../Frameworks/QtNetwork ${APP_LIBRARY_DIR}/QtNetwork
	install_name_tool -change ${QTCORE_FRAMEWORK} @executable_path/../Frameworks/QtCore ${APP_LIBRARY_DIR}/QtNetwork
	install_name_tool -change ${SYSTEM_LIBRARY_DIR}/libz.1.dylib @executable_path/../Frameworks/libz.1.dylib ${APP_LIBRARY_DIR}/QtNetwork
	install_name_tool -change ${SYSTEM_LIBRARY_DIR}/libcrypto.1.0.0.dylib @executable_path/../Frameworks/libcrypto.1.0.0.dylib ${APP_LIBRARY_DIR}/QtNetwork
	install_name_tool -change ${SYSTEM_LIBRARY_DIR}/libssl.1.0.0.dylib @executable_path/../Frameworks/libssl.1.0.0.dylib ${APP_LIBRARY_DIR}/QtNetwork

libginac: libcln libgmp
	cp ${SYSTEM_LIBRARY_DIR}/libginac.2.dylib ${APP_LIBRARY_DIR}
	install_name_tool -id @executable_path/../Frameworks/libginac.2.dylib ${APP_LIBRARY_DIR}/libginac.2.dylib
	install_name_tool -change ${SYSTEM_LIBRARY_DIR}/libcln.6.dylib @executable_path/../Frameworks/libcln.6.dylib ${APP_LIBRARY_DIR}/libginac.2.dylib
	install_name_tool -change ${SYSTEM_LIBRARY_DIR}/libgmp.10.dylib @executable_path/../Frameworks/libgmp.10.dylib ${APP_LIBRARY_DIR}/libginac.2.dylib
	install_name_tool -change ${SYSTEM_LIBRARY_DIR}/gcc47/libgcc_s.1.dylib @executable_path/../Frameworks/gcc47/libgcc_s.1.dylib ${APP_LIBRARY_DIR}/libginac.2.dylib 
	install_name_tool -change ${SYSTEM_LIBRARY_DIR}/libstdc++.6.dylib @executable_path/../Frameworks/libstdc++.6.dylib ${APP_LIBRARY_DIR}/libginac.2.dylib 

libcln: libgmp
	cp ${SYSTEM_LIBRARY_DIR}/libcln.6.dylib  ${APP_LIBRARY_DIR}
	install_name_tool -id @executable_path/../Frameworks/libcln.6.dylib ${APP_LIBRARY_DIR}/libcln.6.dylib
	install_name_tool -change ${SYSTEM_LIBRARY_DIR}/libgmp.10.dylib @executable_path/../Frameworks/libgmp.10.dylib ${APP_LIBRARY_DIR}/libcln.6.dylib
	install_name_tool -change ${SYSTEM_LIBRARY_DIR}/gcc47/libgcc_s.1.dylib @executable_path/../Frameworks/gcc47/libgcc_s.1.dylib ${APP_LIBRARY_DIR}/libcln.6.dylib 
	install_name_tool -change ${SYSTEM_LIBRARY_DIR}/libstdc++.6.dylib @executable_path/../Frameworks/libstdc++.6.dylib ${APP_LIBRARY_DIR}/libcln.6.dylib 

libsbml: libxml2 libz libiconv libbz2
	cp ${SYSTEM_LIBRARY_DIR}/libsbml.5.dylib ${APP_LIBRARY_DIR}
	install_name_tool -id @executable_path/../Frameworks/libsbml.5.dylib ${APP_LIBRARY_DIR}/libsbml.5.dylib
	install_name_tool -change ${SYSTEM_LIBRARY_DIR}/libxml2.2.dylib @executable_path/../Frameworks/libxml2.2.dylib ${APP_LIBRARY_DIR}/libsbml.5.dylib
	install_name_tool -change ${SYSTEM_LIBRARY_DIR}/libz.1.dylib @executable_path/../Frameworks/libz.1.dylib ${APP_LIBRARY_DIR}/libsbml.5.dylib
	install_name_tool -change ${SYSTEM_LIBRARY_DIR}/libiconv.2.dylib @executable_path/../Frameworks/libiconv.2.dylib ${APP_LIBRARY_DIR}/libsbml.5.dylib
	install_name_tool -change ${SYSTEM_LIBRARY_DIR}/libbz2.1.0.dylib @executable_path/../Frameworks/libbz2.1.0.dylib ${APP_LIBRARY_DIR}/libsbml.5.dylib
	install_name_tool -change ${SYSTEM_LIBRARY_DIR}/liblzma.5.dylib @executable_path/../Frameworks/liblzma.5.dylib ${APP_LIBRARY_DIR}/libsbml.5.dylib

libz:
	cp ${SYSTEM_LIBRARY_DIR}/libz.1.dylib ${APP_LIBRARY_DIR}
	install_name_tool -id @executable_path/../Frameworks/libz.1.dylib ${APP_LIBRARY_DIR}/libz.1.dylib

libpng: libz
	cp ${SYSTEM_LIBRARY_DIR}/libpng15.15.dylib ${APP_LIBRARY_DIR}
	install_name_tool -id @executable_path/../Frameworks/libpng15.15.dylib ${APP_LIBRARY_DIR}/libpng15.15.dylib
	install_name_tool -change ${SYSTEM_LIBRARY_DIR}/libz.1.dylib @executable_path/../Frameworks/libz.1.dylib ${APP_LIBRARY_DIR}/libpng15.15.dylib

libgmp: 
	cp ${SYSTEM_LIBRARY_DIR}/libgmp.10.dylib ${APP_LIBRARY_DIR}
	install_name_tool -id @executable_path/../Frameworks/libgmp.10.dylib ${APP_LIBRARY_DIR}/libgmp.10.dylib

libxml2: libz libiconv liblzma
	cp ${SYSTEM_LIBRARY_DIR}/libxml2.2.dylib ${APP_LIBRARY_DIR}
	install_name_tool -id @executable_path/../Frameworks/libxml2.2.dylib ${APP_LIBRARY_DIR}/libxml2.2.dylib
	install_name_tool -change ${SYSTEM_LIBRARY_DIR}/libz.1.dylib @executable_path/../Frameworks/libz.1.dylib ${APP_LIBRARY_DIR}/libxml2.2.dylib
	install_name_tool -change ${SYSTEM_LIBRARY_DIR}/libiconv.2.dylib @executable_path/../Frameworks/libiconv.2.dylib ${APP_LIBRARY_DIR}/libxml2.2.dylib
	install_name_tool -change ${SYSTEM_LIBRARY_DIR}/liblzma.5.dylib @executable_path/../Frameworks/liblzma.5.dylib ${APP_LIBRARY_DIR}/libxml2.2.dylib

liblzma: 
	cp ${SYSTEM_LIBRARY_DIR}/liblzma.5.dylib ${APP_LIBRARY_DIR}
	install_name_tool -id @executable_path/../Frameworks/liblzma.5.dylib ${APP_LIBRARY_DIR}/liblzma.5.dylib

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
	cp ${SYSTEM_LIBRARY_DIR}/libssl.1.0.0.dylib ${APP_LIBRARY_DIR}
	chmod u+w ${APP_LIBRARY_DIR}/libssl.1.0.0.dylib
	install_name_tool -id @executable_path/../Frameworks/libssl.1.0.0.dylib ${APP_LIBRARY_DIR}/libssl.1.0.0.dylib
	install_name_tool -change ${SYSTEM_LIBRARY_DIR}/libz.1.dylib @executable_path/../Frameworks/libz.1.dylib ${APP_LIBRARY_DIR}/libssl.1.0.0.dylib
	install_name_tool -change ${SYSTEM_LIBRARY_DIR}/libcrypto.1.0.0.dylib @executable_path/../Frameworks/libcrypto.1.0.0.dylib ${APP_LIBRARY_DIR}/libssl.1.0.0.dylib

libcrypto:
	cp ${SYSTEM_LIBRARY_DIR}/libcrypto.1.0.0.dylib ${APP_LIBRARY_DIR}
	chmod u+w ${APP_LIBRARY_DIR}/libcrypto.1.0.0.dylib
	install_name_tool -id @executable_path/../Frameworks/libcrypto.1.0.0.dylib ${APP_LIBRARY_DIR}/libcrypto.1.0.0.dylib
	install_name_tool -change ${SYSTEM_LIBRARY_DIR}/libz.1.dylib @executable_path/../Frameworks/libz.1.dylib ${APP_LIBRARY_DIR}/libcrypto.1.0.0.dylib


libffi-gcc47:
	cp ${SYSTEM_LIBRARY_DIR}/gcc47/libffi.4.dylib ${APP_LIBRARY_DIR}/gcc47
	chmod u+w ${APP_LIBRARY_DIR}/gcc47/libffi.4.dylib
	install_name_tool -id @executable_path/../Frameworks/gcc47/libffi.4.dylib ${APP_LIBRARY_DIR}/gcc47/libffi.4.dylib
	install_name_tool -change ${SYSTEM_LIBRARY_DIR}/gcc47/libgcc_s.1.dylib @executable_path/../Frameworks/gcc47/libgcc_s.1.dylib ${APP_LIBRARY_DIR}/gcc47/libffi.4.dylib 

libgomp:
	cp ${SYSTEM_LIBRARY_DIR}/gcc47/libgomp.1.dylib ${APP_LIBRARY_DIR}/gcc47
	chmod u+w ${APP_LIBRARY_DIR}/gcc47/libgomp.1.dylib
	install_name_tool -id @executable_path/../Frameworks/gcc47/libgomp.1.dylib ${APP_LIBRARY_DIR}/gcc47/libgomp.1.dylib
	install_name_tool -change ${SYSTEM_LIBRARY_DIR}/gcc47/libgcc_s.1.dylib @executable_path/../Frameworks/gcc47/libgcc_s.1.dylib ${APP_LIBRARY_DIR}/gcc47/libgomp.1.dylib 
		
libgcc_s:
	cp ${SYSTEM_LIBRARY_DIR}/gcc47/libgcc_s.1.dylib ${APP_LIBRARY_DIR}/gcc47
	chmod u+w ${APP_LIBRARY_DIR}/gcc47/libgcc_s.1.dylib
	install_name_tool -id @executable_path/../Frameworks/gcc47/libgcc_s.1.dylib ${APP_LIBRARY_DIR}/gcc47/libgcc_s.1.dylib

libstdc++:
	cp ${SYSTEM_LIBRARY_DIR}/libstdc++.6.dylib ${APP_LIBRARY_DIR}
	chmod u+w ${APP_LIBRARY_DIR}/libstdc++.6.dylib
	install_name_tool -id @executable_path/../Frameworks/libstdc++.6.dylib ${APP_LIBRARY_DIR}/libstdc++.6.dylib

