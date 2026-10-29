"""
Each level consists of shapes polygons, lines (maybe circles if i implement it later). they are stored in a list.
For polygons, the list contains lists of points. These points define the vertices of the polygons.
        If a polygon's list start with string "x", then it means this polygon cannot be destroyed.
For lines, the list contains tuples, defining the start and end position of a line.

"""
levels = [
# newlevel@ 28-Oct-2026 12:46:13PM
{
"polygons" : [
    [(118, 492), (118, 625), (262, 625), (262, 492)],
    [(459, 492), (459, 625), (603, 625), (603, 492)],
  ],
"lines" : [
  ]
},

# newlevel@ 28-Oct-2026 12:47:25PM
{
"polygons" : [
    [(118, 428), (118, 561), (262, 561), (262, 428)],
    [(459, 428), (459, 561), (603, 561), (603, 428)],
    [(290, 681), (290, 814), (434, 814), (434, 681)],
  ],
"lines" : [
  ]
},
# newlevel@ 28-Oct-2026 01:03:14PM
{
"polygons" : [
    [(110, 289), (110, 413), (230, 413), (230, 289)],
    [(485, 682), (485, 806), (605, 806), (605, 682)],
    ['x', (105, 684), (105, 814), (235, 814)],
  ],
"lines" : [
  ]
},


# newlevel@ 28-Oct-2026 01:05:12PM
{
"polygons" : [
    [(110, 445), (110, 569), (230, 569), (230, 445)],
    [(485, 446), (485, 570), (605, 570), (605, 446)],
    ['x', (315, 415), (315, 716), (375, 716), (375, 415)],
    [(255, 866), (435, 866), (435, 926), (255, 926)],
  ],
"lines" : [
  ]
},


# newlevel@ 28-Oct-2026 01:10:01PM
{
"polygons" : [
    [(195, 355), (105, 445), (195, 535), (285, 445)],
    [(193, 665), (103, 755), (193, 845), (283, 755)],
    [(477, 351), (387, 441), (477, 531), (567, 441)],
    [(475, 661), (385, 751), (475, 841), (565, 751)],
  ],
"lines" : [
  ]
},


# newlevel@ 28-Oct-2026 01:20:09PM
{
"polygons" : [
    ['x', (285, 565), (285, 656), (375, 656), (375, 565)],
    [(476, 354), (386, 444), (476, 534), (566, 444)],
    [(474, 681), (384, 771), (474, 861), (564, 771)],
    [(195, 683), (105, 773), (195, 863), (285, 773)],
  ],
"lines" : [
  ]
},


# newlevel@ 28-Oct-2026 01:23:40PM
{
"polygons" : [
    [(302, 345), (302, 465), (422, 465), (422, 345)],
    [(304, 641), (304, 761), (424, 761), (424, 641)],
    ['x', (92, 499), (92, 619), (212, 619), (212, 499)],
  ],
"lines" : [
    [(275, 555), (455, 555)], 
  ]
},


# newlevel@ 28-Oct-2026 01:29:44PM
{
"polygons" : [
    [(121, 243), (121, 393), (121, 393), (271, 393), (271, 243)],
    [(483, 813), (483, 963), (483, 963), (633, 963), (633, 813)],
    [(275, 553), (275, 703), (275, 703), (425, 703), (425, 553)],
    [(403, 239), (403, 389), (403, 389), (553, 389), (553, 239)],
  ],
"lines" : [
  ]
},


# newlevel@ 28-Oct-2026 01:39:23PM
{
"polygons" : [
    [(148, 415), (268, 415), (148, 535)],
    [(420, 415), (540, 415), (540, 535)],
    [(542, 617), (542, 740), (419, 740)],
    [(150, 621), (150, 741), (270, 741)],
  ],
"lines" : [
    [(345, 505), (345, 686)], 
    [(255, 596), (435, 596)], 
  ]
},

]


for  level in (levels):
	print("//", "#" * 15, "level", "#"*15)
	print("{")
	print("    // lvl polygons: ")
	print("    {")
	for polygon in level["polygons"]:
		print("        {", end="")
		for i, pt in enumerate(polygon, 1):
			end = ", " if i != len(polygon) else ""
			if pt != "x":
				print("V2f", f"({pt[0]}.f, {pt[1]}.f)", sep="", end=end)
			else:
				print("SOLID_V2F", end=", ")
		print("},") 
	
	print("    },")
	
	print()
	print("    // lvl  lines: ")
	print("    {")
	for i, line in enumerate(level["lines"], 1) :
		print("        {", end="")
		for x, pt in enumerate(line, 1) :
			end = ", " if x!=len(pt) else ""
			print("V2f", f"({pt[0]}.f, {pt[1]}.f)", sep="", end=end)
		print("}, ")
		
	print("    }")
	print("}, ")
	print("\n")
	


