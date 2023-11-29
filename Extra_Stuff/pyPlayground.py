statusDict = {
    "solved":["dial"]
}

puzzles = [
    {"name":"dial", "ip":"FILL IN", "port":1234},
    {"name":"bust", "ip":"FILL IN", "port":1234},
    {"name":"plugboard", "ip":"FILL IN", "port":1234},
    {"name":"potentiometer", "ip":"FILL IN", "port":1234},
    {"name":"cant remember lol", "ip":"FILL IN", "port":1234}
]

newStatusDict = {
    "solved":["bust"]
}


solvedList = statusDict.get("solved")
solvedList.append("bust") if "bust" not in solvedList else solvedList

print(statusDict.get("solved"))