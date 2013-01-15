man:
	xsltproc /usr/share/xml/docbook/stylesheet/docbook-xsl/manpages/docbook.xsl ina-reference.xml 

pdf:
	xsltproc --output=ina-reference.fo /usr/share/xml/docbook/stylesheet/docbook-xsl/fo/docbook.xsl ina-reference.xml 
	fop ina-reference.fo
