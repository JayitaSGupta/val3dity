#!/usr/bin/env python
import sys

def main():
  if len(sys.argv) == 1:
    print "No input shell(s) given"
    sys.exit()
  header_stuff()
  lsShells = []
  processoneshell(open(sys.argv[1], 'r'), True)
  for f in sys.argv[2:]:
    processoneshell(open(f, 'r'), False)
  footer_stuff()

def processoneshell(s, exterior=True):  
  nov = int(s.readline().split()[0])
  lsPts = []
  for i in range(nov):
    lsPts.append(map(float, s.readline().split()[1:4]))
  if (exterior == True):
    print '\t\t\t\t\t<gml:exterior>'
  else:
    print '\t\t\t\t\t<gml:interior>'

  print '\t\t\t\t\t\t<gml:CompositeSurface>'
  nof = int(s.readline().split()[0])
  for i in range(nof):
    print '\t\t\t\t\t\t\t<gml:surfaceMember>'
    print '\t\t\t\t\t\t\t\t<gml:Polygon>'
    print '\t\t\t\t\t\t\t\t\t<gml:exterior>'
    print '\t\t\t\t\t\t\t\t\t\t<gml:LinearRing>'
    a = map(int, s.readline().split())
    oring = map(int, s.readline().split())
    oring.pop(0)
    poly = []
    for each in oring:
      poly.append('\t\t\t\t\t\t\t\t\t\t\t<gml:pos>' + str(lsPts[each][0]) + " " + str(lsPts[each][1]) + " " + str(lsPts[each][2])  + '</gml:pos>')
    poly.append('\t\t\t\t\t\t\t\t\t\t\t<gml:pos>' + str(lsPts[oring[0]][0]) + " " + str(lsPts[oring[0]][1]) + " " + str(lsPts[oring[0]][2])  + '</gml:pos>')
    print "\n".join(poly)
    print '\t\t\t\t\t\t\t\t\t\t</gml:LinearRing>'
    print '\t\t\t\t\t\t\t\t\t</gml:exterior>'    
    
    if a[0] > 1:
      print '\t\t\t\t\t\t\t\t\t<gml:interior>'
      print '\t\t\t\t\t\t\t\t\t\t<gml:LinearRing>'
      for j in range(a[0]-1):
        iring = map(int, s.readline().split())
        iring.pop(0)
        poly = []
        for each in iring:
          poly.append('\t\t\t\t\t\t\t\t\t\t\t<gml:pos>' + str(lsPts[each][0]) + " " + str(lsPts[each][1]) + " " + str(lsPts[each][2])  + '</gml:pos>')
        poly.append('\t\t\t\t\t\t\t\t\t\t\t<gml:pos>' + str(lsPts[iring[0]][0]) + " " + str(lsPts[iring[0]][1]) + " " + str(lsPts[iring[0]][2])  + '</gml:pos>')
        print "\n".join(poly)
        s.readline().split() #--pass that line
      print '\t\t\t\t\t\t\t\t\t\t</gml:LinearRing>'
      print '\t\t\t\t\t\t\t\t\t</gml:interior>'
    print '\t\t\t\t\t\t\t\t</gml:Polygon>'    
    print '\t\t\t\t\t\t\t</gml:surfaceMember>'
  print '\t\t\t\t\t\t</gml:CompositeSurface>'
  if (exterior == True):
    print '\t\t\t\t\t</gml:exterior>'
  else:
    print '\t\t\t\t\t</gml:interior>'


    
def header_stuff():
  print '<?xml version="1.0" encoding="utf-8"?>'
  # print '<CityModel xmlns="http://www.citygml.org/citygml/1/0/0" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:xlink="http://www.w3.org/1999/xlink" xmlns:gml="http://www.opengis.net/gml" xsi:schemaLocation="http://www.citygml.org/citygml/1/0/0 http://www.citygml.org/citygml/1/0/0/CityGML.xsd">'
  # print '<CityModel xmlns:gml="http://www.opengis.net/gml" xmlns:bldg="http://www.opengis.net/citygml/building/2.0" xmlns:xlink="http://www.w3.org/1999/xlink" xmlns:xAL="urn:oasis:names:tc:ciq:xsdschema:xAL:2.0" xmlns:dem="http://www.opengis.net/citygml/relief/2.0" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns="http://www.opengis.net/citygml/2.0">'
  print '<CityModel xmlns:smil20="http://www.w3.org/2001/SMIL20/" xmlns:grp="http://www.opengis.net/citygml/cityobjectgroup/1.0" xmlns:pfx0="http://www.citygml.org/citygml/profiles/base/1.0" xmlns:luse="http://www.opengis.net/citygml/landuse/1.0" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:frn="http://www.opengis.net/citygml/cityfurniture/1.0" xmlns:smil20lang="http://www.w3.org/2001/SMIL20/Language" xmlns:xlink="http://www.w3.org/1999/xlink" xmlns:tex="http://www.opengis.net/citygml/texturedsurface/1.0" xmlns:dem="http://www.opengis.net/citygml/relief/1.0" xmlns:tran="http://www.opengis.net/citygml/transportation/1.0" xmlns:wtr="http://www.opengis.net/citygml/waterbody/1.0" xmlns:xAL="urn:oasis:names:tc:ciq:xsdschema:xAL:2.0" xmlns:bldg="http://www.opengis.net/citygml/building/1.0" xmlns:sch="http://www.ascc.net/xml/schematron" xmlns:app="http://www.opengis.net/citygml/appearance/1.0" xmlns:veg="http://www.opengis.net/citygml/vegetation/1.0" xmlns:gml="http://www.opengis.net/gml" xmlns:gen="http://www.opengis.net/citygml/generics/1.0" xmlns="http://www.opengis.net/citygml/1.0" xsi:schemaLocation="http://www.opengis.net/citygml/1.0 http://schemas.opengis.net/citygml/1.0/cityGMLBase.xsd  http://www.opengis.net/citygml/appearance/1.0 http://schemas.opengis.net/citygml/appearance/1.0/appearance.xsd http://www.opengis.net/citygml/building/1.0 http://schemas.opengis.net/citygml/building/1.0/building.xsd http://www.opengis.net/citygml/cityfurniture/1.0 http://schemas.opengis.net/citygml/cityfurniture/1.0/cityFurniture.xsd http://www.opengis.net/citygml/vegetation/1.0 http://schemas.opengis.net/citygml/vegetation/1.0/vegetation.xsd http://www.opengis.net/citygml/generics/1.0 http://schemas.opengis.net/citygml/generics/1.0/generics.xsd http://www.opengis.net/citygml/transportation/1.0 http://schemas.opengis.net/citygml/transportation/1.0/transportation.xsd http://www.opengis.net/citygml/waterbody/1.0 http://schemas.opengis.net/citygml/waterbody/1.0/waterBody.xsd http://www.opengis.net/citygml/landuse/1.0 http://schemas.opengis.net/citygml/landuse/1.0/landUse.xsd http://www.opengis.net/citygml/relief/1.0 http://schemas.opengis.net/citygml/relief/1.0/relief.xsd">'
  print '\t<cityObjectMember>'
  print '\t\t<bldg:Building>'
  print '\t\t\t<bldg:lod1Solid>'
  print '\t\t\t\t<gml:Solid>'


def footer_stuff():
  print '\t\t\t\t</gml:Solid>'
  print '\t\t\t</bldg:lod1Solid>'
  print '\t\t</bldg:Building>'
  print '\t</cityObjectMember>'
  print '</CityModel>'

  
if __name__ == "__main__":
  main()
