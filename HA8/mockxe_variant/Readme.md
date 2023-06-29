``` 
$ ./webbrowser www.example.org /index.html > out.txt
$ dos2unix -q -O out.txt | sed -ne '/^$/,$p' | tee out.html
```
