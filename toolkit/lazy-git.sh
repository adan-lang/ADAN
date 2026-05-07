chmod +x ./format.sh
bash ./format.sh

git add .
git commit -m "upd: lazily push (date: $(date))"
git push --force