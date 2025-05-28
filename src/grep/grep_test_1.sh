#!/bin/bash
SUCCESS=0
FAIL=0
COUNTER=0

files=("text1.txt" "text2.txt")
flags=("--number-nonblank" "--number" "--squeeze-blank" "-E" "-T")

for file in "${files[@]}"
do
    for flag1 in "${flags[@]}"
    do
        for flag2 in "${flags[@]}"
        do
            echo -e "\n$flag1 $flag2 -v ${files[0]} ${files[1]}:"
            
            
            s21_cat_output=$(./s21_cat "$flag1" "-v" "$flag2" "${files[0]}" "${files[1]}")
            echo "$s21_cat_output" > s21cat_output.txt

    
            cat_output=$(cat "$flag1" "$flag2" "-v" "${files[0]}" "${files[1]}")
            echo "$cat_output" > cat_output.txt

            
            if diff -q s21cat_output.txt cat_output.txt >/dev/null 2>&1;
            then
                let "COUNTER++"
                let "SUCCESS++"
                echo "$COUNTER - Success"
            else 
                let "COUNTER++"
                let "FAIL++"
                echo "$COUNTER - Fail"
                echo -e "Your s21_cat output:\n$s21_cat_output"
                echo -e "\n\nCat output:\n$cat_output"
                echo -e "\n"
            fi

            rm s21cat_output.txt
            rm cat_output.txt
        done
    done
done

echo -e "\nSuccess = $SUCCESS"
echo "Fail = $FAIL"
SUCCESS=0
FAIL=0
COUNTER=0

files=("text1.txt" "text2.txt")
flags=("--number-nonblank" "--number" "--squeeze-blank" "-E" "-T")

for file in "${files[@]}"
do
    for flag1 in "${flags[@]}"
    do
        for flag2 in "${flags[@]}"
        do
            echo -e "\n$flag1 $flag2 -v ${files[0]} ${files[1]}:"
            
            
            s21_cat_output=$(./s21_cat "$flag1" "-v" "$flag2" "${files[0]}" "${files[1]}")
            echo "$s21_cat_output" > s21cat_output.txt

    
            cat_output=$(cat "$flag1" "$flag2" "-v" "${files[0]}" "${files[1]}")
            echo "$cat_output" > cat_output.txt

            
            if diff -q s21cat_output.txt cat_output.txt >/dev/null 2>&1;
            then
                let "COUNTER++"
                let "SUCCESS++"
                echo "$COUNTER - Success"
            else 
                let "COUNTER++"
                let "FAIL++"
                echo "$COUNTER - Fail"
                echo -e "Your s21_cat output:\n$s21_cat_output"
                echo -e "\n\nCat output:\n$cat_output"
                echo -e "\n"
            fi

            rm s21cat_output.txt
            rm cat_output.txt
        done
    done
done

echo -e "\nSuccess = $SUCCESS"
echo "Fail = $FAIL"
