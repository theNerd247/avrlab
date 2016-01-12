cmake_minimum_required(VERSION 2.6)
#acknowledge policy push/pop
cmake_policy(SET CMP0011 NEW) 

include(LaTeXBuild)

project("report")

set(mainTexFile "main.tex")

set(texDir "tex")
set(styDir "sty")
set(imgDir "img")

set(subProjects 
	"lab2"
	"lab3"
	"lab4"
	"lab5"
	)

set(styleFiles "${styDir}/labreport.sty")

set(includeList 
	"${texDir}"
	"${styDir}"
	"${imgDir}"
	"plot"
	)

SetupLaTexBuildEnv (
	# the main tex file
	"${mainTexFile}" 
	# the file search paths this project provides
	"${includeList}" 
	# the style files this project provides
	"${styleFiles}"
	# projects this project depends on
	"${subProjects}"
	)
