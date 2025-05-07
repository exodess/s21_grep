#! /bin/bash

if [ -e ~/s21_grep ]
	then
		echo ""
	else 
		echo "Для начала соберите проект: make"
		exit
fi

echo "Запуск тестирования новой утилиты s21_grep..."
read -p "Введите файл(-ы), на котором будет происходить тестирование: " test_file

read -p "Введите шаблон, по которому стоит искать информацию: " templates

file_s21="outgrep21.txt"
file_grep="outgrep.txt"

n=1
succ=0
fail=0
for flag in "e" "i" "v" "c" "l" "n" "h" "s" "o" "f" \
			"vi" "vos" "voh" "hos" "hlo" "ivsoh" "eiflff"
	do 
		# echo -ne "TEST $n. Flag = $flag\t:"
		echo "Команда: s21_grep -$flag $templates $test_file &> $file_s21"
		~/s21_grep -$flag $templates $test_file &> $file_s21 
		echo "Команда: grep -$flag $templates $test_file &> $file_grep"
		grep -$flag $templates $test_file &> $file_grep
		echo -n "Результат: "
		
		result=$(diff $file_s21 $file_grep)
		len=${#result}
		if [[ len -eq 0 ]]
			then 
				echo -e  "\033[32m" SUCCESFULL" \033[0m\n"
				succ=$((succ + 1))
			else 
				echo -e "\033[31m" "   FAIL" "\033[0m\n"
				fail=$((fail + 1))
		fi
		
		# echo $result
		n=$((n + 1))
	done

percent=$(($((succ * 100)) / $((succ + fail))))
echo ""
echo "Успехов: $succ, неудач: $fail"
echo "Прохождение теста: $percent%"

rm $file_s21
rm $file_grep
echo "Тесты завершены!"
exit 0 
