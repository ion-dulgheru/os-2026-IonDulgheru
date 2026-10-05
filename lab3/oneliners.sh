# Q1. How many requests failed?
grep -c FAIL access.log
# Q2. How many different pages were requested, and which?
cut -d' ' -f5 access.log | sort -u | wc -l ; cut -d' ' -f5 access.log | sort -u
# Q3. How many requests did each page get?
cut -d' ' -f5 access.log | sort | uniq -c
# Q4. Which user(s) have the most failed requests, and how many?
grep FAIL access.log | cut -d' ' -f3 | sort | uniq -c | sort -rn | head -3
# Q5. How many requests did user3 make, and how many failed?
grep -c user3 access.log ; grep user3 access.log | grep -c FAIL
# Q6. Last 3 failed requests, only time and user
grep FAIL access.log | tail -3 | cut -d' ' -f2,3
# Q7. Login shells in /etc/passwd and how many accounts use each
cut -d: -f7 /etc/passwd | sort | uniq -c | sort -rn
# Q8. ls -l prints an extra "total N" line before the entries
ls /etc | wc -l ; ls -l /etc | wc -l
