from lxml import etree
from io import StringIO

doc = etree.parse('records.xml')
root = doc.getroot()
#print(etree.tostring(root))
xmlschema_doc = etree.parse('records.xsd')
xmlschema = etree.XMLSchema(xmlschema_doc)
doc = etree.XML(etree.tostring(root))
validation_result = xmlschema.validate(doc)
print(validation_result)
xmlschema.assert_(doc)