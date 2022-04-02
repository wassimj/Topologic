import sys
import os
from os.path import expanduser
home = expanduser("~")
release = os.path.join(home, "topologicbim", "Topologic", "output", "x64", "Release")
print(release)
sys.path.append(release)

from topologic import Vertex, Topology, Dictionary, Attribute, IntAttribute, DoubleAttribute, StringAttribute

print("Starting Test...")
# Create an Integer Value
print("Creating an Integer Value: 340")
i = 340
intAttribute = IntAttribute(i)
print(intAttribute, intAttribute.IntValue(), " should be equal to ", i)
print("Done")

# Create a Double Value
print("Creating a Double Value: 120.567")
d = 120.567
doubleAttribute = DoubleAttribute(d)
print(doubleAttribute, doubleAttribute.DoubleValue(), " should be equal to ", d)
print("Done")

# Create a String Value
print("Creating a String Value: Hello Topologic World")
s = "Hello Topologic World"
stringAttribute = StringAttribute(s)
print(stringAttribute, stringAttribute.StringValue(), " should be equal to ", s)
print("Done")

print("Creating a Dictionary and assigning to a Topology")
values = [intAttribute, doubleAttribute, stringAttribute]
keys = ["int", "double", "string"]

mydict = Dictionary.ByKeysValues(keys, values)
v = Vertex.ByCoordinates(0,0,0)

# Assign Dictionary
v.SetDictionary(mydict)
print("Done")

print("Retrieving a Dictionary from a Topology")
# Retrieve Dictionary
d_back = v.GetDictionary()
print("Done")

print("Retrieving attributes and values from a Dictionary")
# Retrieve Values from Dictionary
newIntAttribute = d_back.ValueAtKey(keys[0])
newDoubleAttribute = d_back.ValueAtKey(keys[1])
newStringAttribute = d_back.ValueAtKey(keys[2])
print("Done")
print(newIntAttribute, newIntAttribute.IntValue(), " should be equal to ", i, ": ", (newIntAttribute.IntValue()==i))
print(newDoubleAttribute, newDoubleAttribute.DoubleValue(), " should be equal to ", d, ": ", (newDoubleAttribute.DoubleValue()==d))
print(newStringAttribute, newStringAttribute.StringValue(), " should be equal to ", s, ": ", (newStringAttribute.StringValue()==s))
print("Done")
if (newIntAttribute.IntValue()==i) and (newDoubleAttribute.DoubleValue()==d) and (newStringAttribute.StringValue()==s):
    print("Congratulations! Test is successful.")
else:
    print("Test failed! Please contact software author.")