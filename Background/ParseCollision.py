import sys

def getCollisionBinary(filename):
	collisionString = ""
	with open(filename) as f:
		for line in f:
			tokens = line.split(',')
			midLine = 0;
			if(len(tokens) == 17):
				tempNum1 = ""
				for i in range(0, 8):
					num = tokens[i][2:]
					if(num == "08"):
						tempNum1 += "0"
					else:
						tempNum1 += "1"
				tempNum1 = hex(int(tempNum1, 2))[2:].zfill(2)
				tempNum2 = ""
				for i in range(8, 16):
					num = tokens[i][2:]
					if(num == "08"):
						tempNum2 += "0"
					else:
						tempNum2 += "1"
				tempNum2 = hex(int(tempNum2, 2))[2:].zfill(2)

				collisionString += "0x" + tempNum1 + ", 0x" + tempNum2 + ", "
		return collisionString

def getCollision(filename):
	collisionString = ""
	with open(filename) as f:
		lineNum = 0
		numValues = 0
		for line in f:
			tokens = line.split(',')
			if(len(tokens) == 17  or len(tokens) == 16):
				for i in range(0, 16):
					# Only want the top left of ever 2x2 block
					# 2 lines of .h file are one line of screen so want lines 1, 2, but skip 3, 4, and so on
					if(((lineNum/2) % 2 == 0) and (i % 2 == 0)):
						tempNum1 = ""
						num = tokens[i][2:]
						if(num == "08"):
							tempNum1 += "0"
						else:
							tempNum1 += "1"

						tempNum1 = hex(int(tempNum1, 2))[2:].zfill(2)
						collisionString += "0x" + tempNum1 + ", "
						numValues += 1
				lineNum += 1
		print("numValues: " )
		print(numValues)
		return collisionString

level = sys.argv[1]
output = "const unsigned char collision" + level + "[] = {";

output += getCollision("Level" + level + "TopCollision.h")
output += getCollision("Level" + level + "BottomCollision.h")

output += "};"
print(output)
