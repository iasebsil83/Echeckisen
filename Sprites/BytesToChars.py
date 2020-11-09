#Create a character coded file (text file) from a byte coded one
def sizeIn3(i):
    if i > 99:
        return str(i)
    elif i > 9:
        return "0" + str(i)
    return "00" + str(i)

oldPath = "Byte_rgba/"
newPath = "Text_rgba/"

while True:
    name = input("Name of the file to convert > ")
    try:
        f = open(oldPath + name,"rb")
        data = f.read()
        f.close()
    except:
        print("Unable to open file " + oldPath + name)
        continue

    toWrite = ""
    for a in data:
        toWrite += sizeIn3(a)

    f = open(newPath + name,"w")
    f.write(toWrite)
    f.close()
    print("Successfully converted from {0} to {1}".format(oldPath,newPath))
