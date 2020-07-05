#!/bin/bash

runner="./a.out"
gcc main.c
score=0
error=0

for value in {1..9}
do
	echo ""
	echo "Running 0${value}.code"
	timeout 5 ${runner} Cases/Correct/0${value}.code Cases/Correct/0${value}.data > Cases/Correct/0${value}.student
	echo "Running diff with 0${value}.expected"
	#Check for correct print
	tr -d '[:space:]' < Cases/Correct/0${value}.student > temp1
	tr -d '[:space:]' < Cases/Correct/0${value}.expected > temp2
	grep -o 'program.*end' temp1 > temp11
	grep -o 'program.*end' temp2 > temp22
	if cmp -s "temp11" "temp22"; then
		echo "Print looks good"
		score=$(($score + 1))
	else
		echo "Compare student print:"
		cat temp11
		echo "and expected print:"
		cat temp22
		read -n 1 -p "Award points anyway? (y/n)" mainmenuinput
		if [ $mainmenuinput = "y" ]; then
			echo -e "\npoints awarded"
			score=$(($score + 1))
		else
			echo -e "\npoints NOT awarded"
		fi
	fi
	#Check for correct output
	grep -o '^\-\?[[:digit:]]\+' Cases/Correct/0${value}.student > temp1
	grep -o '^\-\?[[:digit:]]\+' Cases/Correct/0${value}.expected > temp2
	if cmp -s "temp1" "temp2"; then
		echo "Output looks good"
		score=$(($score + 1))
	else
		echo "Difference in output detected. Student has:"
		cat temp1
		echo "expected:"
		cat temp2
	fi
done

for value in {10..26}
do
	echo ""
	echo "Running 0${value}.code"
	timeout 5 ${runner} Cases/Correct/${value}.code Cases/Correct/${value}.data > Cases/Correct/${value}.student
	echo "Running diff with ${value}.expected"
	#Check for correct print
	tr -d '[:space:]' < Cases/Correct/${value}.student > temp1
	tr -d '[:space:]' < Cases/Correct/${value}.expected > temp2
	grep -o 'program.*end' temp1 > temp11
	grep -o 'program.*end' temp2 > temp22
	if cmp -s "temp11" "temp22"; then
		echo "Print looks good"
		score=$(($score + 1))	
	else
		echo "Compare student print:"
		cat temp11
		echo "and expected print:"
		cat temp22
		read -n 1 -p "Award points anyway? (y/n)" mainmenuinput
		if [ $mainmenuinput = "y" ]; then
			echo -e "\npoints awarded"
			score=$(($score + 1))
		else
			echo -e "\npoints NOT awarded"
		fi
	fi
	#Check for correct output
	grep -o '^\-\?[[:digit:]]\+' Cases/Correct/${value}.student > temp1
	grep -o '^\-\?[[:digit:]]\+' Cases/Correct/${value}.expected > temp2
	if cmp -s "temp1" "temp2"; then
		echo "Output looks good"
		score=$(($score + 1))
	else
		echo "Difference in output detected. Student has:"
		cat temp1
		echo "expected:"
		cat temp2
	fi
done

rm temp1
rm temp2
rm temp11
rm temp22

echo "Running error cases:"
echo ""
echo "Running 01.error:"
timeout 5 ${runner} Cases/Error/01.code Cases/Correct/01.data
read -n 1 -p "Error is ++ in expression. Error message related to that? (y/n)" mainmenuinput
if [ $mainmenuinput = "y" ]; then
	echo -e "\nscore updated"
	error=$(($error + 1))
else
	echo -e "\nscore NOT updated"
fi
echo ""
echo "Running 02.error:"
timeout 5 ${runner} Cases/Error/02.code Cases/Correct/02.data
read -n 1 -p "Error is uninitialized variable. Error message related to that? (y/n)" mainmenuinput
if [ $mainmenuinput = "y" ]; then
	echo -e "\nscore updated"
	error=$(($error + 1))
else
	echo -e "\nscore NOT updated"
fi
echo ""
echo "Running 03.error:"
timeout 5 ${runner} Cases/Error/03.code Cases/Correct/03.data
read -n 1 -p "Error is variable declared twice. Error message related to that? (y/n)" mainmenuinput
if [ $mainmenuinput = "y" ]; then
	echo -e "\nscore updated"
	error=$(($error + 1))
else
	echo -e "\nscore NOT updated"
fi
echo ""
echo "Running 04.error:"
timeout 5 ${runner} Cases/Error/04.code Cases/Correct/04.data
read -n 1 -p "Error is endif missing. Error message related to that? (y/n)" mainmenuinput
if [ $mainmenuinput = "y" ]; then
	echo -e "\nscore updated"
	error=$(($error + 1))
else
	echo -e "\nscore NOT updated"
fi
echo ""
echo "Running 05.error:"
timeout 5 ${runner} Cases/Error/05.code Cases/Correct/05.data
read -n 1 -p "Error is == in condition. Error message related to that? (y/n)" mainmenuinput
if [ $mainmenuinput = "y" ]; then
	echo -e "\nscore updated"
	error=$(($error + 1))
else
	echo -e "\nscore NOT updated"
fi
echo ""
echo "Running 06.error:"
timeout 5 ${runner} Cases/Error/06.code Cases/Correct/06.data
read -n 1 -p "Error is endif instead of endwhile. Error message related to that? (y/n)" mainmenuinput
if [ $mainmenuinput = "y" ]; then
	echo -e "\nscore updated"
	error=$(($error + 1))
else
	echo -e "\nscore NOT updated"
fi
echo ""
echo "Running 07.error:"
timeout 5 ${runner} Cases/Error/07.code Cases/Correct/07.data
read -n 1 -p "Error is ids after end. Error message related to that? (y/n)" mainmenuinput
if [ $mainmenuinput = "y" ]; then
	echo -e "\nscore updated"
	error=$(($error + 1))
else
	echo -e "\nscore NOT updated"
fi
echo ""
echo "Running 08.error:"
timeout 5 ${runner} Cases/Error/08.code Cases/Correct/08.data
read -n 1 -p "Error is begin keyword missing. Error message related to that? (y/n)" mainmenuinput
if [ $mainmenuinput = "y" ]; then
	echo -e "\nscore updated"
	error=$(($error + 1))
else
	echo -e "\nscore NOT updated"
fi
echo ""
echo "Running 09.error:"
timeout 5 ${runner} Cases/Error/09.code Cases/Correct/09.data
read -n 1 -p "Error is missing semicolon. Error message related to that? (y/n)" mainmenuinput
if [ $mainmenuinput = "y" ]; then
	echo -e "\nscore updated"
	error=$(($error + 1))
else
	echo -e "\nscore NOT updated"
fi
echo ""
echo "Running 10.error:"
timeout 5 ${runner} Cases/Error/10.code Cases/Correct/10.data
read -n 1 -p "Error is missing right parenthesis. Error message related to that? (y/n)" mainmenuinput
if [ $mainmenuinput = "y" ]; then
	echo -e "\nscore updated"
	error=$(($error + 1))
else
	echo -e "\nscore NOT updated"
fi

echo ""
echo "Calculating scores"
score=$(($score + 13))
error=$(($error * 2))
echo "Correct cases score (out of 65):"
echo $score
echo "Error cases score (out of 20):"
echo $error

echo "Done!"